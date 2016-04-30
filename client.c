/*
Client.c
Author:Roi Cohen
ID:301063814
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <ctype.h>

#define TRUE 0
#define FALSE 1
#define FAILURE -1
#define TIMESIZE 128
#define CHARSIZE 4
#define REQUEST_SIZE 2048
#define RESPONSE_SIZE 2048
#define BUFFER_SIZE 1024
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

typedef enum {false, true} bool;

/*Deceleration*/
void timehandle(char**,int*,char*,int*,char*,int*,char*,int*);
void fixTime(char***,char**,int**);
void urlhandle(char**,char **,char **,char **,char **,char*,int*);
bool check(int);
void check2(int,int,int,int,int);
void createConnection(char** ,char*,char*,char*,int,int,int,int);
int cToi(const char *s);
void printUsage();


int main(int argc, char* argv[]){

/*Variables->>if not modify the date so d,m,h=1*/
	char * ch_port;
	char * ch_host;
	char * ch_path;
	char * ch_protocol;
	char *url;
	char timebuf[TIMESIZE];
	
	char d[CHARSIZE]="0";
	char h[CHARSIZE]="0";
	char m[CHARSIZE]="0";

	int h_flag=FAILURE;
	int d_flag=FAILURE;
	int time_flag=FAILURE;
	int url_flag=FAILURE;
	int port;

	int day=0;
	int hour=0;
	int minute=0;
	int i=1;
	
	int counter_d=0;
	int counter_h=0;
	int counter_t=0;
	int counter_u=0;
	
	time_t currtime;

	char const colon[CHARSIZE]=":";
	char const *http="http://";
	
/*-while loop to get place and insert to flags*/
	while(i<argc && (check(argc)==TRUE))
	{
		if(!(strcmp(argv[i],"-h"))){
			h_flag=i;
			counter_h++;
			}
		if((!(strcmp(argv[i],"-d"))))
		{
			d_flag=i;
			counter_d++;
			if(argv[i+1]!=NULL){
				time_flag=d_flag+1;
				counter_t++;
			}
			if(argv[i+1]==NULL){
				printUsage();
				exit(FAILURE);
			}
		}				
		if(((strstr(argv[i],http)))){
			url_flag=i;
			counter_u++;
		}
		
		i++;
	}
/*Check input*/

	check2(argc,counter_d,counter_u,counter_h,counter_t);
		
	if((counter_d > 1) || (counter_u > 1) || (counter_h > 1) || (counter_t > 1)){
			printUsage();
			exit(FAILURE);
	}
	int tmp=counter_d + counter_u + counter_h + counter_t; 		
	if(tmp!=(argc-1)){
		printf("wrong input\n");
		exit(FAILURE);
	}

/*-----------------------URL handle-----------------------*/
	if(url_flag!=FAILURE && (check(argc)==TRUE)){
		url=argv[url_flag];		
		urlhandle(&url,&ch_host,&ch_port,&ch_protocol,&ch_path,":",&port);
	}
	else
	{
		printUsage();
		exit(FAILURE);
	}
/*-----------------------Time handle-----------------------
add this line just if the user insert time--------------*/
	if(time_flag!=FAILURE && (check(argc)==TRUE))
	{	
		char *tmp;
		int counter=0;
		tmp=argv[time_flag];
		while(*tmp){
			if(*tmp==':')
				counter++;
				tmp++;
			}
		if(counter==2){
			char *ch=strtok(argv[time_flag],colon);
			timehandle(&ch,&time_flag,d,&day,h,&hour,m,&minute);
		}
		else{
			printf("wrong input\n");
			exit(FAILURE);
		}
	}
	currtime= time(NULL);
	currtime-=(day*24*3600+hour*3600+minute*60);
	strftime(timebuf,TIMESIZE, RFC1123FMT, gmtime(&currtime));

//	myPrint(d,h,m,ch_protocol,ch_host,ch_path,timebuf,port,time_flag,h_flag);//DEBUGG
	createConnection(argv,ch_host,ch_path,timebuf,port,h_flag,url_flag,time_flag);
	return 0;

}
/*--------------------create Connection method------------------------*/
void createConnection(char ** argv,char* host,char* path,char* time,int port,int h_flag,int url_flag,int time_flag){
	
	int sd;
	struct sockaddr_in cli;
	int cli_len = sizeof(cli);
	struct hostent *hp;	
	char req[REQUEST_SIZE];
	int received_response_bytes;
	memset(&cli, 0, cli_len);
	
/*get the host from argv then use the method gethostbyname*/
	if(!(hp=gethostbyname(host))){
		printf("wrong input\n");
		exit(FAILURE);
	}

/*update port on sockaddr_in(cli) then do DNS finally use the Internet add family*/
	bzero((char *) &cli,cli_len);
	cli.sin_port =htons(port);
	cli.sin_family=AF_INET;
	cli.sin_addr.s_addr =((struct in_addr*)(hp->h_addr))->s_addr;

/*Create Socket*/
	if ((sd = socket(PF_INET,SOCK_STREAM, 0)) < 0){
		perror("	Socket");
		exit(FAILURE);
	}

/*Connect to socket*/
	if((connect(sd,(struct sockaddr*)&cli,cli_len))<0){
		perror("connect");
		exit(FAILURE);
	}

	char buff[BUFFER_SIZE];
	int resp_len=RESPONSE_SIZE;
	int bytes_r=0;
	memset(&buff, 0, sizeof(buff));
	memset(&req, 0, sizeof(req));

/*create the response */
	char* response = (char*)calloc(resp_len, sizeof(char));
	if(response == NULL) {
		perror("calloc");
		exit(FAILURE);
	}
/*if the user insert head*/
	if(h_flag!=FAILURE)
		 strcat(req,"HEAD ");
	else
		 strcat(req,"GET ");
		 
/*add the http:// , the host  and the version HTTP/1.0 to the head of the request*/	 
	strcat(req,"http://");	
	strcat(req,host);
	
/*add path to the request,dont forget to put / before*/	
	if(path!=NULL){
			strcat(req,"/");
			strcat(req,path);
		}
	strcat(req, " HTTP/1.0\r\n\n");

/*add time to the request if the user inesrt -d [<time interval>]*/	
	if (time_flag!=FAILURE){
        strcat(req, "If-Modified-Since: ");
        strcat(req,time);
        strcat(req, "\r\n\n");
   }

	printf("HTTP request =\n%s\nLEN = %d\n", req, (int) strlen(req));	
/*Write to Socket*/
	if((write(sd,req, sizeof(req))) < 0){
		perror("write");
		exit(FAILURE);
	}
/*read from buffer*/	
	while((received_response_bytes = read(sd,buff,sizeof(buff))) > 0)
	{

	 if(received_response_bytes< 0) {
       perror("read");
       exit(FAILURE);

    }
    printf("%s",buff);
    
    bytes_r+=received_response_bytes;
    if(received_response_bytes>= (resp_len-bytes_r))
    {
      response = realloc(response, (resp_len *= 2));
      if(response==NULL){
         perror("realloc");
         exit(FAILURE);
       }
    }

	}
	   printf("\nTotal received response bytes:%d\n",bytes_r);
}
/*----------------<<<<<END----->createConnection method>>>>>-------------*/

