#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

char stringUID[10];
char command[255];
int uid;

struct sockaddr_in address;
int sock = 0, valread;
struct sockaddr_in serv_addr;

void getUID(){
    uid = getuid();
    sprintf(stringUID, "%d", uid);
    send(sock, stringUID, strlen(stringUID), 0);
}
  
int main(int argc, char const *argv[]) {

    // char *hello = "Hello from client";
    char buffer[2048] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    getUID();

    if(uid){
        read(sock, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%s>%s\n", argv[2], argv[4]);
        send(sock, buffer, strlen(buffer), 0);
    }

    memset(buffer, 0, sizeof(buffer));
    
    read(sock, buffer, sizeof(buffer));
    printf("%s\n", buffer);

    if(!strcmp(buffer, "User/Password Invalid! Try again!")){
        return 0;
    }

    while(1){
        printf("QUERY --> ");
        scanf(" %[^\n]", command);

        send(sock, command, strlen(command), 0);
        memset(buffer, 0, sizeof(buffer));

        read(sock, buffer, sizeof(buffer));
        printf("%s\n", buffer);
    }

    return 0;




    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );
    // return 0;
}