#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<sys/un.h>
#include<stddef.h>

#define PORT 9527
#define SERVER_ADDR "/home/fantistac/NJUPA/test/server_file.socket"
int main()
{
	int sockfd, client_fd;
	int client_port;
	char client_id[BUFSIZ];
	int ret = 0, rb_fd = 0;
	struct sockaddr_un server_addr, client_addr;
	socklen_t client_len, server_len;
	char BUF[BUFSIZ];

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sockfd == -1) fprintf(stderr, "socket error: %s\n", strerror(errno));
	
	unlink(SERVER_ADDR);
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SERVER_ADDR);

	server_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path);
	ret = bind(sockfd, (struct sockaddr *)&server_addr, server_len);
	if(ret == -1) fprintf(stderr, "bind error: %s\n", strerror(errno));

	listen(sockfd, 20);
	
	while(1)
	{

		client_len = sizeof(client_addr);
		client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
		if(client_fd == -1) {
			fprintf(stderr, "accept error:\n", strerror(errno));
			exit(-1);
		}	
		client_len -= offsetof(struct sockaddr_un, sun_path);
		client_addr.sun_path[client_len] ='\0';	
		fprintf(stderr, "client socket = %s\n", client_addr.sun_path);
		while(1){	
		ret = read(client_fd, BUF, BUFSIZ);
		if (ret == -1){
			fprintf(stderr, "recvfrom error:%s\n", strerror(errno));
			exit(-1);
		} 
		else if(ret == 0){ 
			fprintf(stderr, "client %s is closed\n", client_addr.sun_path);			
		 	close(client_fd);
			exit(0);
		}else{
		write(STDOUT_FILENO, BUF, ret);
		for(int j = 0; j < ret; ++j){ 
				BUF[j] = toupper(BUF[j]);
 		}  
		client_len += offsetof(struct sockaddr_un, sun_path);	
		write(client_fd, BUF, ret);
		}
	}
	}
	close(sockfd);	
	return 0;
}

