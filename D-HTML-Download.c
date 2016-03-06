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
#define PORT 80
#define BUFFSIZE 4096	//4K

char *build_get_query(char *host, char *page);
char *get_host(char *path);
char *get_page(char *path, char *host);
void test_strtok();

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
		printf("Download: %s\n",argv[1]);
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
	
	//Get host and page from agrv[1]
	host = get_host(argv[1]);
	page = get_page(argv[1],host);
	printf("\nHost: %s - Page: %s\n",host,page);
	
	//Lay thong tin tu host
	retcode = getaddrinfo(host,SERVICE,&hints,&info);
	
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
				\n\t\t ./MSSV [path]\
				\n\tEx: ./1312084 www.vnexpress/tin-tuc/\n");
}
void test_strtok()
{
	char str[] = "http://abc.com/duy/abc";
	char *pt;
	
	pt = strtok(str,"/");
	printf("\n%s",pt);
	while(pt!=NULL)
	{
		pt = strtok(NULL,"/");
		printf("\n%s",pt);
	}
}

char *get_host(char *path)
{
	//Input se co dang: http://example.com/index.html
	//Output: example.com hoac www.example.com
	char *dup = (char*) malloc(strlen(path)+1);
	strcpy(dup,path);
	char *pt;
	char *host;
	
	pt = strtok(dup,"/");
	printf("Path format: %s\n",dup);

	if (pt && !strcmp(pt,"http:"))
	{
		//printf("1");
		pt = strtok(NULL,"/");
	}

	host = (char*) malloc(strlen(pt)+1);
	strcpy(host,pt);
	free(dup);
	printf("Return host name: %s\n",host);

	return host;
}

//Get /tin-tuc trong: www.vnexpress.net/tin-tuc
char *get_page(char *path, char *host)
{
	printf("\n");
	char *pt;
	char *page;
	char *dup = (char*)malloc(strlen(path));
	strcpy(dup,path);
	
	printf("Original path: %s\n",path);
	pt = strtok(dup,"/");

	if ( strcmp(pt,"http:") == 0 )
	{
		path += 7 + strlen(host);
	}
	else
	{
		path += strlen(host);
	}
	
	page = (char*) malloc(strlen(path)+1);
	strcpy(page,path);
	printf("Return Page: %s",page);
	free(dup);
	
	return page;
}

//Build HTML query request!
char *build_get_query(char *host, char *page)
{
	char *query;
	char *getpage = page;
	char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	
	//Bo dau '/'
	if(getpage[0] == '/')
	{
		getpage = getpage + 1;
		fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0

	query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}
