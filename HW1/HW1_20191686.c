#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct hostent *hostp; // find sever pointer
    struct sockaddr_in server; // socket address
    int sock; // socket
    char send_buf[BUFSIZ], recv_buf[BUFSIZ]; // buffer
    int bytesread; // read byte value
    int current = 0; // download current value
    char *f; // find filename pointer
    int total = 0; // file total byte
    int cnt = 0; // header count, 10 percent count
    char *filename; // filename
    // char *header = ""; // 
    char *answer; // total size
    char *ptr; // pointer
    char *p; // response body pointer
    char *status; // status value
    int is = 1; // headerFlag

    printf("Student ID : 20191686\n");
    printf("Name : Hyewon Choi\n");

    if(argc != 4) {
        (void) fprintf(stderr,"usage: %s host port\n", argv[0]);
        exit(1);
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1); 
    }
    if ((hostp = gethostbyname(argv[1])) == 0) {
        fprintf(stderr,"%s: unknown host\n",argv[1]);
        exit(1); 
    }

    memset((void *) &server, 0, sizeof (server));
    server.sin_family = AF_INET;
    memcpy((void *) &server.sin_addr, hostp->h_addr, hostp->h_length); 
    server.sin_port = htons((u_short)atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&server, sizeof (server)) < 0) { 
        (void) close(sock);
        fprintf(stderr, "connect");
        exit(1); 
    }

    // http Request message
    sprintf(send_buf, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-agent: HW1/1.0\r\nConnection: close\r\n\r\n", argv[3], argv[1]);

    // write
    if (write(sock, send_buf, strlen(send_buf)) < 0) {
        perror("write");
        exit(1); 
    }

    // find filename
    f = strtok(argv[3], "/");
    while (f != NULL){
        filename = f;
        f = strtok(NULL, "/");
    }

    FILE *file = fopen(filename, "wb");
    file = fopen(filename, "wb");

    // read
    while ((bytesread = read(sock, recv_buf, sizeof recv_buf)) > 0){
        // http response message header
        if (is){
            // p : find body
            p = strstr(recv_buf, "\r\n\r\n");
            p += 4;

            // ptr, status : find total byte, status
            ptr = strtok(recv_buf, "\r\n\r\n");
            while (ptr != NULL){
                if (cnt == 0) status = ptr;
                cnt++;
                ptr = strtok(NULL, "\r\n");
                if (cnt == 6) answer = ptr;
            }
            ptr = strtok(status, " ");
            ptr = strtok(NULL, " ");

            // print status
            while (ptr != NULL){
                if (!strcmp(ptr, "200")) {
                    status = "200";
                    break;
                }
                printf("%s ", ptr);
                ptr = strtok(NULL, " ");
            }

            if (strcmp(status, "200")) {
                printf("\n\n");
                break;
            }
            // print total size
            else {
                ptr = strtok(answer, ":");
                while (ptr != NULL){
                    answer = ptr;
                    ptr = strtok(NULL, ":");
                }
                total = atoi(answer);
                printf("Total Size %d bytes\n",total);
            }
        }

        // seperate header, body
        if (is) {
            int body_len = 0;
            char *it;
            it = recv_buf;
            while (it+2 != p){
                body_len+=strlen(it)+2;
                it += strlen(it)+2;
            }
            // printf("%s\n",p);
            fwrite(p, sizeof(char), bytesread-body_len-2, file);
            current += bytesread-body_len-2;
            is= 0;
            continue;
        }
        fwrite(recv_buf, sizeof(char), bytesread, file);
        current += bytesread;
        double percent = (double)current/(double)total*100;
        if (percent >= cnt){
            printf("Current Downloading %d/%d (bytes) %.f%%\n", current, total, percent); 
            cnt += 10;
        }       
        if (percent >= 100){
            printf("Download Complete: %s, %d/%d\n", filename, current, total);
        }
        
    } 
}
