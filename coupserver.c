#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/select.h>
#include<stdlib.h>

#define SERVER_PORT 5000

// 직업 구조체
   struct character {
    int duke;
    int assassin;
    int ambassador;
    int captain;
    int contessa;
};

// 관리자 구조체
struct administrator {
    int bankaccount;
    struct character admin_character;
};
// 유저 구조체
   struct user_node{ 
      int user_bankaccount;
      int user_sockfd; 
      struct character user_character;
      int turn1;
      int turn2;
   };
   void income(struct user_node *player, int index);
   void foreign_aid(struct user_node *player, int index);
   void coup(struct user_node *player1, struct user_node *player2, int index1,int index2);
   void duke(struct user_node *player1, int index1);
   void captain(struct user_node *player1, struct user_node *player2, int index1,int index2);
   void assassin(struct user_node *player1, struct user_node *player2, int index1, int index2);
   void ambassador(struct user_node *player1, int index1,struct administrator *admin);
   void challenge(struct user_node *player1, struct user_node *player2, int index1, int index2,int character);
   int Socket(int family, int type, int protocol);
   void Bind(int sockfd, const struct sockaddr *socketaddr, socklen_t addrlen);
   void Listen(int sockfd, int backlog);
   int Accept(int sockfd, struct sockaddr* Cliaddr, socklen_t *addelen);
   void Help(struct user_node *player1, int index1);

   int main(int argc, char *argv[])
   {
      struct user_node player[4];
      struct administrator admin;
      int server_socket;
      int client_socket;
      struct sockaddr_in server_addr;
      struct sockaddr_in client_addr;
      int addrsize, msgsize;
      memset(&admin, 0, sizeof(struct administrator));
      for (int i = 0; i < 4; i++)
        memset(&player[i], 0, sizeof(struct user_node));
    

      char BUFF[1024] = {0};
      char act[1024] = {0};
      memset(act, '\0', sizeof(act));   
      fd_set readfd;

      int maxfd;
      int room_index;
      int user_index;
      int tempsockfd;
      int temp_user_count=0;
      int user_count;

      int duke =0, assassin =0, ambassador=0,captain=0,contessa=0;

      
       

         /*(1) 서버 기본설정*/
      server_socket = Socket(AF_INET,SOCK_STREAM,0);

      server_addr.sin_family=AF_INET;
      server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
      server_addr.sin_port=htons(atoi(argv[1]));

      memset(&(server_addr.sin_zero), 0, 8);

      Bind(server_socket,(struct sockaddr*)&server_addr, sizeof(struct sockaddr));
      /*(2) 서버 접속대기*/
      Listen(server_socket, 0);
      
      maxfd = server_socket;
      FD_ZERO(&readfd);
      while(1){
         /*(3) 서버에 접속한 클라이언트 소켓기술자를 FD_SET에 설정 */
         
         FD_SET(server_socket,&readfd);
         int readyDescriptors = select(maxfd+1, &readfd, NULL, NULL, NULL);
         if (readyDescriptors == -1) {
             perror("select error");
             // 추가적인 오류 처리를 원하면 이 부분에 추가하세요.
             exit(EXIT_FAILURE);}


         /*(4) 서버로의 접속이 있는 클라이언트에 대한 처리*/

         if(FD_ISSET(server_socket,&readfd)){
            addrsize = sizeof(struct sockaddr_in);
            memset(BUFF, '\0', sizeof(BUFF));      
            client_socket=Accept(server_socket,(struct sockaddr*)&server_addr, &addrsize);
            msgsize = read(client_socket, BUFF, sizeof(BUFF));
            if(msgsize <=0){
               printf("Enter user Error\n");
               continue;
            }
            printf("Recive Message: %s\n", BUFF);// 접속자 이름 출력
            player[user_count].user_sockfd = client_socket;
            player[user_count].turn1 = user_count;
            user_count++;
   
            memset(BUFF, '\0', sizeof(BUFF));      
            strncpy(BUFF, "Connect OK", sizeof(BUFF) - 1);
            write(client_socket,BUFF,strlen(BUFF));

         }
         for(user_index=0;user_index<4;user_index++){
         tempsockfd = player[user_index].user_sockfd;
            FD_SET(tempsockfd,&readfd);
            if(tempsockfd>maxfd)
               maxfd=tempsockfd;
            for (int i = 0; i < 4; i++) {
                write(player[i].user_sockfd, BUFF, sizeof(BUFF));
                }
         memset(BUFF,'\0',sizeof(BUFF));
         }
          for (user_index = 0; user_index < 4; user_index++) {
              for(int user_index2 =0;user_index2<4;user_index2++)
            {
            memset(BUFF, '\0', sizeof(BUFF));
            strncpy(BUFF, "It's your turn, do anything you want to act", sizeof(BUFF) - 1);
        
                 if (player[user_index2].turn1 == user_index2) 
                   {write(player[user_index2].user_sockfd, BUFF, sizeof(BUFF));
                 memset(BUFF, '\0', sizeof(BUFF));}
            memset(BUFF, '\0', sizeof(BUFF));
              read(player[user_index2].user_sockfd,BUFF,sizeof(BUFF));
              for (int i = 0; i < 4; i++) 
                    write(player[i].user_sockfd, BUFF, sizeof(BUFF));
            if (strcmp(BUFF, "duke") == 0)                //행동 체크용 변수 도전사용자정의함수 인자로 사용
               duke=1;
            else if (strcmp(BUFF, "assassin") == 0) 
                   assassin=2;
              else if (strcmp(BUFF, "ambassador") == 0) 
                 ambassador=3;
             else if (strcmp(BUFF, "captain") == 0) 
                 captain=4;
             else if (strcmp(BUFF, "contessa") == 0) 
                 contessa=5;
    
                  // 행동을 읽고 그대로 다시 뿌림
                  
           
            if (FD_ISSET(player[user_index2].user_sockfd, &readfd)) {
                tempsockfd = player[user_index2].user_sockfd;
                msgsize = read(tempsockfd, act, sizeof(act));
            memset(BUFF,'\0',sizeof(BUFF));
            strncpy(BUFF, "Is there anyone who restricts or challenges this action?", sizeof(BUFF) - 1);
            
            }
            for (int i = 0; i < 4; i++) {
                write(player[i].user_sockfd, BUFF, sizeof(BUFF));
                }
         memset(BUFF,'\0',sizeof(BUFF));
         // for(int i =0;i<4;i++)
         // {
         //    if(FD_ISSET(player[i].user_sockfd,&readfd))
            
         //       read(player[i].user_sockfd,BUFF,sizeof(BUFF));}
         //       도전 여부 확인 도전시 도전 사용자정의함수 실행
         //       저지시 행동한 사용자에게 도전여부 송신
         //       도전한다하면 저지에대한 도전사용자정의함수 실행

         //       위에 해당사항 없으면 행동 사용자정의함수 처리
            
         //       }
          }
            
      }
   }
   }

