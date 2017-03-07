//		   D-HTML-Download		#
//Ten: 		Nguyen Duc Duy		#
//MSSV: 	1312084				#
//###############################
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//Thu vien tao thu muc
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//DEFINE
#define USERAGENT "HTMLGET 1.0"
#define SERVICE "html"
#define PORT 80
#define BUFFSIZE 8192	//8K buffer
#define MSSV "1312084"

//Struct tu tao
struct stat st = {0};
//typedef enum {false,true} bool;

//Ham tao query de goi toi server
char *build_get_query(char *host, char *page);

//Ham lay ten host tu link goc
char *get_host(char *path);

//Ham lay page can down tu link goc
char *get_page(char *path, char *host);

//Ham lay file name can down tu link goc
char *get_filename(char *path);
//-----------------------------

//Ham get link tu file index.html ra
char *getlink(FILE *fp);

//Ham download file le
char* download_file(char *path, char *folder_name, int mode);

//Ham download chinh
bool main_download(char *path);

//Ham chua thong tin huong dan nhap input
void help();


//Tham số dòng lệnh
//Format: $ <MSSV> www.vnexpress.net (2 tham so)
int main(int argc, char **argv)
{
	//Check arguments
	if(argc == 2)
	{
		printf("Download: %s\n",argv[1]);
	}
	else
	{
		//Hien thi huong dan su dung^^
		help();
		exit(EXIT_FAILURE);
	}
	
	//-----DOWNLOAD FILES-----
	main_download(argv[1]);
	
	exit(EXIT_SUCCESS);
}
//-------------End main() function-----------------------


//-----------Cac ham su dung trong chuong trinh----------
void help()
{
	printf("Please type with this format\
				\n\t\t ./MSSV [path]\
				\n\tEx: ./1312084 www.vnexpress.net/tin-tuc/\n");
}

char *get_host(char *path)
{
	//Input se co dang: http://example.com/index.html
	//Output: example.com hoac www.example.com
	//Download file index goc va loop, get link de down het nhung file con lai
	char *dup = (char*) malloc(strlen(path)+1);
	strcpy(dup,path);
	char *pt, sav;
	char *host;
	
	pt = strtok(dup,"/");
	if (pt && !strcmp(pt,"http:"))
	{
		pt = strtok(NULL,"/");
	}
	host = (char*) malloc(strlen(pt)+1);
	strcpy(host,pt);
	free(dup);
	return host;
}

//Get /tin-tuc trong: www.vnexpress.net/tin-tuc
char *get_page(char *path, char *host)
{
	printf("\n");
	char *pt;
	char *page = NULL;
	char *dup = (char*)malloc(strlen(path)+1);
	char *dup2 = (char*)malloc(strlen(path)+1);
	char *del_dup2;
	
	strcpy(dup,path);
	strcpy(dup2,path);
	del_dup2 = dup2;
	pt = strtok(dup,"/");
	
	if ( strcmp(pt,"http:") == 0 )
	{
		dup2 += 7 + strlen(host);		//+7 = skip "http://"
		dup2[strlen(dup2)] = '\0';
	}
	else
	{
		dup2 += strlen(host);
	}
	
	page = (char*) malloc(strlen(dup2)+1);
	strcpy(page,dup2);
	printf("Return Page: %s\n",page);
	free(dup);
	free(del_dup2);
	return page;
}

