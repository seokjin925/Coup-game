#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/select.h>


#define MAX 30

char *HELP = "Help";

int Socket(int family, int type, int protocol);


int main(int argc, char *argv[]) {
    struct sockaddr_in connect_addr, from_addr;
    int connect_fd;
    int msgsize;

    char name[MAX] = {0};
    char msg[1024] = {0};
    socklen_t adr_sz;

    fd_set readfd;
    int temp_index;

    printf("****************************************************\n");
    printf("************Welcome to Coup Game *******************\n");
    printf("****************************************************\n");

    printf(" Enter User Name=> (name player*) ex) seokjin player1\n");
    memset(name, '\0', sizeof(name));

    /* 사용자 대화명 입력처리 */
    fgets(name, MAX, stdin);
    name[strlen(name)-1]='\0';

    memset(msg, '\0', sizeof(msg));

    for (temp_index = 0; temp_index < strlen(name); temp_index++) {
        msg[temp_index] = name[temp_index];
    }

    /* (1) 클라이언트 기본설정 */
    connect_fd = Socket(PF_INET, SOCK_DGRAM, 0);
    memset(&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = htons(atoi(argv[2])); // 변경: 포트 번호를 상수로 설정
    connect_addr.sin_addr.s_addr = inet_addr(argv[1]); // 변경: IP 주소를 명령행 인자로 사용하지 않음


    sendto(connect_fd, msg, sizeof(msg), 0, (struct sockaddr*)&connect_addr, sizeof(connect_addr));
    adr_sz=sizeof(from_addr);		

    memset(msg, '\0', sizeof(msg));

    /* 채팅 서버에 대한 접속 결과 확인 */
    recvfrom(connect_fd, msg, sizeof(msg), 0, (struct sockaddr*)&from_addr, &adr_sz);

    if (!strcmp(msg, "Connect OK")) {
        printf("************Server Connection Success*************\n");
        printf("************ Start Chatting Program  *************\n");
    }
    if (!strcmp(msg, "User Count Overflow Error")) {
        printf("%s\n", msg);
        exit(1);
    }

    /* 수신 데이터의 동시처리 */
    FD_ZERO(&readfd);

    while (1) {

        /* (2) 수신데이터 및 송신 데이터 처리 */
        FD_SET(0, &readfd);
        FD_SET(connect_fd, &readfd);

        select(connect_fd + 1, &readfd, NULL, NULL, NULL);
        if (FD_ISSET(0, &readfd)) {

            memset(msg, '\0', sizeof(msg));

            fgets(msg, sizeof(msg), stdin);
			msg[strcspn(msg, "\n")+1] = '\0';
            msg[strlen(msg) - 1] = '[';
            strcat(msg, name);
            msg[strlen(msg) ] = ']';
            msgsize = strlen(msg);
            sendto(connect_fd, msg, sizeof(msg), 0, (struct sockaddr*)&connect_addr, sizeof(connect_addr));
			continue;
        }

      
            memset(msg, '\0', sizeof(msg));
            adr_sz = sizeof(from_addr);
            recvfrom(connect_fd, msg, sizeof(msg), 0, (struct sockaddr*)&from_addr, &adr_sz);

            if (msgsize <= 0)
                continue;
            printf("server : %s \n", msg);
        
        
    }
    return 0;
}



int Socket(int family, int type, int protocol) {
    int result = 0;
    result = socket(family, type, protocol);
    if (result == -1) {
        perror("Socket Constructing Error"); // 변경: perror를 사용하여 오류 출력
        exit(0);
    }
    return result;
}