int Socket(int family, int type, int protocol)
{
            int result = 0;
            result = socket(family, type, protocol);
            if(result==-1){
               printf("Socket Contructing Error\n");
               exit(0);
            }
            return result;
}

void Bind(int sockfd, const struct sockaddr *socketaddr, socklen_t addrlen){
            int result = 0;
            result = bind(sockfd, socketaddr, addrlen);
            if(result ==-1){
               printf("Socket Binding Error\n");
               exit(0);
            }
            else {
               printf("Success Binding\n");
            }
}
         
void Listen(int sockfd, int backlog){
            int result = 0;
            result = listen(sockfd, backlog);
            if(result == -1){
               printf("Listening Error\n");
               exit(0);
            }
            else{
               printf("Success Listening\n");
            }
}
int Accept(int sockfd, struct sockaddr* cliaddr, socklen_t *addrlen){
            int result = 0;
            result=accept(sockfd, cliaddr, addrlen);
            if(result == -1){
               printf("Accept Error\n");
               exit(0);
            }
            else{
               printf("Success Accept\n");
            }
            return result;
}
void income(struct user_node *player, int index) {
    // 유효한 인덱스인지 확인
    if (index >= 0 && index < 4) {
        // 해당 인덱스의 bankaccount를 1 증가시킴
        player[index].user_bankaccount += 1;
    } else {
        // 인덱스가 유효하지 않을 경우 오류 처리 또는 경고 등을 수행
        printf("유효하지 않은 인덱스입니다.\n");
    }
}


