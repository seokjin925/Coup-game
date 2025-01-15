#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/select.h>
#include<stdlib.h>
#include<time.h>
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
   
   
   int server_socket;
   socklen_t clnt_adr_sz, act_adr_sz;
   struct sockaddr_in client_addr[4], temp_addr;
   void challenge(struct user_node *player1, struct user_node *player2, int index1, int index2,int character);
   void ambassador(struct user_node *player1, int index1);
   void assassin(struct user_node *player1, struct user_node *player2, int index1);
   void captain(struct user_node *player1, struct user_node *player2, int index1);
   void duke(struct user_node *player1, int index1);
   void coup(struct user_node *player1, struct user_node *player2, int index1); 
   void foreign_aid(struct user_node *player, int index);
   void income(struct user_node *player, int index);
   int Socket(int family, int type, int protocol);
   void Bind(int sockfd, const struct sockaddr *socketaddr, socklen_t addrlen);
   void Listen(int sockfd, int backlog);
   int Accept(int sockfd, struct sockaddr* Cliaddr, socklen_t *addelen);
   int duke1 = 0, assassin1 = 0, ambassador1 = 0, captain1 = 0, contessa1 = 0,foreign_aid1=0;
   int main(int argc, char *argv[])
   {
      
      struct administrator admin;
      struct user_node player[4];
      int client_socket;
      struct sockaddr_in server_addr;
      
      int addrsize, msgsize;
      memset(&admin, 0, sizeof(struct administrator));
      for (int i = 0; i < 4; i++)
        memset(&player[i], 0, sizeof(struct user_node));
      
      char ch_buff[1024] = {0};
      char BUFF[1024] = {0};
      char act[1024] = {0};
      memset(act, '\0', sizeof(act));   
      fd_set readfd;

      int maxfd=0;
      int room_index=0;
      int user_index=0;
      int tempsockfd=0;
      int temp_user_count=0;
      int user_count=0;
      int str_len = 0;

      for (int i =0;i<4;i++)
      player[i].user_bankaccount = 2;
             // 은화, 직업 분배

    for(int i=0;i<4;i++){
        player[i].user_bankaccount = 2;
    }
    srand(time(NULL));
    int count = 0;
    int ran;     
    for(int i=0;i<4;i++){
        for(count = 0; count<2; count++){
            ran = rand()%5;
            if(ran == 0)
                player[i].user_character.duke++;
            else if(ran == 1)
                player[i].user_character.assassin++;
            else if(ran == 2)
                player[i].user_character.ambassador++;
            else if(ran == 3)
                player[i].user_character.captain++;
            else if(ran == 4)
                player[i].user_character.contessa++;
        }
    }   
    for(int i=0;i<4;i++){
        printf("%d %d %d %d %d\n",player[i].user_character.duke,player[i].user_character.assassin,
        player[i].user_character.ambassador,player[i].user_character.captain,player[i].user_character.contessa);
    }

         

         /*(1) 서버 기본설정*/
      server_socket = Socket(PF_INET,SOCK_DGRAM,0);

      server_addr.sin_family=AF_INET;
      server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
      server_addr.sin_port=htons(atoi(argv[1]));

      memset(&(server_addr.sin_zero), 0, 8);

      Bind(server_socket,(struct sockaddr*)&server_addr, sizeof(struct sockaddr));
      /*(2) 서버 접속대기*/
      
      
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

         if(FD_ISSET(server_socket,&readfd)){                     // 1번 접속자 처리
            addrsize = sizeof(struct sockaddr_in);
            memset(BUFF, '\0', sizeof(BUFF));      
            clnt_adr_sz=sizeof(client_addr[0]);
            str_len=recvfrom(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[0], &clnt_adr_sz);
            if(clnt_adr_sz <=0){
               printf("Enter user Error\n");
               continue;
            }
            printf("Recive Message: %s\n", BUFF);// 접속자 이름 출력
            player[0].user_sockfd = client_socket;
            printf("%d", player[0].user_sockfd);
            player[0].turn1 = 0;
            user_count++;
            
            memset(BUFF, '\0', sizeof(BUFF));      
            strncpy(BUFF, "Connect OK", sizeof(BUFF) - 1);
            sendto(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[0], clnt_adr_sz);
         }
         if(FD_ISSET(server_socket,&readfd)){                     // 2번 접속자 처리
            addrsize = sizeof(struct sockaddr_in);
            memset(BUFF, '\0', sizeof(BUFF));      
            clnt_adr_sz=sizeof(client_addr[1]);
            str_len=recvfrom(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[1], &clnt_adr_sz);
            if(clnt_adr_sz <=0){
               printf("Enter user Error\n");
               continue;
            }
            printf("Recive Message: %s\n", BUFF);// 접속자 이름 출력
            player[1].user_sockfd = client_socket;
            printf("%d", player[1].user_sockfd);
            player[1].turn1 = user_count;
            user_count++;
   
            memset(BUFF, '\0', sizeof(BUFF));      
            strncpy(BUFF, "Connect OK", sizeof(BUFF) - 1);
            sendto(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[1], clnt_adr_sz);
         }

         if(FD_ISSET(server_socket,&readfd)){                     // 3번 접속자 처리
            addrsize = sizeof(struct sockaddr_in);
            memset(BUFF, '\0', sizeof(BUFF));      
            clnt_adr_sz=sizeof(client_addr[2]);
            str_len=recvfrom(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[2], &clnt_adr_sz);
            if(clnt_adr_sz <=0){
               printf("Enter user Error\n");
               continue;
            }
            printf("Recive Message: %s\n", BUFF);// 접속자 이름 출력
            player[user_count].user_sockfd = client_socket;
            printf("%d", player[2].user_sockfd);
            player[user_count].turn1 = user_count;
            user_count++;
   
            memset(BUFF, '\0', sizeof(BUFF));      
            strncpy(BUFF, "Connect OK", sizeof(BUFF) - 1);
            sendto(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[2], clnt_adr_sz);
         }

         if(FD_ISSET(server_socket,&readfd)){                     // 4번 접속자 처리
            addrsize = sizeof(struct sockaddr_in);
            memset(BUFF, '\0', sizeof(BUFF));      
            clnt_adr_sz=sizeof(client_addr[3]);
            str_len=recvfrom(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[3], &clnt_adr_sz);
            if(clnt_adr_sz <=0){
               printf("Enter user Error\n");
               continue;
            }
            printf("Recive Message: %s\n", BUFF);// 접속자 이름 출력
            player[user_count].user_sockfd = client_socket;
            printf("%d", player[3].user_sockfd);
            player[user_count].turn1 = user_count;
            
   
            memset(BUFF, '\0', sizeof(BUFF));      
            strncpy(BUFF, "Connect OK", sizeof(BUFF) - 1);
            sendto(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&client_addr[3], clnt_adr_sz);
         }
         

         for(user_index=0;user_index<4;user_index++){
         tempsockfd = player[user_index].user_sockfd;
            FD_SET(tempsockfd,&readfd);
            if(tempsockfd>maxfd)
               maxfd=tempsockfd;
         }

         

      


         
            memset(BUFF, '\0', sizeof(BUFF));      
            str_len=recvfrom(server_socket, BUFF, 100, 0, 
                  (struct sockaddr*)&temp_addr, &clnt_adr_sz);
            if (strstr(BUFF, "start") != NULL)
         {
         //직업 알려주기
         for (int i = 0; i < 4; i++) {
              // ...

              // 무작위로 두 개의 직업을 1로 설정
        // ...

        // 클라이언트에게 현재 플레이어의 직업 정보 전송
           memset(BUFF, '\0', sizeof(BUFF));   
           sprintf(BUFF, "당신의 직업: Duke(%d), Assassin(%d), Ambassador(%d), Captain(%d), Contessa(%d)\n",
                player[i].user_character.duke, player[i].user_character.assassin,
                player[i].user_character.ambassador, player[i].user_character.captain,
                player[i].user_character.contessa);

           sendto(server_socket, BUFF,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
         }

              
          while(1) {          
                                            //게임루프
            for(int user_index =0;user_index<4;user_index++)
            {

            memset(BUFF, '\0', sizeof(BUFF));
            strncpy(BUFF, "It's your turn, do anything you want to act", sizeof(BUFF) - 1);
        
                 
                sendto(server_socket, BUFF, 100, 0,                         
               (struct sockaddr*)&client_addr[user_index], clnt_adr_sz);
                 

            memset(BUFF, '\0', sizeof(BUFF));
            
            //if (FD_ISSET(player[user_index].user_sockfd, &readfd)){
              str_len=recvfrom(server_socket, BUFF, 100, 0, 
                        (struct sockaddr*)&temp_addr, &clnt_adr_sz);
            printf("%s\n", BUFF);
         duke1 =0, assassin1 = 0, ambassador1 =0, captain1 = 0, contessa1=0, foreign_aid1=0;
            
            if (strstr(BUFF, "duke") != NULL)
                duke1 = 1;
           // 지워야 됌
            
             //sendto(server_socket, act, 100, 0, (struct sockaddr*)&client_addr[user_index], act_adr_sz);
            //sendto(server_socket, duke, 100, 0, (struct sockaddr*)&client_addr[user_index], clnt_adr_sz);   //지워야댐                 
            else if (strstr(BUFF, "assassin") != NULL)
                assassin1=2;
            else if (strstr(BUFF, "ambassador") != NULL) 
                ambassador1=3;
            else if (strstr(BUFF, "captain") != NULL) 
                captain1=4;
            else if (strstr(BUFF, "contessa") != NULL) 
                contessa1=5;
            else if(strstr(BUFF, "foreign_aid") != NULL)
                foreign_aid1=6;
    
                  
            //}
                        // 행동을 읽고 그대로 다시 뿌림
              for(int i=0;i<4;i++){
                sendto(server_socket, BUFF, 100, 0, 
               (struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }   
            strcpy(act, BUFF);
            printf("act buff : %s\n", act);
            
         memset(BUFF,'\0',sizeof(BUFF));
         strncpy(BUFF, "이 행동에 대해 저지 혹은 도전하실 플레이어 있으신가요?", sizeof(BUFF) - 1);      
            
            for(int i=0;i<4;i++){
            sendto(server_socket, BUFF, 200, 0,                         
            (struct sockaddr*)&client_addr[i], clnt_adr_sz);    
         }
         memset(BUFF,'\0',sizeof(BUFF));

         
         str_len=recvfrom(server_socket, BUFF, 100, 0, 
               (struct sockaddr*)&temp_addr, &clnt_adr_sz); //}      // 임시저장
            
         for(int i=0;i<4;i++){
            sendto(server_socket, BUFF, 100, 0, 
            (struct sockaddr*)&client_addr[i], clnt_adr_sz); }            
            // sendto(server_socket, BUFF, 100, 0, 
            // (struct sockaddr*)&client_addr[0], clnt_adr_sz); 
            
             
            
               
               printf("%s\n",BUFF);   
            if (strstr(BUFF, "challenge") != NULL){
            sprintf(ch_buff, "누군가 방금의 행동에 대해 도전했습니다!");      
            printf("누군가 방금 행동에 대해 도전했습니다!\n");
               for (int i = 0; i < 4; i++) {
                  sendto(server_socket, ch_buff, 100, 0, 
            (struct sockaddr*)&client_addr[i], clnt_adr_sz);
               }      

            if (strstr(BUFF, "1") != NULL){                                       
                if(duke1 == 1)
                    challenge(player,player,user_index,0,1);
                else if(assassin1 == 2)
                    challenge(player,player,user_index,0,2);
                else if(ambassador1 == 3)
                    challenge(player,player,user_index,0,3);
                else if(captain1 == 4)
                    challenge(player,player,user_index,0,4);
                else if(contessa1 == 5)
                    challenge(player,player,user_index,0,5);
            }
            if (strstr(BUFF, "2") != NULL){                                    
                if(duke1 == 1)
                    challenge(player,player,user_index,1,1);
                else if(assassin1 == 2)
                    challenge(player,player,user_index,1,2);
                else if(ambassador1 == 3)
                    challenge(player,player,user_index,1,3);
                else if(captain1 == 4)
                    challenge(player,player,user_index,1,4);
                else if(contessa1 == 5)
                    challenge(player,player,user_index,1,5);
            }
            if (strstr(BUFF, "3") != NULL){                                      
                if(duke1 == 1)
                    challenge(player,player,user_index,2,1);
                else if(assassin1 == 2)
                    challenge(player,player,user_index,2,2);
                else if(ambassador1 == 3)
                    challenge(player,player,user_index,2,3);
                else if(captain1 == 4)
                    challenge(player,player,user_index,2,4);
                else if(contessa1 == 5)
                    challenge(player,player,user_index,2,5);
            }
            if (strstr(BUFF, "4") != NULL){                                      
                if(duke1 == 1)
                    challenge(player,player,user_index,3,1);
                else if(assassin1 == 2)
                    challenge(player,player,user_index,3,2);
                else if(ambassador1 == 3)
                    challenge(player,player,user_index,3,3);
                else if(captain1 == 4)
                    challenge(player,player,user_index,3,4);
                else if(contessa1 == 5)
                    challenge(player,player,user_index,3,5);
            }
               
       
               
            }
         else if (strstr(BUFF, "block") != NULL){
                memset(ch_buff,'\0',sizeof(ch_buff));
                sprintf(ch_buff, "누군가 방금의 행동을 저지했습니다!");
                printf("누군가 방금 행동을 저지했습니다!\n");
                for(int j=0;j<4;j++){
                sendto(server_socket, ch_buff, 200, 0, 
                    (struct sockaddr*)&client_addr[j], clnt_adr_sz);               
                }
                memset(ch_buff,'\0',sizeof(ch_buff));
                sprintf(ch_buff, "저지에 대해 도전하시겠습니까?");
                sendto(server_socket, ch_buff, 200, 0,
                    (struct sockaddr*)&client_addr[user_index], clnt_adr_sz);
               memset(ch_buff,'\0',sizeof(ch_buff));
                str_len=recvfrom(server_socket, ch_buff, 100, 0, 
                        (struct sockaddr*)&temp_addr, &clnt_adr_sz);
                for(int j=0;j<4;j++){
                sendto(server_socket, ch_buff, 200, 0, 
                    (struct sockaddr*)&client_addr[j], clnt_adr_sz);              
                }
                if(strstr(ch_buff,"challenge") != NULL){
                    if(strstr(BUFF, "1") != NULL){
                        if(assassin1 == 2)
                            challenge(player,player,0,user_index,5);
                        else if(captain1 == 4){
                            if(strstr(BUFF, "captain") != NULL)
                                challenge(player,player,0,user_index,4);
                            else if(strstr(BUFF, "ambassador") != NULL)
                                challenge(player,player,0,user_index,3);
                        }
                        else if(foreign_aid1 == 6)
                            challenge(player,player,0,user_index,1);    // , , Player1 저지받은, 저지하고있는 유저 턴유저
                    }
                    if(strstr(BUFF, "2") != NULL){
                        if(assassin1 == 2)
                            challenge(player,player,1,user_index,5);
                        else if(captain1 == 4){
                            if(strstr(BUFF, "captain") != NULL)
                                challenge(player,player,1,user_index,4);
                            else if(strstr(BUFF, "ambassador") != NULL)
                                challenge(player,player,1,user_index,3);
                        }
                        else if(foreign_aid1 == 6)
                            challenge(player,player,1,user_index,1);
                    }
                    if(strstr(BUFF, "3") != NULL){
                        if(assassin1 == 2)
                            challenge(player,player,2,user_index,5);
                        else if(captain1 == 4){
                            if(strstr(BUFF, "captain") != NULL)
                                challenge(player,player,2,user_index,4);
                            else if(strstr(BUFF, "ambassador") != NULL)
                                challenge(player,player,2,user_index,3);
                        }
                        else if(foreign_aid1 == 6)
                            challenge(player,player,2,user_index,1);
                    }
                    if(strstr(BUFF, "4") != NULL){
                        if(assassin1 == 2)
                            challenge(player,player,3,user_index,5);
                        else if(captain1 == 4){
                            if(strstr(BUFF, "captain") != NULL)
                                challenge(player,player,3,user_index,4);
                            else if(strstr(BUFF, "ambassador") != NULL)
                                challenge(player,player,3,user_index,3);
                        }
                        else if(foreign_aid1 == 6)
                            challenge(player,player,3,user_index,1);
                    }
                }
                else
                    continue;
            }
         
         
    
         

            
         

       else{
            
         if (strstr(act, "income") != NULL)
         income(player,user_index);
         else if (strstr(act, "foreign") != NULL)
         foreign_aid(player,user_index);
         else if (strstr(act, "coup") != NULL)
         coup(player,player,user_index);                           
         else if (strstr(act, "duke") != NULL)
         duke(player,user_index);
         else if (strstr(act, "captain") != NULL)
         captain(player,player,user_index);
         else if (strstr(act, "assassin") != NULL)
         assassin(player,player,user_index);
         else if (strstr(act, "ambass") != NULL)
         ambassador(player,user_index);
         }

         for (int i = 0; i < 4; i++) {
           memset(BUFF, '\0', sizeof(BUFF));   
           sprintf(BUFF, "당신의 직업: Duke(%d), Assassin(%d), Ambassador(%d), Captain(%d), Contessa(%d)\n",
                player[i].user_character.duke, player[i].user_character.assassin,
                player[i].user_character.ambassador, player[i].user_character.captain,
                player[i].user_character.contessa);
           sendto(server_socket, BUFF,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
         }
         for (int i = 0; i < 4; i++){
            if(player[i].user_character.duke == 0 && player[i].user_character.assassin == 0 && player[i].user_character.ambassador == 0
                  && player[i].user_character.captain && player[i].user_character.contessa){
                     memset(BUFF, '\0', sizeof(BUFF));
                     sprintf(BUFF, "모든 직업카드를 잃었습니다! 당신의 차례가 되면 DIE를 입력해주세요");
                     sendto(server_socket, BUFF, 200, 0, 
                        (struct sockaddr*)&client_addr[i], clnt_adr_sz);              
                    
            }
         }
         
         
         

         if(user_count >=5)
         {
            for(int i =0;i<4;i++)
            {
               close(player[i].user_sockfd);
            }
         }
            }


         }          // while 
      
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
   char MSG[256];
    memset(MSG, '\0', sizeof(MSG));

    // 유효한 인덱스인지 확인
    if (index >= 0 && index < 4) {
        // 해당 인덱스의 bankaccount를 1 증가시킴
        player[index].user_bankaccount += 1;
    } else {
        // 인덱스가 유효하지 않을 경우 오류 처리 또는 경고 등을 수행
        printf("유효하지 않은 인덱스입니다.\n");
    }
   sprintf(MSG,"현재 보유 은화는 %d개 입니다!", player[index].user_bankaccount);
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index], clnt_adr_sz); 
}


void foreign_aid(struct user_node *player, int index){
   char MSG[256];
    memset(MSG, '\0', sizeof(MSG));
    if (index >= 0 && index < 4) {
        // 해당 인덱스의 bankaccount를 1 증가시킴
        player[index].user_bankaccount += 2;


    } else {
        // 인덱스가 유효하지 않을 경우 오류 처리 또는 경고 등을 수행
        printf("유효하지 않은 인덱스입니다.\n");
}
sprintf(MSG,"현재 보유 은화는 %d개 입니다!", player[index].user_bankaccount);
sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index], clnt_adr_sz); 
}


void coup(struct user_node *player1, struct user_node *player2, int index1) {
    player1[index1].user_bankaccount -= 7;
    char MSG[256];
   int index2;
    memset(MSG, '\0', sizeof(MSG));

   sprintf(MSG,"쿠데타 대상 플레이어를 골라주세요! (player*)");
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 


   recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);
   char *player = strtok(MSG, "[");
   if   (strstr(player, "player1") != NULL)
      index2=0;
   else if   (strstr(player, "player2") != NULL)
      index2=1;
   else if   (strstr(player, "player3") != NULL)
      index2=2;
   else if   (strstr(player, "player4") != NULL)
      index2=3;
      

   memset(MSG, '\0', sizeof(MSG));

   sprintf(MSG, "현재 보유 카드 duke : %d assassin : %d ambassador : %d capatain : %d contessa : %d\n"
   
   , player2[index2].user_character.duke, player2[index2].user_character.assassin,
            player2[index2].user_character.ambassador, player2[index2].user_character.captain,
         player2[index2].user_character.contessa);
   sendto(server_socket, MSG, 200, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
   memset(MSG, '\0', sizeof(MSG));
   sprintf(MSG, "상대플레이어의 쿠데타로 인해 카드를 한장 잃게됩니다. 버릴 카드를 선택해주세요!\n");   
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz);          // indx1 에서 2로 바꿈 마지막에

   
   
   
   recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);

    while (1) {
        if (strstr(MSG, "duke") != NULL) {
            player2[index2].user_character.duke--;
            break;
        } else if (strstr(MSG, "assassin") != 0) {
            player2[index2].user_character.assassin--;
            break;
        } else if (strstr(MSG, "ambassador") != 0) {
            player2[index2].user_character.ambassador--;
            break;
        } else if (strstr(MSG, "captain") != 0) {
            player2[index2].user_character.captain--;
            break;
        } else if (strstr(MSG, "contessa") != 0) {
            player2[index2].user_character.contessa--;
            break;
        } else {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "duke, assassin, ambassador, captain, contessa 중 다시 고르세요!", sizeof(MSG) - 1);
           
         sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
        }
    }
   memset(MSG, '\0', sizeof(MSG));
   sprintf(MSG, "현재 보유 카드 duke : %d assassin : %d ambassador : %d capatain : %d contessa : %d"
   , player2[index2].user_character.duke, player2[index2].user_character.assassin,
            player2[index2].user_character.ambassador, player2[index2].user_character.captain,
         player2[index2].user_character.contessa);
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
}


