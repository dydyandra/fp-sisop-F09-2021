#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#define PORT 8080

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
// char buffer[2048]={0};

struct loggedin{
    char id[255];
    char pwd[255];
}account;

char syntaxError[2048] = "RESULT--> SYNTAX ERROR!";
char perrError[2048] = "RESULT--> PERMISSION DENIED!";
char successCreate[2048] = "RESULT--> USER IS CREATED!";
char dbError[2048] = "RESULT--> DATABASE IS NOT FOUND, TRY AGAIN!";
char workDB[2048] = "RESULT--> DATABASE CHANGED!";

char cwDatabase[255] = {0} ;
char folderDB[] = "databaseku";
int cekRoot = 0;
int cus = 0;

int authorize(char string[]){
    FILE *fp;
    char namaFile[1024];
    char cek[80];
    sprintf(namaFile, "%s", folderDB);
    strcat(namaFile, "/users/user.txt");
    printf("%s", namaFile);

    fp = fopen(namaFile, "r");

    if(fp == NULL) {
      perror("Error opening fileE");
      return(-1);
    }
    
    while(fgets(cek, sizeof(cek), fp)!=NULL){
        if(strcmp(cek, string) == 0){
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}


void connectToClient(){
    char buffer[2048]; memset(buffer, 0, sizeof(buffer));
    char message[2048]; memset(message, 0, sizeof(message));

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, sizeof(buffer));

    char superMsg[2048] = "Connection accepted!\n\tLogged in as: ";
    if(strcmp(buffer, "0") == 0){
        // strcpy(message, superMsg);
        cus=1;
        cekRoot=1;
        strcpy(account.id, "root");
        strcat(superMsg, account.id);
        strcpy(message, superMsg);
    }   

    if(cus == 0){
        send(new_socket, "cek", 10, 0);

        memset(buffer, 0, sizeof(buffer));

        valread = read(new_socket, buffer, sizeof(buffer));
        char errMsg[2048] = "User/Password Invalid! Try again!";

        if(authorize(buffer)){
            cus = 1;
            char acc[2048] = "Connection accepted!\n\tLogged in as: ";
            char *ptr1 = buffer;

            // strcat(acc, account.id);
            // strcpy(message, acc);

            char* ptr = buffer;
            char* cut = strchr(buffer, '>') + 1;
            strcpy(account.pwd, cut);
            strncpy(account.id, ptr, strlen(ptr) - (strlen(cut) + 1) );
            
            strcat(acc, account.id);
            strcpy(message, acc);
        }
        else strcpy(message, errMsg);
    }
    send(new_socket, message, strlen(message), 0);
}


char *queryCreate(char *str){
    char *hi;
    char message[255] = {0};
    memset(message, 0, sizeof(message));

    char id[255] = {0};
    memset(id, 0, sizeof(id));

    char pass[255] = {0};
    memset(pass, 0, sizeof(pass));

    char temp[1024] = {0};
    memset(temp, 0, sizeof(temp));
    strcpy(temp, str);

    char *a; char *b = temp;
    int i = 0;
    FILE *fp;

    a=strtok_r(b, " ", &b);
    for(;a!=NULL;a=strtok_r(NULL, " ", &b)){
        if(i == 2){
            strcpy(id, a);
        }

        else if (i == 3){
            if(strcmp(a, "IDENTIFIED")){
                strcpy(message, syntaxError);
                hi = message;
                return hi;
            }
        }

        else if(i == 4){
            if(strcmp(a, "BY")){
                strcpy(message, syntaxError);
                hi = message;
                return hi;
            }
        }

        else if (i > 5){
                strcpy(message, syntaxError);
                hi = message;
                return hi;
        }

        else if (i == 5){
            strncpy(pass, a, strlen(a) - 1);
        }
        i++;
    }

    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%s>%s", id, pass);

    fp = fopen("databaseku/users/user.txt", "a");
    fprintf(fp, "%s\n", temp);
    fclose(fp);

    strcpy(message, successCreate);
    hi = message;
    return hi;

}

char *queryUse(char string[]){
    FILE *fp;
    char message[255]={0};
    memset(message, 0, sizeof(message));

    char* hi = string + 4;

    char path[100];
    memset(path, 0, sizeof(path));

    int tanpaTitik2 = strlen(hi) - 1;
    strncpy(path, hi, tanpaTitik2);

    char namaFile[255] = {0};
    memset(namaFile, 0, sizeof(namaFile));

    sprintf(namaFile, "%s/%s", folderDB, path);
    strcat(namaFile, "/access.txt");

    // fp = fopen(namaFile, "r");
    // if(fp = NULL){
    //     strcpy(message, dbError);
    //     hi = message;
    //     return hi;
    // }

    // char cek[80];

    // while(fgets(cek, sizeof(cek), fp)!=NULL){
    //     int cekLogin = strncmp(cek, account.id, strlen(account.id));
    //     if(cekLogin == 0){
    //         fclose(fp);
    //         strcpy(message,workDB);
    //         hi = message;
    //         sprintf(cwDatabase, "%s", path);
    //         return hi;
    //     }
    // }

    // fclose(fp);
    strcpy(message, namaFile);
    hi = message;
    return hi;
}

void takeQuery(char buffer[]){
        char message[2048] = {};
        memset(message, 0, sizeof(message));
        
        int getQuery = strlen(buffer) - 1;

        int cekCreate = strncasecmp(buffer, "create user", 11);
        int cekUse = strncasecmp(buffer, "use", 3);
        
        if(buffer[getQuery] != ';'){
            strcpy(message, syntaxError);
        }
        // printf("bisa masuk abis if 1");

        else if(!cekCreate){
            if(!cekRoot){
                strcpy(message, perrError);
            }
            else strcpy(message, queryCreate(buffer));
            
        }

        else if(!cekUse){
            // printf("bisa masuk");
            strcpy(message, queryUse(buffer));
        }

        send(new_socket, message, strlen(message), 0);

}


int main() {

    // pid_t pid, sid;        // Variabel untuk menyimpan PID

    // pid = fork();     // Menyimpan PID dari Child Process

    // /* Keluar saat fork gagal
    //  * (nilai variabel pid < 0) */
    // if (pid < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // /* Keluar saat fork berhasil
    // *  (nilai variabel pid adalah PID dari child process) */
    // if (pid > 0) {
    //     exit(EXIT_SUCCESS);
    // }

    // umask(0);

    // sid = setsid();
    // if (sid < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // if ((chdir("/")) < 0) {
    //     exit(EXIT_FAILURE);
    // }

    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);


    // printf("bisa oy");
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    char message[2048] = {}; 
    FILE *fp;
    // memset(message, 0, sizeof(message));

    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // printf("bisa oy");
    connectToClient();
    // printf("bisa oy");

    // char pathDB[2048];
    // sprintf(pathDB, "/%s", folderDB);
    mkdir("databaseku", 0777);
    mkdir("databaseku/users", 0777);

    fp = fopen("databaseku/users/user.txt", "a");
    if(fp) fclose(fp);
    else if (fp == NULL) {
      perror("Error opening file");
      return(-1);
    }

    while(1){
        memset(buffer, 0, sizeof(buffer));
        memset(message, 0, sizeof(message));

        valread = read(new_socket, buffer, sizeof(buffer));
        if(valread == 0){
            cus = 0;
            cekRoot = 0;
            connectToClient();
            continue;
        }

        takeQuery(buffer);

    }
    
}