void foreign_aid(struct user_node *player, int index){
    if (index >= 0 && index < 4) {
        // 해당 인덱스의 bankaccount를 1 증가시킴
        player[index].user_bankaccount += 2;
    } else {
        // 인덱스가 유효하지 않을 경우 오류 처리 또는 경고 등을 수행
        printf("유효하지 않은 인덱스입니다.\n");
}
}


void coup(struct user_node *player1, struct user_node *player2, int index1,int index2){
   char MSG[256];

   if(player1[index1].user_bankaccount < 7){
      memset(MSG, '\0', sizeof(MSG));
      strncpy(MSG, "은화가 부족합니다", sizeof(MSG)-1);
      write(player1[index1].user_sockfd,MSG,sizeof(MSG));
   }
   else{
      player1[index1].user_bankaccount -= 7;
      memset(MSG,'\0',sizeof(MSG));
      strncpy(MSG,"coup 당했습니다. 버릴 카드를 골라주세요",sizeof(MSG));
      write(player2[index2].user_sockfd,MSG,sizeof(MSG));
      memset(MSG,'\0',sizeof(MSG)-1);
      read(player2[index2].user_sockfd, MSG, sizeof(MSG));
      while(1){
         if (strcmp(MSG, "duke") == 0){
            player2[index2].user_character.duke--;
            break;}
         else if (strcmp(MSG, "assassin") == 0){
            player2[index2].user_character.assassin--;
            break;}
         else if (strcmp(MSG, "ambassador") == 0){
            player2[index2].user_character.ambassador--;
            break;}
         else if (strcmp(MSG, "captain") == 0){
            player2[index2].user_character.captain--;
            break;}
         else if (strcmp(MSG, "contessa") == 0){
            player2[index2].user_character.contessa--;
            break;}
         else{
            memset(MSG,'\0',sizeof(MSG));
            strncpy(MSG, "duke, assassin, ambassador, captain, contessa중 다시 고르세요!", sizeof(MSG) - 1);
            write(player2[index2].user_sockfd,MSG,sizeof(MSG));}
   }
}

void duke(struct user_node *player1, int index1){
   player1[index1].user_bankaccount += 3;
   
}

void captain(struct user_node *player1, struct user_node *player2, int index1,int index2){
   char MSG[256];
   while(1)
   {
         if(player2[index2].user_bankaccount >= 2)
            {player2[index2].user_bankaccount -= 2;
            player1[index1].user_bankaccount += 2;
            break;
            }
            
         else{
      
            strncpy(MSG, "상대 플레이어가 코인을 2원 미만으로 소지하고 있습니다 다른 플레이어를 골라주세요", sizeof(MSG) - 1);
            write(player1[index1].user_sockfd,MSG,sizeof(MSG)); // 턴을 진행 중인 유저   
            }
   }
}
//              암살시도 플레이어            암살대상플레이어        암살시도플레이어인데스, 암살대상플레이어 인덱스
void assassin(struct user_node *player1, struct user_node *player2, int index1, int index2)
{
   char MSG[256];
   if(player1[index1].user_bankaccount >= 3)
      player1[index1].user_bankaccount -= 3;
   else{
      memset(MSG,'\0',sizeof(MSG));
      strncpy(MSG, "은화가 부족합니다 다른 행동을 하세요!", sizeof(MSG) - 1);
      write(player1[index1].user_sockfd,MSG,sizeof(MSG));
   }
   while(1){
         memset(MSG,'\0',sizeof(MSG));
         read(player1[index1].user_sockfd,MSG,sizeof(MSG)); // 턴을 진행 중인 유저
         memset(MSG,'\0',sizeof(MSG));
         strncpy(MSG, "암살당했습니다! 버릴 카드를 선택하세요", sizeof(MSG) - 1);
         write(player2[index2].user_sockfd,MSG,sizeof(MSG)); // 암살 당한 유저에게
         while(1){
            memset(MSG,'\0',sizeof(MSG));
            read(player2[index2].user_sockfd,MSG,sizeof(MSG)); // 암살 당한 유저에게
            if(strcmp(MSG, "duke") == 0)
               player2[index2].user_character.duke--;
            else if(strcmp(MSG, "assasin") == 0)
               player2[index2].user_character.assassin--;
            else if(strcmp(MSG, "ambassador") == 0)
               player2[index2].user_character.ambassador--;
            else if(strcmp(MSG, "captain") == 0)
               player2[index2].user_character.captain--;
            else if(strcmp(MSG, "contessa") == 0)
               player2[index2].user_character.contessa--;
            else{
               
               memset(MSG,'\0',sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 골라주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd,MSG,sizeof(MSG)); // 암살 당한 유저에게
               break;
               
   }
         }
   }}

void ambassador(struct user_node *player1, int index1,struct administrator *admin){
      int msgsize = 0;
      char MSG[256];
      memset(MSG,'\0',sizeof(MSG));
      strncpy(MSG, "원하는 카드를 고르세요!", sizeof(MSG) - 1);
      write(player1[index1].user_sockfd,MSG,sizeof(MSG)); // 현재 턴인 유저
      while(1){
         memset(MSG,'\0',sizeof(MSG));
         read(player1[index1].user_sockfd,MSG,sizeof(MSG)); 
         if(strcmp(MSG, "duke") == 0)
            {player1[index1].user_character.duke++;
            break;}
         else if(strcmp(MSG, "assassin") == 0){
            player1[index1].user_character.assassin++;
            break;}
         else if(strcmp(MSG, "ambassador") == 0){
            player1[index1].user_character.ambassador++;
            break;}
         else if(strcmp(MSG, "captain") == 0){
            player1[index1].user_character.captain++;
            break;}
         else if(strcmp(MSG, "contessa") == 0){
            player1[index1].user_character.contessa++;
            break;}
         else{
            memset(MSG,'\0',sizeof(MSG));
            strncpy(MSG, "카드를 다시 골라주세요!", sizeof(MSG) - 1);
            write(player1[index1].user_sockfd,MSG,sizeof(MSG));
         }
      }
      memset(MSG,'\0',sizeof(MSG));
      strncpy(MSG, "카드 한장은 버려주세요!", sizeof(MSG) - 1);
      write(player1[index1].user_sockfd,MSG,sizeof(MSG)); // 현재 턴인 유저
      
      while(1){
         memset(MSG,'\0',sizeof(MSG));
         msgsize = read(player1[index1].user_sockfd,MSG,sizeof(MSG));
         if(strcmp(MSG, "duke") == 0)
            {player1[index1].user_character.duke--;
            break;}
         else if(strcmp(MSG, "assassin") == 0){
            player1[index1].user_character.assassin--;
            break;}
         else if(strcmp(MSG, "ambassador") == 0){
            player1[index1].user_character.ambassador--;
            break;}
         else if(strcmp(MSG, "captain") == 0){
            player1[index1].user_character.captain--;
            break;}
         else if(strcmp(MSG, "contessa") == 0){
            player1[index1].user_character.contessa--;
            break;}
         else{
            memset(MSG,'\0',sizeof(MSG));
            strncpy(MSG, "버릴 카드를 다시 선택해 주세요!", sizeof(MSG) - 1);
            write(player1[index1].user_sockfd,MSG,sizeof(MSG));
         }
      }
   
      }
   

// 쌩도전
void challenge(struct user_node *player1, struct user_node *player2, int index1, int index2,int character){
   char MSG[256];
   memset(MSG,'\0',sizeof(MSG));
   strncpy(MSG, "당신의 행동 혹은 저지가 도전받았습니다!", sizeof(MSG) - 1);
   write(player1[index1].user_sockfd,MSG,sizeof(MSG)); // 도전 받은 유저
   while(1){
      switch (character) {
       case 1: {
           if (player1[index1].user_character.duke == 1 || player1[index1].user_character.duke == 2) {
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "진실이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player2[index2].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player2[index2].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player2[index2].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player2[index2].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player2[index2].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player2[index2].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player2[index2].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
           }
            else if(player1[index1].user_character.duke == 0){
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "거짓이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player1[index1].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player1[index1].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player1[index1].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player1[index1].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player1[index1].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player1[index1].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player1[index1].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
            }
       }
         case 2: {
           if (player1[index1].user_character.assassin == 1 || player1[index1].user_character.assassin == 2) {
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "진실이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player2[index2].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player2[index2].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player2[index2].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player2[index2].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player2[index2].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player2[index2].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player2[index2].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
           }
            else if(player1[index1].user_character.assassin == 0){
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "거짓이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player1[index1].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player1[index1].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player1[index1].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player1[index1].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player1[index1].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player1[index1].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player1[index1].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
            }
         }
         case 3: {
           if (player1[index1].user_character.ambassador == 1 || player1[index1].user_character.ambassador == 2) {
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "진실이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player2[index2].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player2[index2].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player2[index2].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player2[index2].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player2[index2].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player2[index2].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player2[index2].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
           }
            else if(player1[index1].user_character.ambassador == 0){
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "거짓이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player1[index1].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player1[index1].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player1[index1].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player1[index1].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player1[index1].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player1[index1].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player1[index1].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
            }
         }
         case 4: {
           if (player1[index1].user_character.captain == 1 || player1[index1].user_character.captain == 2) {
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "진실이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player2[index2].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player2[index2].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player2[index2].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player2[index2].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player2[index2].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player2[index2].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player2[index2].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
           }
            else if(player1[index1].user_character.captain == 0){
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "거짓이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player1[index1].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player1[index1].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player1[index1].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player1[index1].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player1[index1].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player1[index1].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player1[index1].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
            }
         }
         case 5: {
           if (player1[index1].user_character.contessa == 1 || player1[index1].user_character.contessa == 2) {
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "진실이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player2[index2].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player2[index2].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player2[index2].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player2[index2].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player2[index2].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player2[index2].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player2[index2].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
           }
            else if(player1[index1].user_character.contessa == 0){
               memset(MSG, '\0', sizeof(MSG));
                  strncpy(MSG, "거짓이었습니다", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
               write(player2[index2].user_sockfd, MSG, sizeof(MSG));
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, sizeof(MSG));
            memset(MSG,'\0',sizeof(MSG));
                  read(player1[index1].user_sockfd, MSG, sizeof(MSG));
               if (strcmp(MSG, "duke") == 0) {
                    player1[index1].user_character.duke--;
                     break;
               }
               else if (strcmp(MSG, "assassin") == 0) {
                     player1[index1].user_character.assassin--;
                   break;
               }    
               else if (strcmp(MSG, "ambassador") == 0) {
                     player1[index1].user_character.ambassador--;
                   break;
               }    
               else if (strcmp(MSG, "captain") == 0) {
                   player1[index1].user_character.captain--;
                   break;
               }    
               else if (strcmp(MSG, "contessa") == 0) {
                   player1[index1].user_character.contessa--;
                   break;
               }    
               else {
                   memset(MSG, '\0', sizeof(MSG));
                   strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
                   write(player1[index1].user_sockfd, MSG, sizeof(MSG));
                   break;
               }
            }
         }
        break;
    }
      
      }
   }
}
void Help(struct user_node *player1, int index1){
   char MSG[512];
   memset(MSG,'\0',sizeof(MSG));
   strncpy(MSG,"도움말 : 사용 가능한 행동\n"
            "(income):coin+1 from bank\n"
            "(foreign_aid):coin+2 from bank\n"
            "(duke):+3 from bank\n"
            "(captain, other player name):+2 coin from other player\n"
            "(assassin, other player name):coin-7, kill ohter player\n"
            "(ambassador):change card\n"
            "(contessa):defense assassin\n"
            "(challge):if you doubt ohter player, you can challenge\n"
            "(block):if you want blocking ohter player's behavior\n", sizeof(MSG));
   write(player1[index1].user_sockfd, MSG, sizeof(MSG));