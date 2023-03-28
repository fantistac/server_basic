#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

int main()
{
	struct sockaddr_in server_addr;
	int fd = 0, ret = 0;
	char BUF[BUFSIZ];
	socklen_t server_len;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd == -1)
	fprintf(stderr, "socket error: %s\n", strerror(errno));


	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9527);

	ret = inet_aton("127.0.0.1", &server_addr.sin_addr.s_addr);
	if(ret == 0){
		fprintf(stderr, "translate error\n");
	}

	while( fgets(BUF, BUFSIZ, stdin) ){
		int ret = sendto(fd, BUF, strlen(BUF), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if(ret == -1)
				fprintf(stderr, "connect error: %s\n", strerror(errno));
		else{ 
				ret = recvfrom(fd, BUF, BUFSIZ, 0, NULL, NULL);
		if(ret == -1)
				fprintf(stderr, "read error: %s\n", strerror(errno));

				write(STDOUT_FILENO, BUF, ret);
				if(ret == -1)
						fprintf(stderr, "write2 error: %s\n", strerror(errno));
		}
	}
return 0;
}