/*-----------------------timehandle method---------------------
take the ch** that is the time with colon and then
first check if I got time and the ch differnt from NULL
then use strcpy each time ,cToi method and strtok method
each time I take the *ch insert to d/m/y then convert to int(for the connect)
then cut it with strtok.
-----------------------timehandle method---------------------*/
void  timehandle(char** ch,int* time_flag,char* d,int* day,char* h,int* hour,char* m,int* minute){
	int i=0;		

	while (*time_flag!=FAILURE && ch != NULL){

		if(i==0)											/*DAY*/
			if(*ch!=NULL){				
				fixTime(&ch,&d,&day);		
		}
		if(i==1)										  /*HOUR*/
			if(*ch!=NULL)
			  fixTime(&ch,&h,&hour);
			  
		if(i==2){									  /*MINUTE*/
			if(*ch!=NULL)
			{
				strcpy(m,*ch);
				if(!(isdigit(*m))){
				printUsage();
				exit(FAILURE);
				}
			*minute=atoi(m);
			break;
			}
		else{
			printUsage();
			exit(FAILURE);
		}
		break;
		}
		i++;
	}
}
/*------------------fix Time methood----------------------
Checks id the time isnt char----------------------------*/
void fixTime(char*** ch,char** t,int** it){
	strcpy(*t,**ch);
	if(!(isdigit(**t))){
		printUsage();
		exit(FAILURE);
	}
	**it=cToi(*t);
	**ch = strtok(NULL,":");
	
}
/*-------------<<<<<<<END--->timehandle method>>>>>>---------------*/


/*--------------------urlhandle method--------------------
First we take to protocol till colon found
Then take from host // till /
In ch_port insert from colon till end of host string,
If port not insert then update just port(int)
Then insert to ch_path all the rest
Take off the port from the host
Finally Delete : from port
--------------------urlhandle method--------------------*/
void urlhandle(char** url,char ** ch_host,char ** ch_port,char ** ch_protocol,char ** ch_path,char* colon,int *port)
{
	*ch_protocol=strtok(*url,colon);
	*ch_host=strtok(NULL,"/");
	
	if(*ch_host==NULL){						//insert just http://
		printf("wrong input\n");
		exit(FAILURE);
	}	
	if((strchr(*ch_host,':'))){			//if port is inserted
		*ch_port=strchr(*ch_host,':');
		*port=cToi(*ch_port);

	}
	else
		*port=80;
	*ch_path=strtok(NULL," ");
	*ch_host=strtok(*ch_host,colon);
	*ch_port+=1;
}
/*------------<<<<<<<<END--->urlhandle method>>>>>>>-------------*/

/*----------------------my check methods-----------------
Checks if the user insert correct input
----------------------my check method-----------------*/
bool check(int argc){
	if(argc>=2 && argc<=5)
		return TRUE;
	return FALSE;
}
void check2(int argc,int counter_d,int counter_u,int counter_h,int counter_t){		
	if((counter_d > 1) || (counter_u > 1) || (counter_h > 1) || (counter_t > 1)){
			printUsage();
			exit(FAILURE);
	}
	int tmp=counter_d + counter_u + counter_h + counter_t; 		
	if(tmp!=(argc-1)){
		printf("wrong input\n");
		exit(FAILURE);
	}

}

/*------------<<<<<END--->my check method>>>>>>>>>--------*/



/*----------------------my atoi method-----------------
When I need atoi method to convert from char* to int
I use this method to convert from String to int.
----------------------my atoi method-----------------*/
int cToi(const char *s)
{
	int sign=1;
	if(*s == '-')
   	sign = FAILURE;
	s++;
	int num=0;
	while(*s){
		num=((*s)-'0')+num*10;
		s++;
	}
return num*sign;
}
/*------------------print Usage method------------------*/
void printUsage() {
    printf("Usage: client [-h] [-d <time-interval> ] <URL>\n");
}