char *get_filename(char *path)
{
	bool check = false;
	//printf("In get_filename(), path: %s\n",path);
	if(path[strlen(path)-1] == '/')	
		check = true;	//Day la 1 folder!
		
	char *pt, *sav;
	char *filename;
	char *dup = (char*) malloc(strlen(path)+1);
	strcpy(dup,path);
	pt = strtok(dup,"/");
	sav = pt;
	
	while(pt = strtok(NULL,"/")) {	sav = pt; }
	
	if(check == true)
	{
		//printf("Folder zoi`, hehe\n");
		filename = (char*) malloc(strlen(sav)+2);
		strcpy(filename,sav);
		strcat(filename,"/");
		printf("Folder: %s\n",filename);
	}
	else
	{
		filename = (char*) malloc(strlen(sav)+1);
		strcpy(filename,sav);
		printf("File name: %s\n",filename);
	}
	free(dup);
	return filename;
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

//Download file index goc va loop, get link de down het nhung file con lai
bool main_download(char *path)
{
	//Get host, page
	char *host, *page, *filename;
	char *query;
	char *buffer;
	host = get_host(path);
	page = get_page(path, host);
	filename = get_filename(path);
	//printf("File name got: %s\n",filename);
	
	//Tao folder va luu file vao folder
	char *folder_name = NULL;
	if(filename[strlen(filename) - 1] == '/')
	{
		char *tmp = strdup(filename);
		tmp[strlen(tmp) - 1] = '\0';
		folder_name = (char*)malloc(strlen(tmp) + 4 + 10);		
		//+4 for "./" voi '\0' va '/';
		strcpy(folder_name,"./");
		char a[] = "1312084_";
		strcat(folder_name,a);
		strcat(folder_name,tmp);
		strcat(folder_name,"/");
		
		printf("Folder name: %s\n",folder_name);
		 
		if (stat(folder_name, &st) == -1) 
		{
			if(!mkdir(folder_name, 0777))
				printf("Made dir!\n");
			else
			{
				printf("Cannot create new directory! :(\n");
				printf("Plz try again :(\n");
				exit(EXIT_FAILURE);
			}
		}
		free(tmp);
	}	
	//---------Het phan code tao folder---------

	//DOWNLOAD file index goc!!!
	//Hoac download 1 file
	char *main_file = download_file(path, folder_name, 1);
	
	//Kiem tra coi file nay la file gi?
	//Neu la file binh thuong: .mp3, .doc thi ngung lai, ko down nua
	bool file = true;		//Mac dinh la down file, true la down 1 file thoi!!
	if(filename[strlen(filename)-1] == '/')
	{
		printf("Download them cac file tu link trong file index goc.\n");
		file = false;
	}
	
	//Neu la down 1 trang web thi moi vo vong lap nay
	if (file == false)
	{		
		printf("Download mot folder!\n");
		//Get link and download file con!
		FILE *fp = fopen(main_file,"r");
		if(fp == NULL)
		{
			printf("Error: Cannot open main file!!\n");
			return false;
		}
		
		printf("Doc file index goc thanh cong\n");
		char *short_link;
		
		while( !feof(fp))
		{
			//printf("Ok 2\n");
			if( (short_link = getlink(fp)) == NULL )
			{
				printf("Ok 2.5\n");
				break;
			}

			//Download file con!
			char *full_link = (char*)malloc(strlen(host) + strlen(page) + strlen(short_link) + 1);
			strcpy(full_link,host);
			strcat(full_link,page);
			strcat(full_link,short_link);
			//printf("Full link: %s\n",full_link);
			
			if(full_link!=NULL)
				download_file(full_link, folder_name, 2);
			else
				printf("Link Error!\n");
				
			//printf("Ok\n");
			free(short_link);
			free(full_link);	
		};
		fclose(fp);
		printf("Folder is downloaded!\n");
	}
	else
	{
		printf("File is downloaded!\n");
	}
	
	printf("\nDownload Completed!^^\n");
	
	//Giai phong bo nho
	free(main_file);
	free(host);
	free(page);
	free(filename);
	free(folder_name);
	return true;
}

//Ham nay co 2 mode:
//1: download file index cua folder goc hoac down 1 file le
//2: download cac files linh tinh con lai
//Tra ve file name!!
char* download_file(char *path, char *folder_name, int mode)
{
	//Get host, page
	char *host, *page, *filename;
	char *html_query;
	char ip[INET_ADDRSTRLEN];
	int DSock;
	int retcode;				//return code
	struct addrinfo hints;
	struct addrinfo *info;		//pointer to results

	host = get_host(path);
	page = get_page(path, host);
	filename = get_filename(path);
	printf("Get file name: %s\n",filename);

	
	//***Neu khong phai la folder goc thi ko down!!!!!!
	//Kiem tra co phai downfoler ko, neu la folder thi skip, return 1!!
	//Folder co dau '/'
	if(mode==2 && filename[strlen(filename) - 1] == '/')
	{
		printf("%s is a sub folder, skip this folder!\n",filename);
		printf("***NOTE: Only download main folder!!!\n");
		free(host);
		free(page);
		free(filename);
		return NULL;
	}
	//else, down file nay ^^

	//Make the struct empty!
	memset(&hints, 0, sizeof(hints));	
	hints.ai_family = AF_UNSPEC;		//IPv4 hay IPv6 gi cung dc
	hints.ai_socktype = SOCK_STREAM;	//TCP for html connections
	//hints.ai_flags = AI_PASSIVE;		//Auto fill IP
	
	//Lay thong tin tu host
	retcode = getaddrinfo(host,"http",&hints,&info);
	if(retcode != 0)
	{
		
		fprintf(stderr,"Failed at getaddrinfo(): %s\n", gai_strerror(retcode));
		printf("Host: %s\n",host);
		printf("Page: %s\n",page);
		return NULL;
	}

	//Convert ip stored in info->ai_addr->sin_addr to char* ip with length INET_ADDRSTRLEN
	inet_ntop(info->ai_family, &(((struct sockaddr_in *)info->ai_addr)->sin_addr),ip,INET_ADDRSTRLEN);
	DSock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	 
	if(DSock == -1)
	{
		//Failed!
		printf("Cannot create socket!\n");
		//clear up struct addrinfo info!
		freeaddrinfo(info);	
		free(html_query);
		free(host);
		free(page);	
		return NULL;
	}
	 
	if(connect(DSock,info->ai_addr,info->ai_addrlen) < 0)
	{
		printf("Cannot connect to %s - %s\n",host,ip);
		//clear up struct addrinfo info!
		freeaddrinfo(info);	
		free(html_query);
		free(host);
		free(page);
		return NULL;
	}
	printf("Connected to %s - %s\n",host,ip);
	

	html_query = build_get_query(host, page);
	printf("Query content:\
						\n<<BEGIN>>\n%s\n<<END!>>\n",html_query);
	
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
			//clear up struct addrinfo info!
			freeaddrinfo(info);	
			free(html_query);
			free(host);
			free(page);	
			return NULL;
		}
		retcode += bytes_sent;
		
	}while(retcode < strlen(html_query));
	printf("Send HTML-REQUEST succeed\n");

	//clear up struct addrinfo info!
	freeaddrinfo(info);	
	free(html_query);
	free(host);
	free(page);	
	//##################################
					

	//###Receive response from server###
	//########## A.K.A - DOWNLOAD ######
	FILE *fp;
	char *temp_name; 
	
	if(folder_name != NULL)
	{
		printf("**Folder name: %s\n",folder_name);
		temp_name = (char*)malloc(strlen(filename) + strlen(MSSV) + strlen(folder_name) + 10);
		strcpy(temp_name,folder_name);
		//strcat(temp_name,"/");
	}
	else
	{
		//Chi down 1 file
		temp_name = (char*)malloc(strlen(filename) + strlen(MSSV) + 2);
		strcpy(temp_name,MSSV);
		strcat(temp_name,"_");
		if(filename[strlen(filename)-1] == '/')
		{
			printf("This is a folder!\n");
			filename[strlen(filename)-1] = '\0';
		}
	}
	strcat(temp_name,filename);
	printf("File name sau khi cat: %s\n",temp_name);
	
	//Xu ly tempname don gian
	//bool save = false;
	if(temp_name[strlen(temp_name)-1] == '/')
		temp_name[strlen(temp_name)-1] = '\0';
	
	fp = fopen(temp_name,"w");
	
	if (fp == NULL)
	{
		printf("Failed to write file: %s\n",temp_name);
		return NULL;
	}

	char buffer[BUFFSIZE];
	memset(buffer, 0, sizeof(buffer));
	int bytes_recv;
	long total = 0;
	long total_write = 0;
	bool htmlstart = 0;
	char *htmlcontent;

	printf("HTML content:\n");
	while((bytes_recv = recv(DSock,buffer,BUFFSIZE,0)) > 0)
	{
		total+=bytes_recv;
		//printf("First Packet: \n%s\n",buffer);	
		if(htmlstart == 0)
		{
			//Skip header!!!
			htmlcontent = strstr(buffer,"\r\n\r\n");
			int header_size = strlen(buffer) - strlen(htmlcontent) + 4;
			if(htmlcontent != NULL)
			{
				htmlstart = 1;
				htmlcontent += 4;		//Skip \r\n\r\n
				int real_bytes = bytes_recv - header_size;
				//printf("Header!!\nHTML content: %s\n",htmlcontent);
				total_write += fwrite (htmlcontent , sizeof(char), real_bytes, fp);
				//total_write += real_bytes;
				//printf("%s\n",htmlcontent);
				memset(buffer,0,bytes_recv);
				continue;
			}
		}
		else
		{
			htmlcontent = buffer;
		}

		if(htmlstart == 1)
		{
			//Ghi file!
			total_write += fwrite (buffer, sizeof(char), bytes_recv, fp);
			printf("%s\n",htmlcontent);
		}
		memset(buffer,0,bytes_recv);
	}
	
	if(bytes_recv < 0)
	{
		printf("Error when receiving data...\n");
	}
	fclose(fp);
	printf("TOTAL: %ld bytes\n",total);
	printf("WROTE: %ld bytes\n",total_write);
	//##################################
				
	//close socket
	close(DSock);	
	return temp_name;
}

