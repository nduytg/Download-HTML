//D-HTML-Download
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

//Tham số dòng lệnh
//Format: $ <MSSV> http://students.iitk.ac.in/courses
int main(int argc, char **argv)
{
	char *page;
	
	if(argc == 1)
	{
		//help();
		exit(1);
	}
	else if (argc > 2)
	{
		page = argv[2];
		printf("Download: %s",argv[2]);
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
	
	//hints.ai_family = AF_INET;	//IPv4
	//hints.ai_family = AF_INET6;	//IPv6
	
	hints.ai_family = AF_UNSPEC;	//IPv4 hay IPv6 gi cung dc
	hints.ai_socktype = SOCK_STREAM;	//TCP for html connections
	//hints.ai_flags = AI_PASSIVE;		//Auto fill IP
	
	DSock = getaddrinfo("www.vnexpress.net","http",&hints,&info);
	
	if(DSock != 0)
	{
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(DSock));
		exit(1);
	}
				
	//clear up struct addrinfo info!
	freeaddrinfo(info);
}
