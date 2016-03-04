//D-HTML-Download
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define USERAGENT "HTMLGET 1.0"

char *build_get_query(char *host, char *page);

//Tham số dòng lệnh
//Format: $ <MSSV> www.vnexpress.net
int main(int argc, char **argv)
{
	char *page;
	char ip[INET_ADDRSTRLEN];
	int DSock;
	
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
	
	int retcode;
	struct addrinfo hints;
	struct addrinfo *info;		//pointer to results
	
	//Make the struct empty!
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;	//IPv4 hay IPv6 gi cung dc
	hints.ai_socktype = SOCK_STREAM;	//TCP for html connections
	//hints.ai_flags = AI_PASSIVE;		//Auto fill IP
	
	retcode = getaddrinfo(argv[1],"http",&hints,&info);
	
	if(retcode != 0)
	{
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(retcode));
		exit(1);
	}

	//Convert ip stored in info->ai_addr->sin_addr to char* ip with length INET_ADDRSTRLEN
	inet_ntop(info->ai_family, &(((struct sockaddr_in *)info->ai_addr)->sin_addr),ip,INET_ADDRSTRLEN);
	printf("Dia chi cua trang: %s la %s\n",argv[1],ip);
				
	//Using sockets from here :D
	/*
	 * #include <sys/types.h>
		#include <sys/socket.h>

		int socket(int domain, int type, int protocol); 
	 * 
	 */
	 
	 DSock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	 if(DSock == -1)
	 {
		 //Failed!
		 exit(1);
	 }
	 
	 if(connect(DSock,info->ai_addr,info->ai_addrlen) < 0)
	 {
		 printf("Cannot connect to %s - %s\n",argv[1],ip);
		 exit(1);
	 }
	 printf("Connected to %s - %s\n",argv[1],ip);
	
				
				
	//clear up struct addrinfo info!
	freeaddrinfo(info);
	return 0;
}


//Build HTML query request!
char *build_get_query(char *host, char *page)
{
	char *query;
	char *getpage = page;
	char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	
	/*
	if(getpage[0] == '/')
	{
		getpage = getpage + 1;
		fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0
	*/

	query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}
