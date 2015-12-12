# HTTP-Client
create HTTP request 1.0 on C 
-----------------------------------------------
login: roich
personal details: Roi Cohen 301063814
Ex2 - HTTP client

Files:
	1)Readme
	2)client.c
-----------------------------------------------	
Exercise Description:

8 methods:
----------------------------------------------------------------------
1)void timehandle(char**,int*,char*,int*,char*,int*,char*,int*);
2)void fixTime(char***,char**,int**);

	those 2 methods take the time from argv[time_flag] and check if the input correct.
	Then convert the time from string to int with my method cToi.
----------------------------------------------------------------------
3)void urlhandle(char**,char **,char **,char **,char **,char*,int*);
	
	take url from the input(argv[url_flag]) than cut it for two parts:
	One is the host: www.EXAMPLE.com 
	Second is the port: cut  the colon and convert the port to int.
-----------------------------------------------------------------------
4)bool check(int);
5)void check2(int,int,int,int,int);
	
	Those two method checks if the iput is OK
	The first one check allways if the argc is in the range.
	The second check if the user insert -u or things like that.
	I create counter for each varibale and check if he is correct.
----------------------------------------------------------------------
6)int cToi(const char *s);

	This method conevrt the char* to int and return it.
----------------------------------------------------------------------
7)void printUsage();

	This method print "Usage: client [-h] [-d <time-interval> ] <URL>\n"
----------------------------------------------------------------------
8)void createConnection(char** ,char*,char*,char*,int,int,int,int);

	-First I try to get the host with the function gethostbyname.
	-Then insert the port inside the struct sockaddr_in with htons().
	-Then I do the "DNS" convert from "www.google.com" to 8.8.8.8:
		cli.sin_addr.s_addr =((struct in_addr*)(hp->h_addr))->s_addr;
	-Create the socket.
	-Create the response varibale with calloc.
	-Check if the request is HEAD or GET with h_flag and add the corent string to the request:
		request = GET  {For Example}
	-Then add the http:// and the host:
		request = GET http://www.EXAMPLE.com
	-Check if I got Path(if its isnt NULL) add "/" + path
		request = GET http://www.EXAMPLE.com/bla/bla.jpeg
	-Add to the request the string : HTTP/1.0\r\n\n like he should.
		request = GET http://www.EXAMPLE.com/bla/bla.jpeg HTTP/1.0
		
	-Check if the time_flag is up so add to the request "If-Modified-Since:" 
		and the Time and after it the format of the request : \r\n\n
		
		request = GET http://www.EXAMPLE.com/bla/bla.jpeg HTTP/1.0
		If-Modified-Since: <time interval>
	
	-Then we can finnaly write the request.
	-Read from the buffer and relaoc if necesery.
	-Finnaly print the total received response bytes.
----------------------------------------------------------------------

	
