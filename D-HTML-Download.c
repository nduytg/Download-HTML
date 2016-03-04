//D-HTML-Download
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>


//Tham số dòng lệnh
//Format: $ <MSSV> http://students.iitk.ac.in/courses
int main(int argc, char **argv)
{
	char *page;
	char ip[INET_ADDRSTRLEN];
	
	if(argc == 1)
	{
		//help();
		exit(1);
	}
	else if (argc > 2)
	{
		page = argv[1];
		printf("Download: %s",argv[1]);
	}
	else
	{
		//????
	}
	
	int DSock;
	struct addrinfo hints;
	struct addrinfo *info;		//pointer to results
	
	//Make the struct empty!
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;	//IPv4 hay IPv6 gi cung dc
	hints.ai_socktype = SOCK_STREAM;	//TCP for html connections
	//hints.ai_flags = AI_PASSIVE;		//Auto fill IP
	
	DSock = getaddrinfo(argv[1],NULL,&hints,&info);
	
	if(DSock != 0)
	{
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(DSock));
		exit(1);
	}
	//inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
	//struct sockaddr_in *tmp = info->
	inet_ntop(info->ai_family, &(((struct sockaddr_in *)info->ai_addr)->sin_addr),ip,INET_ADDRSTRLEN);
	//sprintf("Dia chi cua trang %s la %s\n",page,ip);
	printf("Dia chi cua trang: %s la %s\n",page,ip);
				
	//clear up struct addrinfo info!
	freeaddrinfo(info);
}
