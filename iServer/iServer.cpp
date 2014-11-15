//#include <arpa/inet.h> //For doing htonl
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <unistd.h>	//For read and write functions ie for processes
#include <sys/socket.h>	
#include <sys/types.h>
#include <netinet/in.h>	//For initializing part
#include <netdb.h>
#include <vector>

using namespace std;

int UpdateDataBase(char theUserName[255], char thePassword[255]){
	
	ofstream fp("UsersData.txt", ios::app);
	//cout<<theUserName<<" "<<thePassword<<endl;
	fp <<theUserName<<" "<<thePassword<<endl;
	fp.close();
	return 0;	
}

int AlreadyUser(char theUserName[255]){
	
	char U[255];
	char P[255];
	int counter = 0;
	fstream fp2("UsersData.txt");	
	fp2>>U>>P;
	while(fp2){
		if(strcmp(U, theUserName) == 0){
			cout<<counter<<endl;
			fp2.close();
			return counter;
		}
		counter ++;
		fp2 >>U>>P;
	}
	fp2.close();
	return -1;

}

int CheckPassword(char theUserName[255] ,char thePassword[255]){
	int counter = 0;
	char U[255];
	char P[255];
	
	fstream fp2("UsersData.txt");	
	fp2>>U>>P;
	while(fp2){
		if(strcmp(U, theUserName) == 0){
			if(strcmp(P, thePassword) == 0){
				fp2.close();
				return counter;
			}
			else
				return -1;
		}
		counter++;
		fp2 >>U>>P;
	}
	fp2.close();
	return -1;
}


int counter = 1;
void deleteFileAt(string dir_name, int k){
	DIR *dirp = opendir(dir_name.c_str());
	if(dirp == NULL)
		return;
	struct dirent *dp;
	
	while(dirp){
		if((dp = readdir(dirp)) !=NULL){

			if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
			{
	
				if(counter == k){
					if(dir_name != "")
					{
						string ftoDel = "rm -r " + dir_name + "/" + dp->d_name;
						system(ftoDel.c_str());
						break;
					}
				}
				counter ++;
				//cout<<"|"<<endl;
				deleteFileAt(dir_name + "/" + dp->d_name, k);
			}
			
		}
		
		else{
			break;
		}				
	}
}

void writeLS(string dir_name,int i, ofstream *fp){
	DIR *dirp = opendir(dir_name.c_str());
	if(dirp == NULL)
		return;
	struct dirent *dp;
	
	while(dirp){
		if((dp = readdir(dirp)) !=NULL){

			if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
			{
				for(int j = 0; j<i; j++){
					*fp<<" ";
				}
				*fp<<"|"<<counter<<"|";
				counter ++;
				*fp<<dp->d_name<<endl;
				//cout<<"|"<<endl;
				writeLS(dir_name + "/" + dp->d_name,i+2, fp);
			}
			
		}
		
		else{
			break;
		}				
	}
}


