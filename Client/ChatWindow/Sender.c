#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>/*close*/
#include <string.h> /*memset*/
#include <arpa/inet.h> /*inet_addr()*/
/* gcc Sender.c -o Sender.out */

#define PORT 2231

int main(int argc, char *argv[])
{
	char IP[20], name[20], groupName[20];
	struct sockaddr_in addr;
	int fd, id;

	FILE* fptr = fopen("pidC.txt","w");
    id = getpid();
    fprintf(fptr,"%d",id);
    fclose(fptr);
    printf("sender pid = %d\n",id);

	argc > 1 ? strcpy(IP, argv[1]) : 0;
	argc > 2 ? strcpy(name, argv[2]) : 0;
	argc > 3 ? strcpy(groupName, argv[3]) : 0;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);
	
	printf("Hello %s, enter your massages here to group %s\n", name, groupName);
	
	while (1)
	{
		char msg[200], toSend[200];
		int nbytes;
		
		fgets(msg, 100, stdin);
		sprintf(toSend, "\033[1;32m%s\033[0m : %s", name, msg);
		nbytes = sendto(fd, toSend, strlen(toSend), 0, (struct sockaddr*) &addr, sizeof(addr));
		if (nbytes < 0)
		{
			perror("sendto");
			return 1;
		}
		
	}
	return 0;
}


