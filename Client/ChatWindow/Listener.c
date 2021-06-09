#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 2231
#define MSGBUFSIZE 256
/* gcc Listener.c -o Listener.out */

int main(int argc, char *argv[])
{
	char IP[20], groupName[20];
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	int yes = 1, fd, id;

	FILE* fptr = fopen("pidS.txt","w");
    id = getpid();
    fprintf(fptr,"%d",id);
    fclose(fptr);
    printf("listener pid = %d\n",id);

	argc > 1 ? strcpy(IP, argv[1]) : 0;
	argc > 2 ? strcpy(groupName, argv[2]) : 0;
	
	memset(&addr, 0, sizeof(addr));
	memset(&mreq, 0, sizeof(mreq));

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		perror("socket");
		return 1;
	}

	/* allow multiple sockets to use the same PORT number
	*/
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0)
	{
		perror("Reusing ADDR failed");
		return 1;
	}

	/* set up destination address
	*/
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);

	/* bind to receive address
	*/
	if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return 1;
	}

	/* use setsockopt() to request that the kernel join a multicast _ip
	*/
	mreq.imr_multiaddr.s_addr = inet_addr(IP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt");
		return 1;
	}

	/* now just enter a read-print loop
	*/
	
	printf("This is the chat window of group %s\n", groupName);
	
	while (1) 
	{
		char msgbuf[MSGBUFSIZE];
		int nbytes, addrlen = sizeof(addr);
		
		nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *)&addrlen);
		if (nbytes < 0)
		{
			perror("recvfrom");
			return 1;
		}
		msgbuf[nbytes] = '\0';
		puts(msgbuf);
		
	}

	return 0;
}
