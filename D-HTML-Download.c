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
#define SERVICE "html"
#define BUFFSIZE 4096	//4K

char *build_get_query(char *host, char *page);
void help();

//Tham số dòng lệnh
//Format: $ <MSSV> www.vnexpress.net (2 tham so)
int main(int argc, char **argv)
{
	char *page, *host;
	char ip[INET_ADDRSTRLEN];
	int DSock;
	
	if(argc == 2)
	{
		//page = argv[1];
		printf("Download: %s",argv[1]);
	}
	else
	{
		help();
		exit(EXIT_FAILURE);
	}
	
	int retcode;				//return code
	struct addrinfo hints;
	struct addrinfo *info;		//pointer to results
	
	//Make the struct empty!
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;		//IPv4 hay IPv6 gi cung dc
	hints.ai_socktype = SOCK_STREAM;	//TCP for html connections
	//hints.ai_flags = AI_PASSIVE;		//Auto fill IP
	
	//Lay thong tin tu host
	retcode = getaddrinfo(argv[1],SERVICE,&hints,&info);
	
	if(retcode != 0)
	{
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(retcode));
		exit(EXIT_FAILURE);
	}

	//Convert ip stored in info->ai_addr->sin_addr to char* ip with length INET_ADDRSTRLEN
	inet_ntop(info->ai_family, &(((struct sockaddr_in *)info->ai_addr)->sin_addr),ip,INET_ADDRSTRLEN);
	DSock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	 
	 if(DSock == -1)
	 {
		 //Failed!
		 printf("Cannot create socket!\n");
		 exit(EXIT_FAILURE);
	 }
	 
	 if(connect(DSock,info->ai_addr,info->ai_addrlen) < 0)
	 {
		 printf("Cannot connect to %s - %s\n",argv[1],ip);
		 exit(EXIT_FAILURE);
	 }
	 printf("Connected to %s - %s\n",argv[1],ip);
	
	char *html_query = build_get_query(host, page);
	printf("Query content:\
						\n\t<<BEGIN>>\
						%s\
						\n\t<<END!>>\n",html_query);
	
	//###Send html request###
	//Loop toi khi goi dc het goi tin qua ben server
	//retcode = 0;
	int bytes_sent;
	retcode =  0;
	
	do
	{
		bytes_sent = send(DSock,html_query,strlen(html_query),0);
		if(bytes_sent == -1)
		{
			printf("Failed to send HTML-request\n");
			exit(EXIT_FAILURE);
		}
		retcode += bytes_sent;
		
	}while(retcode < strlen(html_query));
	//##################################
				
				
	//###Receive response from server###
	char *buffer[BUFFSIZE];
	memset(buffer, 0, sizeof(buffer));
	int bytes_recv;
	
	do
	{
		bytes_recv = recv(DSock,buffer,BUFFSIZE,0);
		if(bytes_recv == -1)
		{
			printf("Failed to receive response!");
			exit(EXIT_FAILURE);
		}
		
		
	}
	while(bytes_recv > 0);
	//##################################
				
				
	//clear up struct addrinfo info!
	freeaddrinfo(info);	
	free(html_query);
	free(host);
	free(page);	
	
	
	//close socket
	close(DSock);
	exit(EXIT_SUCCESS);
}
//-------------End main() function-----------

void help()
{
	printf("Please type with this format\
				\n\t ./MSSV [path]\
				\n\tEx: ./1312084 www.vnexpress/tin-tuc/\n");
}

//Build HTML query request!
char *build_get_query(char *host, char *page)
{
	char *query;
	char *getpage = page;
	char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	
	//Khuc nay chua hieu???
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
