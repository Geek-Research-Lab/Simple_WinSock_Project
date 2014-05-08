/* WinSock - LocalHost */
#include<stdio.h> /*required for printf() */
#include<stdlib.h> /*required for exit() */
#include<string.h> /*required for memcpy() and strcpy()*/
#include<windows.h> /*required for winsock and sleep()*/
#include<fcntl.h> /*required for File I/O constants*/
#include<sys/stat.h> /*required for File I/O constants*/
#include<io.h> /*required for open(), close() and eof()*/
#define PORT_NUM 1050 /*arbitrary port number for the server*/
#define MAX_LISTEN 1 /*Maximum number of listens to queue*/
#define SIZE 256 /*Size in bytes of transfer buffer*/
void main(int argc, char *argv[])
{
	WORD wVersionRequested = MAKEWORD(1,1); /*WSA functions*/
	WSADATA wsadata; /*WinSock API data structure*/
	unsigned int remote_s; /*remote socket description*/
	struct sockaddr_in remote_addr; /*remote internet address*/
	struct sockaddr_in server_addr; /*server internet address*/
	unsigned char bin_buf[SIZE]; /*Buffer for file transfer*/
	unsigned int fh; /*File handle*/
	unsigned int length; /*length of buffers transferred*/
	struct hostent *host; /*Structure for gethostbyname()*/
	struct in_addr address; /*structure for internet address*/
	char host_name[256]; /*String for host name*/
	int addr_len; /*internet address length*/
	unsigned int local_s; /*local socket descriptor*/
	struct sockaddr_in local_addr; /*local internet address*/
	struct in_addr remote_ip_addr; /*remote IP address*/
	/*Check if number of command line arguments is valid or not*/
	if(argc!=4)
	{
		printf("error");
		exit(1);
	}
	/*Initialization of WinSock*/
	WSAStartup(wVersionRequested, &wsaData);
	/*Copy host name into host_name*/
	strcpy(host_name, argv[1]);
	/*gethostbyname()*/
	host = gethostbyname(argv[1]);
	if(host == NULL)
	{
		printf("error");
		exit(1);
	}
	/*Copy the 4-byte client IP address into IP address structure*/
	memcpy(&address, host->h_addr, 4);
	/*Create a socket for remote host*/
	remote_s = socket(AF_INET, SOCK_STREAM, 0);
	/*Server (remote) socket's address information*/
	server_addr.sin_family = AF_INET; /*address family to be used*/
	server_addr.sin_port = htons(PORT_NUM); /*port number to be used*/
	server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(address)); /*IP address*/
	/*Connecting with listening server*/
	connect(remote_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
	/*Opening and reading the '.exe file' */
	if((fh = open(argv[2], O_RDONLY | O_BINARY, S_IREAD | S_IWRITE)) == -1)
	{
		printf("error : Unable to open the file '%s'\n", argv[2]);
		exit(1);
	}
	/*output message letting us know the sending of executable file*/
	printf("Sending '%s' to remote server on '%s' \n", argv[2], argv[1]);
	/*Sending .exe file to remote host*/
	while(!eof(fh))
	{
		length = read(fh, bin_buf, SIZE);
		send(remote_s, bin_buf, length, 0);
	}
	/*Close the .exe file that was sent to the remote host*/
	close(fh);
	/*Closing the socket*/
	closesocket(remote_s);
	/*cleanup winsock*/
	WSACleanup();
	/*output message letting us know the sending of executable file*/
	printf(" '%s' is executing on remote server \n", argv[2]);
	/*delay to allow everything to cleanup*/
	Sleep(100);
	/*Initialization of WinSock*/
	WSAStartup(wVersionRequested, &wsaData);
	/*create a new socket to receive output file from remote server*/
	local_s = socket(AF_INET, SOCK_STREAM, 0);
	/*Server (remote) socket's address information*/
	local_addr.sin_family = AF_INET; /*address family to be used*/
	local_addr.sin_port = htons(PORT_NUM); /*port number to be used*/
	local_addr.sin_addr.s_addr = hton1(INADDR_ANY); /*listen on any IP address*/
	bind(local_s, (struct sockaddr *)&local_addr, sizeof(local_addr));
	/*listen for connections queuing up on MAX_LISTEN*/
	listen(local_s, MAX_LISTEN);
	/*Connection acceptance...xoxo <3 */
	addr_len = sizeof(remote_addr);
	remote_s = accept(local_s, (struct sockaddr *)&remote_addr, addr_len);
	/*Copying 4-byte client IP address into IP address structure*/
	memcpy(&remote_ip_addr, &remote_addr.sin_addr.s_addr, 4);
	/*Create and open output file for writing*/
	if((fh = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE)) == -1)
	{
		printf("error");
		exit(1);
	}
	/*receive output file from server*/
	length = SIZE;
	while(length > 0)
	{
		length = recv(remote_s, bin_buf, SIZE, 0);
		write(fh, bin_buf, length);
	}
	/*close output file that was recived from the remote host*/
	close(fh);
	/*closing the sockets*/
	closesocket(local_s);
	closesocket(remote_s);
	/*output final status message*/
	printf("Execution of '%s' and transfer of output to '%s' done \n", argv[2], argv[3]);
	/*Cleanup Winsock*/
	WSACleanup();
	}
