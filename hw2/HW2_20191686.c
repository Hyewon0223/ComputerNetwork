#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	int bytesread, addrlen;
	int i;
	char buf[BUFSIZ];


	if (argc != 2) {
		(void) fprintf(stderr,"usage: %s port\n", argv[0]);
		exit(1);
	}
	if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}

    printf("Student ID : 20191686\n");
    printf("Name : Hyewon Choi\n");

	memset((void *) &server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons((u_short)atoi(argv[1]));

	if (bind(request_sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
		perror("bind");
		exit(1);
	}
	if (listen(request_sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(1);
	}
    for(;;){
        addrlen = sizeof(remote);
        new_sock = accept(request_sock,(struct sockaddr *)&remote, &addrlen);
        if (new_sock < 0) {
            perror("accept");
            exit(1);
        }
        printf("connection: Host IP %s, Port %d, socket %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), new_sock);

        bytesread = read(new_sock, buf, sizeof (buf) - 1);
        if (bytesread<=0) {
            printf("server: end of file on %d\n", new_sock);
            if (close(new_sock)) 
                perror("close");
            break;
        }
        buf[bytesread] = '\0';
        char *ptr; // pointer
        char *request_file;
        char *request_file_type;
        char buf_copy[BUFSIZ];
        char responseMessage[BUFSIZ];
        int headerCnt = 0;
        int fileSize = 0;
        char *userAgent;
        char *status;

        strcpy(buf_copy, buf);
        ptr = strtok(buf_copy, " \t\r\n");
        request_file = strtok(NULL, " \t\r\n") + 1;
        char request_file_copy[strlen(request_file)];
        strcpy(request_file_copy, request_file);

        request_file_type = strtok(request_file_copy,".");
        request_file_type = strtok(NULL,"\r\n");

        if (!strcmp(request_file_type,"jpg")) request_file_type = "image/jpeg";
        else request_file_type = "text/html";

        userAgent = strstr(buf,"Agent");
        ptr = strtok(buf, "\r\n");
        printf("%s\n",ptr); // -> GET /palladio.jpg HTTP/1.1
        while (ptr != NULL){
            headerCnt++;
            ptr = strtok(NULL, "\r\n");
        }
        printf("User-Agent: %s\n",strtok(userAgent+7," "));
        fflush(stdout);
        printf("%d headers\n",headerCnt-1);
        fflush(stdout);

        FILE *fp;
        if((fp = fopen(request_file, "r")) != NULL){
            fseek(fp, 0, SEEK_END);
            status = "200 OK";
            fileSize = ftell(fp);
            rewind(fp);
        }
        else{
            status = "404 NOT FOUND";
            fileSize = 0;
            printf("Server Error : No Such file ./%s!\n", request_file);
        }

        sprintf(responseMessage, "HTTP/1.0 %s\r\nConnection: close\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n", status, fileSize, request_file_type);

        int send_length = 0;
        write(new_sock, responseMessage, strlen(responseMessage));
        while(send_length != fileSize){
            memset(buf, 0, sizeof(buf));
            fread(buf, 1, 1, fp);
            buf[1] = '\0';
            write(new_sock, buf, 1);
            send_length += 1;
        }
        printf("finish %d %d\n", fileSize, send_length);
        fclose(fp);
    }
}
