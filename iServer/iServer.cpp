#include <iostream>
#include <cstdlib>
#include <unistd.h>	//For read and write functions ie for processes
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>	//For initializing part
#include <string.h>	//For bzero initialization
//#include <arpa/inet.h> //For doing htonl
using namespace std;

void attend(char* theString){
}


void doProcessing(int socket){
	int n;

	int theNumber;
	n = read(socket,&theNumber, sizeof(theNumber));
	if(n<0){
		cout<<"Error reading from socket"<<endl;
	}
	int Number = ntohl(theNumber);
	char theString[255];
	/*if(theNumber == 1){
		n = read(socket,(void *)theString, 255);
		cout<<"Call Show on the Folder"<<endl;
	}
*/
	cout<<"The Number sent by the client is "<<Number<<endl;
	//attend(theString);
	n = read(socket,(void *)theString, 255);
	if(n<0){
		cout<<"Error reading from socket"<<endl;
	}

	cout<<"The String sent by the client is "<<theString<<endl;
	n = read(socket,(void *)theString, 255);
	if(n<0){
		cout<<"Error reading from socket"<<endl;
	}

	cout<<"The String sent by the client is "<<theString<<endl;
	/*n = write(socket,&theNumber, sizeof(theNumber));
	if(n<0){
		cout<<"ERROR writing to socket"<<endl;
	}*/

}

int main(int argc, char const *argv[])
{
	int socket_fd, newsocket_fd, port;
	int pid;
	socklen_t client_size, server_size;
	struct sockaddr_in server_addr, client_addr;

	//Creating a socket
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"Error opening socket"<<endl;
		exit (1);
	}

	//Initialize the socket
	bzero(&server_addr, sizeof(server_addr));
	port = 5005;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);	//function converts the unsigned short integer hostshort from host byte order to network byte order
	server_size = sizeof(server_addr);
	//Binding the socket
	
	if(bind(socket_fd, (struct sockaddr *) &server_addr, server_size)){
		cout<<"Error binding the socket"<<endl;
		exit(1);	
	}

	//Listening
	listen(socket_fd, 5);
	cout<<"Listening on port "<<port<<"..."<<endl;
	client_size = sizeof(client_addr);
	
	while(1){
		newsocket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_size );
		if(newsocket_fd<0){
			cout<<"ERROR on accept"<<endl;
			exit(1);
		}

		//Create a child process 
		pid = fork();
		if(pid < 0){
			cout<<"ERROR on fork"<<endl;
		}
		if(pid == 0){
			//This is child process
			close(socket_fd);
			//while(1)
			doProcessing(newsocket_fd);
			close(newsocket_fd);
			exit(0);
		}
		else{
			//close(socket_fd);
		}
	}

	return 0;
}