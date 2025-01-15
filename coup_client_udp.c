#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



int main(int argc, char *argv[]) {
   
    

    int client_socket;
    struct sockaddr_in server_addr;
    char send_buffer[1024];
    char receive_buffer[1024];

    // 클라이언트 소켓 생성
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); 

    // 통신 시작
    while (1) {
        // 데이터 전송
        printf("Enter a message: ");
        fgets(send_buffer, sizeof(send_buffer), stdin);
        sendto(client_socket, send_buffer, strlen(send_buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        // 데이터 수신
        memset(receive_buffer, 0, sizeof(receive_buffer));
        recvfrom(client_socket, receive_buffer, sizeof(receive_buffer), 0, NULL, NULL);

        printf("Server says: %s\n", receive_buffer);
    }

    // 클라이언트 소켓 닫기
    close(client_socket);

    return 0;
}