void duke(struct user_node *player1, int index1){
   char MSG[256];
    memset(MSG, '\0', sizeof(MSG));
    player1[index1].user_bankaccount += 3;

   sprintf(MSG,"현재 보유 은화는 %d개 입니다!", player1[index1].user_bankaccount);

   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
   
}

void captain(struct user_node *player1, struct user_node *player2, int index1){
   char MSG[256];
   int index2;
    memset(MSG, '\0', sizeof(MSG));

   sprintf(MSG,"은화 강탈 대상 플레이어를 골라주세요! (player*)");
   
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
   recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);
   

   char *player = strtok(MSG, "[");
   if   (strstr(player, "player1") != NULL)
      index2=0;
   else if   (strstr(player, "player2") != NULL)
      index2=1;
   else if   (strstr(player, "player3") != NULL)
      index2=2;
   else if   (strstr(player, "player4") != NULL)
      index2=3;      
   memset(MSG, '\0', sizeof(MSG));
   while(1)
   {
         if(player2[index2].user_bankaccount >= 2)
            {player2[index2].user_bankaccount -= 2;
            player1[index1].user_bankaccount += 2;
            break;
            }
            
         else{
      
            strncpy(MSG, "상대 플레이어가 코인을 2원 미만으로 소지하고 있습니다 다른 플레이어를 골라주세요", sizeof(MSG) - 1);
            sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); // 턴을 진행 중인 유저   
            }
   }
   memset(MSG, '\0', sizeof(MSG));
   sprintf(MSG,"강탈 성공으로 현재 보유 은화는 %d개 입니다!", player1[index1].user_bankaccount);

   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz);
   memset(MSG, '\0', sizeof(MSG));
   sprintf(MSG,"상대 플레이어의 강탈로 인해 현재 보유 은화는 %d개 입니다!", player1[index2].user_bankaccount);

   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz);
   
}
//              암살시도 플레이어            암살대상플레이어        암살시도플레이어인데스, 암살대상플레이어 인덱스
void assassin(struct user_node *player1, struct user_node *player2, int index1)
{
   char MSG[256];
   int index2;
    memset(MSG, '\0', sizeof(MSG));

   sprintf(MSG,"암살 대상 플레이어를 한번더 말해주세요! (player*)");
   
   sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
   recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);
   

   char *player = strtok(MSG, "[");
   if   (strstr(player, "player1") != NULL)
      index2=0;
   else if   (strstr(player, "player2") != NULL)
      index2=1;
   else if   (strstr(player, "player3") != NULL)
      index2=2;
   else if   (strstr(player, "player4") != NULL)
      index2=3;      
   memset(MSG, '\0', sizeof(MSG));
   if(player1[index1].user_bankaccount >= 3)
      player1[index1].user_bankaccount -= 3;
   else{
      memset(MSG,'\0',sizeof(MSG));
      strncpy(MSG, "은화가 부족합니다 다른 행동을 하세요!", sizeof(MSG) - 1);
      sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
   }
  
         
            memset(MSG,'\0',sizeof(MSG));

         sprintf(MSG, "현재 보유 카드 duke : %d assassin : %d ambassador : %d capatain : %d contessa : %d\n"
   
   , player2[index2].user_character.duke, player2[index2].user_character.assassin,
            player2[index2].user_character.ambassador, player2[index2].user_character.captain,
         player2[index2].user_character.contessa);
         sendto(server_socket, MSG, 200, 0, 
                  (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
         memset(MSG, '\0', sizeof(MSG));
         sprintf(MSG, "상대플레이어의 암살로 인해 카드를 한장 잃게됩니다. 버릴 카드를 선택해주세요!\n");   
         sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
         
         memset(MSG, '\0', sizeof(MSG));
         recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);

            // 암살당한
            if(strstr(MSG, "duke") != NULL)
               player2[index2].user_character.duke--;
               
            else if(strstr(MSG, "assassin") != NULL)
               player2[index2].user_character.assassin--;
              
            
            else if(strstr(MSG, "ambassador") != NULL)
               player2[index2].user_character.ambassador--;
             
            else if(strstr(MSG, "captain") != NULL)
               player2[index2].user_character.captain--;
               
            else if(strstr(MSG, "contessa") != NULL)
               player2[index2].user_character.contessa--;
           
            
           
         
       memset(MSG,'\0',sizeof(MSG));

         sprintf(MSG, "현재 보유 카드 duke : %d assassin : %d ambassador : %d capatain : %d contessa : %d\n"
   
   , player2[index2].user_character.duke, player2[index2].user_character.assassin,
            player2[index2].user_character.ambassador, player2[index2].user_character.captain,
         player2[index2].user_character.contessa);
         sendto(server_socket, MSG, 200, 0, 
                  (struct sockaddr*)&client_addr[index2], clnt_adr_sz); 
       
   
   }