void AddAClient(int socket, char theUserName[255]){
		
		char thePassword[255];
		char message[255];
		int n = read(socket,(void *)theUserName, 255);
		if(n<0){
			cout<<"Error reading from socket"<<endl;
			exit(1);
		}
		while(AlreadyUser(theUserName) != -1){	
			strcpy(message, "Choose a different Username");
			n = write(socket,(void *)message, 255);
			if(n<0){
			cout<<"Error writing through the socket"<<endl;
			exit(1);
			}
			n = read(socket,(void *)theUserName, 255);
			if(n<0){
				cout<<"Error reading from socket"<<endl;
				exit(1);
			}
		}
		strcpy(message, "Thank You, we'll remember your name");
		n = write(socket,(void *)message, 255);
		if(n<0){
			cout<<"Error writing through the socket"<<endl;
			exit(1);
		}
		n = read(socket,(void *)thePassword, 255);
		if(n<0){
			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		cout<<"Client Added "<<theUserName<<": "<<thePassword<<endl;
		UpdateDataBase(theUserName, thePassword);

		
}

void ConfirmClient(int socket,  char theUserName[255]){
		
		char thePassword[255];
		char message[255];
		int n = read(socket,(void *)theUserName, 255);
		if(n<0){
			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		
		n = read(socket,(void *)thePassword, 255);
		if(n<0){
			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		int testPass = CheckPassword(theUserName, thePassword);
		while(testPass == -1){	
			strcpy(message, "Incorrect Username/ Password");
			n = write(socket,(void *)message, 255);
			if(n<0){
			cout<<"Error writing through the socket"<<endl;
			exit(1);
			}
			n = read(socket,(void *)theUserName, 255);
			if(n<0){
				cout<<"Error reading from socket"<<endl;
			exit(1);
			}
			n = read(socket,(void *)thePassword, 255);
			if(n<0){
				cout<<"Error reading from socket"<<endl;
			exit(1);
			}
			testPass = CheckPassword(theUserName, thePassword);
		}
		cout<<"Client :"<<theUserName<<" is connected"<<endl;
		strcpy(message, "You are Good to go");
		n = write(socket,(void *)message, 255);
			if(n<0){
			cout<<"Error writing through the socket"<<endl;
			exit(1);
		}

		
}

void DeleteFileRequest(int socket, char UserName[255]){
		

		string folder = UserName;

		
		string loc = folder + "/.a";
		string removefile = "rm " + loc;
		system(removefile.c_str());
		ofstream  fp(loc.c_str());

		writeLS(folder , 8, &fp);
		counter = 1;
		fp.close();


		int theNumber;	
		int n = read(socket,&theNumber, sizeof(theNumber));
		if(n<0){

			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		int Number = ntohl(theNumber);
		deleteFileAt(folder ,Number);
		counter = 1;
		cout<<UserName<<" deleted a file"<<endl;
}

void ShareRequest(int socket, char UserName[255]){
	char shareUser[255];
	char fileName[255];
	char message[255];

	
	int n = read(socket,(void *)shareUser, 255);
	if(n<0){

		cout<<"Error reading from socket"<<endl;
	exit(1);
	}
	int shareUserId = AlreadyUser(shareUser);
	while(shareUserId == -1){	
		strcpy(message, "There is No Such User");
		n = write(socket,(void *)message, 255);
		if(n<0){

		cout<<"Error writing through the socket"<<endl;
		exit(1);
		}
		n = read(socket,(void *)shareUser, 255);
		if(n<0){

			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		shareUserId = AlreadyUser(shareUser);
	}
	strcpy(message, "Share Request sent ...");
	n = write(socket,(void *)message, 255);
	if(n<0){

	cout<<"Error writing through the socket"<<endl;
	exit(1);
	}

	n = read(socket,(void *)fileName, 255);
	if(n<0){

		cout<<"Error reading from socket"<<endl;
	exit(1);
	}
	//UpdateShareRequests(shareUserId, UserId, fileName);
	string filename = shareUser;
	cout<<UserName<<" has shared a file"<<endl;
	filename = filename + ".txt";
	ofstream requestWriter(filename.c_str(), ios::app);
	requestWriter<<UserName<<" "<<fileName<<endl;
	requestWriter.close();
}


void ExcecuteShareRequest(int socket,  char UserName[255], int counter){
	int n;
	int theNumber;
	n = read(socket,&theNumber, sizeof(theNumber));
	if(n<0){

		cout<<"Error reading from socket"<<endl;
		exit(1);
	}
		


	int Number = ntohl(theNumber);
	cout<<"Counter is "<<counter<<" "<<Number<<endl;
	if(Number <= counter){
		string Sharer, theFile;
		string User = UserName;
		string Fname = User + ".txt";
		string Tempname = Fname + "_";
		ifstream fp1(Fname.c_str());	
		ofstream fp2(Tempname.c_str());
		int i = 0;
		fp1>>Sharer>>theFile;
		while(fp1){
			if(i == Number){
				//fp2<<theUserName<<" "<<thePassword<<endl;	
				
				string command = "rsync -ab " + Sharer + "/" + theFile + "  " + User + "/"; 
				cout<<" i is "<<i<<" ";
				cout<<command<<endl;
				system(command.c_str());
				fp1>>Sharer>>theFile;
			}
			else{
				fp2<<Sharer<<" "<<theFile<<endl;	
				fp1>>Sharer>>theFile;
			}
			i++;
		}
		cout<<UserName<<" has accepted a file share request"<<endl;
		string command = "rm " + Fname;
		system(command.c_str());
		command = "mv " + Tempname + " " + Fname;
		system(command.c_str());
		fp2.close();
		fp1.close();
	}
}

void ShowShareRequests(int socket,  char UserName[255]){
	char sharer[255];
	char fileName[255];
	int n;
	//string Sharer, theFile;
	string Fname = UserName;
	Fname = Fname + ".txt";
	ifstream fp2(Fname.c_str());	
	if(fp2.is_open()){
		fp2>>sharer>>fileName;
		int counter = 0;
		while(fp2){
			n = write(socket, &sharer, 255);
			if(n<0){

				cout<<"Error writing from socket"<<endl;
			exit(1);
			}
			n = write(socket, &fileName, 255);
			if(n<0){

				cout<<"Error writing from socket"<<endl;
			exit(1);
			}
			counter ++;
			fp2>>sharer>>fileName;
		}
	fp2.close();
	}
	strcpy(sharer,"Finished");
	n = write(socket, &sharer, 255);
	if(n<0){

		cout<<"Error writing from socket"<<endl;
	exit(1);
	}
	cout<<UserName<<" has requested shared files "<<endl;
	int theNumber;
	n = read(socket,&theNumber, sizeof(theNumber));
	if(n<0){

		cout<<"Error reading from socket"<<endl;
	exit(1);
	}
	int Number = ntohl(theNumber);
	cout<<"The number for executing shared File Request is "<<Number<<endl;
	if(Number == 1)
		ExcecuteShareRequest(socket, UserName, counter);
	
}


void doProcessing(int socket){
	int n;

	int theNumber;
	n = read(socket,&theNumber, sizeof(theNumber));
	if(n<0){

		cout<<"Error reading from socket"<<endl;
	exit(1);
	}
	int Number = ntohl(theNumber);
	char User[255];


	//cout<<"The Client is a fresh Login "<<Number<<endl;
	//attend(theString);
	if(Number == 0){
		//ADDING A CLIENT
		cout<<"A new User is trying to connect..."<<endl;
		AddAClient(socket, User);
	}
	else if(Number == 1){
		//Confirm Client
		cout<<"Existing User is trying to connect..."<<endl;
		ConfirmClient(socket, User);
	}
	n = read(socket,&theNumber, sizeof(theNumber));
	if(n<0){

		cout<<"Error reading from socket"<<endl;
	}
	Number = ntohl(theNumber);
	while(Number != 9){
		//cout<<"The Number sent later by the client is "<<Number<<endl;
	
		if(Number == 2){
			//DELETING REQUEST FROM THE CLIENT
			cout<<User<<" wants to Delete a File "<<endl;	
			DeleteFileRequest(socket, User);
		}
		else if(Number == 3){
			cout<<User<<" wants to Share a File "<<endl;	
			ShareRequest(socket, User);
		}
		else if(Number == 4){
			cout<<User<<" wants to See Share Requests "<<endl;	
			ShowShareRequests(socket, User);
		}
		else 
		{
			exit(1);
		}

		n = read(socket,&theNumber, sizeof(theNumber));
		if(n<0){

			cout<<"Error reading from socket"<<endl;
		exit(1);
		}
		Number = ntohl(theNumber);
	}
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
	port = 5001;
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