#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>

#define PORT 9527

int main()
{
	int sockfd, client_fd;
	int client_port;
	char client_id[BUFSIZ];
	int ret = 0, rb_fd = 0;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;
	char BUF[BUFSIZ];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd == -1) fprintf(stderr, "socket error: %s\n", strerror(errno));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9527);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(ret == -1) fprintf(stderr, "bind error: %s\n", strerror(errno));

	
	while(1)
	{

		client_len = sizeof(client_addr);
		int ret = recvfrom(sockfd, BUF, BUFSIZ, 0, (struct sockaddr *)&client_addr, &client_len);
		if (ret == -1) fprintf(stderr, "recvfrom error:%s\n", strerror(errno));
		else{
				inet_ntop(AF_INET, (void *)&client_addr.sin_addr.s_addr, client_id, BUFSIZ);
				client_port = ntohs(client_addr.sin_port);
				fprintf(stderr, "client ip = %s, client port = %d is connecting\n",client_id, client_port);  

					write(STDOUT_FILENO, BUF, ret);
					for(int j = 0; j < ret; ++j){
						BUF[j] = toupper(BUF[j]);
 			 		}
					sendto(sockfd, BUF, ret, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
 		} 
 		
		}
	return 0;
}

