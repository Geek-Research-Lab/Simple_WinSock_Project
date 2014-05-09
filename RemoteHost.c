#include<stdio.h> /*required for printf()*/
#include<stdlib.h> /*required for exit()*/
#include<string.h> /*required for memcpy() and strcpy()*/
#include<windows.h> /*required for WinSock and sleep()*/
#include<fcntl.h> /*required for file i/o constants*/
#include<sys/stat.h> /*required for file i/o constants*/
#include<io.h> /*required for open(), close() and eof()*/
#define PORT_NUM 1050 /*arbitrary port number for the server*/
#define MAX_LISTEN 1 /*Maximum number of listens to queue*/
#define IN_FILE "run.exe" /*Name given to transferred .exe file*/
#define TEXT_FILE "output" /*Name of output file for stdout*/
#define SIZE 256 /*size in bytes of transfer buffer*/
void main(void)
{
	WORD wVersionRequested = MAKEWORD(1,1); /*WSA functions*/
	WSADATA wsaData; /*WSA functions*/
	unsigned int remote_s; /*remote socket descriptor*/
	struct sockaddr_in remote_addr; /*remote internet address*/
	struct sockaddr_in server_addr; /*server internet address*/
	unsigned int local_s; /*Local socket descriptor*/
	struct in_addr local_ip_addr; /*local IP address*/
	int addr_len; /*internet address length*/
	unsigned char bin_buf[SIZE]; /*file transfer buffer*/
	unsigned int fh; /*File handle*/
	unsigned int length; /*length of the transfer buffers*/
	/*let the good times roll*/
	while(1)
	{
		/*winsock initialization*/
		WSAStartup(wVersionRequested, &wsaData);
		/*creating a socket*/
		remote_s = socket(AF_INET, SOCK_STREAM, 0);
		/*socket information*/
		remote_addr.sin_family = AF_INET; /*address family to use*/
		remote_addr.sin_port = htons(PORT_NUM); /*port number to use*/
		remote_addr.sin_addr.s_addr = hton1(INADDR_ANY); /*listen on any IP address*/
		bind(remote_s,(struct sockaddr *)&remote_addr,sizeof(remote_addr));
		/*output waiting message*/
		printf("Waiting for connection...\n");
		/*listen for connection*/
		listen(remote_s, MAX_LISTEN);
		/*Accepting connection...oh sugar! */
		local_addr addr_len = sizeof(local_addr);
		local_s = accept(remote_s,(struct sockaddr *)&local_addr,&addr_len);
		/*Copying the 4-byte client IP address into IP address structure*/
		memcpy(&local_ip_addr, &local_addr.sin_addr.s_addr, 4);
		/*output message stating acknowledgement*/
		printf("Connection accepted... reciving remote exe file \n");
		/*Open IN_FILE for remote exe file*/
		if((fh=open(IN_FILE, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE)) == -1)
		{
			printf("error: unable to open the exe file \n");
			exit(1);
		}
		/*receiving exe file from localhost*/
		length = 256;
		while(length > 0)
		{
			length = recv(local_s, bin_buf, SIZE, 0);
			write(fh, bin_buf, length);
		}
		/*close the recieved IN_FILE*/
		close(fh);
		/*closing sockets*/
		closesocket(remote_s);
		closesocket(local_s);
		/*cleanup winsock*/
		WSACleanup();
		/*message acknowledging execution of .exe */
		printf("Executing remote executable (stdout to output file) \n");
		/*execute remote executable file(in IN_FILE) */
		system(IN_FILE ">" TEXT_FILE);
		/*winsock initialization to re-open socket to send output file to localhost*/
		WSAStartup(wVersionRequested, &wsaData);
		/*creating a socket*/
		local_s = socket(AF_INET, SOCK_STREAM, 0);
		/*socket information*/
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(PORT_NUM);
		server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(local_ip_addr));
		/*connnecting with listening localhost*/
		connect(local_s,(struct sockaddr *)&server_addr,sizeof(server_addr));
		/*output message acknowledging transfer to client*/
		printf("Sending output file to localhost \n");
		/*open output file to send to client*/
		if((fh=open(TEXT_FILE, O_RDONLY | O_BINARY, S_IREAD | S_IWRITE)) == -1)
		{
			printf("error: unable to open \n");
			exit(1);
		}
		/*send output file to client*/
		while(!eof(fh))
		{
			length = read(fh, bin_buf, SIZE);
			send(local_s, bin_buf, length, 0);
		}
		/*closing output file*/
		close(fh);
		/*closing sockets*/
		closesocket(remote_s);
		closesocket(local_s);
		/*cleanup winsock*/
		WSACleanup();
		/*delay to allow cleanup process*/
		Sleep(100);
	}
}
