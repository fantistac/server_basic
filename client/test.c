#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<stddef.h>

#define CLIENT_ADDR "client_file.socket"
#define SERVER_ADDR "server_file.socket"

int main()
{
	struct sockaddr_un server_addr, client_addr;
	int fd = 0, ret = 0;
	char BUF[BUFSIZ];
	socklen_t server_len, client_len;
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd == -1)
	fprintf(stderr, "socket error: %s\n", strerror(errno));

	unlink(CLIENT_ADDR);

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SERVER_ADDR);
	server_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path);
	client_addr.sun_family = AF_UNIX;
	strcpy(client_addr.sun_path, CLIENT_ADDR);
	client_len = offsetof(struct sockaddr_un, sun_path) + strlen(client_addr.sun_path);

	bind(fd, (struct sockaddr *)&client_addr, client_len);

	ret = connect(fd, (struct sockaddr *)&server_addr, server_len);
	if(ret == -1)
			fprintf(stderr, "connect error: %s\n", strerror(errno));
	while( fgets(BUF, BUFSIZ, stdin) ){ 
		int ret = write(fd, BUF, strlen(BUF));
		if(ret == -1)
				fprintf(stderr, "write error: %s\n", strerror(errno));
		else{ 
				ret = read(fd, BUF, BUFSIZ);
				if(ret == -1)
					fprintf(stderr, "read error: %s\n", strerror(errno));

				write(STDOUT_FILENO, BUF, ret);
				if(ret == -1)
						fprintf(stderr, "write error: %s\n", strerror(errno));
		}   
	}
	close(fd);
	return 0;
}