void ambassador(struct user_node *player1, int index1){
      int msgsize = 0;
      char MSG[256];
      memset(MSG,'\0',sizeof(MSG));
      sprintf(MSG, "원하는 카드를 고르세요! (duke, assassin, ambassador, captain, contessa\n");   
         sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
      while(1){
         memset(MSG,'\0',sizeof(MSG));
         recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);
         if(strstr(MSG, "duke") != NULL)
            {player1[index1].user_character.duke++;
            break;}
         else if(strstr(MSG, "assassin") != NULL){
            player1[index1].user_character.assassin++;
            break;}
         else if(strstr(MSG, "ambassador") != NULL){
            player1[index1].user_character.ambassador++;
            break;}
         else if(strstr(MSG, "captain") != NULL){
            player1[index1].user_character.captain++;
            break;}
         else if(strstr(MSG, "contessa") != NULL){
            player1[index1].user_character.contessa++;
            break;}
         else{
            memset(MSG,'\0',sizeof(MSG));
            sprintf(MSG, "카드 선택 오류 카드 다시 골라주세요! (duke, assassin, ambassador, captain, contessa\n");   
            sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
         }
      }
      memset(MSG,'\0',sizeof(MSG));
      sprintf(MSG, "현재 보유 카드 duke : %d assassin : %d ambassador : %d capatain : %d contessa : %d\n"
   
   , player1[index1].user_character.duke, player1[index1].user_character.assassin,
            player1[index1].user_character.ambassador, player1[index1].user_character.captain,
         player1[index1].user_character.contessa);
         sendto(server_socket, MSG, 200, 0, 
                  (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
      memset(MSG,'\0',sizeof(MSG));
      sprintf(MSG, "카드 한장은 버려주세요! (duke, assassin, ambassador, captain, contessa\n");   
         sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
      
      while(1){
         memset(MSG,'\0',sizeof(MSG));
         recvfrom(server_socket, MSG, 100, 0, (struct sockaddr*)&temp_addr, &clnt_adr_sz);
         if(strstr(MSG, "duke") != NULL)
            {player1[index1].user_character.duke--;
            break;}
         else if(strstr(MSG, "assassin") != NULL){
            player1[index1].user_character.assassin--;
            break;}
         else if(strstr(MSG, "ambassador") != NULL){
            player1[index1].user_character.ambassador--;
            break;}
         else if(strstr(MSG, "captain") != 0){
            player1[index1].user_character.captain--;
            break;}
         else if(strstr(MSG, "contessa") != 0){
            player1[index1].user_character.contessa--;
            break;}
         else{
            memset(MSG,'\0',sizeof(MSG));
            sprintf(MSG, "카드 선택 오류 카드 다시 골라주세요! (duke, assassin, ambassador, captain, contessa\n");   
            sendto(server_socket, MSG, 100, 0, 
            (struct sockaddr*)&client_addr[index1], clnt_adr_sz); 
         }
      }
      }
   
  
   

// 쌩도전
void challenge(struct user_node *player1, struct user_node *player2, int index1, int index2,int character){
   char MSG[256];
   
   memset(MSG,'\0',sizeof(MSG));
   strncpy(MSG, "당신의 행동 혹은 저지가 도전받았습니다!", sizeof(MSG) - 1);
   sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz); // 도전 받은 유저
   
      switch (character) {
      case 1: {
         if (player1[index1].user_character.duke >= 1 ) {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "공작 행동은 진실이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index2].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index2].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index2].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index2].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index2].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index2].user_sockfd, MSG, strlen(MSG));
               
            }
         }
         else if(player1[index1].user_character.duke == 0){
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "공작 행동은 거짓이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") == 0) {
               player2[index1].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
               
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index1].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index1].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index1].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index1].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
               
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, strlen(MSG));
               
            }
         }
         break;
      }
      case 2: {
         if (player1[index1].user_character.assassin >= 1 ) {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "암살자 행동은 진실이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index2].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index2].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
              
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index2].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index2].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index2].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index2].user_sockfd, MSG, strlen(MSG));
           
            }
         }
         else if(player1[index1].user_character.assassin == 0){
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "암살자 행동은 거짓이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index1].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index1].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index1].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index1].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index1].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, strlen(MSG));
               
            }
         }
         break;
      }
      
      
      case 3: {
         if (player1[index1].user_character.ambassador >= 1 ) {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "대사 행동은 진실이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index2].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index2].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index2].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index2].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index2].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
              
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index2].user_sockfd, MSG, strlen(MSG));
           
            }
         }
         else if(player1[index1].user_character.ambassador == 0){
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "대사 행동은 거짓이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index1].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index1].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index1].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index1].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index1].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
           
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, strlen(MSG));
              
            }
         }
         break;
      }
      case 4: {
         if (player1[index1].user_character.captain >= 1 ) {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "사령관 행동은 진실이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index2].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
              
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index2].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
               
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index2].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
           
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index2].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
              
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index2].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
             
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index2].user_sockfd, MSG, strlen(MSG));
            
            }
         }
         else if(player1[index1].user_character.captain == 0){
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "사령관 행동은 거짓이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index1].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
              
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index1].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index1].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
           
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index1].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
              
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index1].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
           
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, strlen(MSG));
              
            }
         }
         break;
      }

      case 5: {
         if (player1[index1].user_character.contessa >= 1 ) {
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "백작부인 행동은 진실이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index2].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
             
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index2].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index2].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index2].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
            
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index2].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index2], clnt_adr_sz);
           
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index2].user_sockfd, MSG, strlen(MSG));
             
            }
         }
         else if(player1[index1].user_character.contessa == 0){
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "백작부인 행동은 거짓이었습니다", sizeof(MSG) - 1);
            for(int i =0;i<4;i++){
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[i], clnt_adr_sz);
            }
            memset(MSG, '\0', sizeof(MSG));
            strncpy(MSG, "버릴 카드를 선택해주세요!", sizeof(MSG) - 1);
            sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            memset(MSG,'\0',sizeof(MSG));
            recvfrom(server_socket,MSG,100,0,(struct sockaddr*)&temp_addr,&clnt_adr_sz);
            if (strstr(MSG, "duke") != NULL) {
               player2[index1].user_character.duke--;
               sprintf(MSG, "공작 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }
            else if (strstr(MSG, "assassin") != NULL){
               player2[index1].user_character.assassin--;
               sprintf(MSG, "암살자 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
           
            }    
            else if (strstr(MSG, "ambassador") != NULL) {
               player2[index1].user_character.ambassador--;
               sprintf(MSG, "대사 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
             
            }    
            else if (strstr(MSG, "captain") != NULL) {
               player2[index1].user_character.captain--;
               sprintf(MSG, "사령관 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
           
            }    
            else if (strstr(MSG, "contessa") != NULL) {
               player2[index1].user_character.contessa--;
               sprintf(MSG, "백작부인 카드를 잃었습니다!");
               sendto(server_socket,MSG,100,0,(struct sockaddr*)&client_addr[index1], clnt_adr_sz);
            
            }    
            else {
               memset(MSG, '\0', sizeof(MSG));
               strncpy(MSG, "버릴 카드를 다시 선택해주세요!", sizeof(MSG) - 1);
               write(player1[index1].user_sockfd, MSG, strlen(MSG));
              
            }
         }
         break;
      }
   }
}