//Code de xai ham getlink!!!
/*
FILE *fp = fopen("Test_get_link.html","r");
if(fp == NULL)
{
	printf("FAILED!\n");
	exit(EXIT_FAILURE);
}
printf("Open file thanh cong\n");

char *tmp;
while(!feof(fp))
{
	tmp = getlink(fp);
	printf("Link got: %s\n",tmp);
}
printf("SUCCEED!\n");
fclose(fp);
exit(EXIT_SUCCESS);
*/
// #######Het code get link#######

char *getlink(FILE *fp)
{
	if(fp == NULL)
	{
		printf("File poiter is NULL\n");
		return NULL;
	}

	char *link = NULL;
	char *pt1 = NULL, *pt2 = NULL;
	char buffer[BUFFSIZE];
	
	while(fgets(buffer,BUFFSIZE,fp))
	{
		if((pt1 = strstr(buffer,"href=\""))!=NULL)
		{
			pt1+=6;		//Skip chu href
			pt2 = strtok(pt1,"\"");
			break;
		}
	}
	
	if(pt2!=NULL)
	{
		//printf("strlen(pt2): %d\n",strlen(pt2));
		link = (char*) malloc(strlen(pt2)+1);
		strcpy(link,pt2);
		return link;
	}
	return NULL;
}


