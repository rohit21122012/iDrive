#include <iostream>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <unistd.h>	//For read and write functions ie for processes
#include <sys/socket.h>	
#include <sys/types.h>
#include <netinet/in.h>	//For initializing part
#include <netdb.h>

#include <pthread.h>	//Header for POSIX threads

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool syncing = false;
bool autosyncing = false;
bool scheduledsyncing = false;
bool backsyncing = false;


string username = "New User";
string password = "No Password";
string plantype = "Basic";

void getName(){

	cout<<"\tEnter a Username for your new account?"<<endl<<"\n\tUsername : ";
	cin>>username;
	while(username.length()<5){
		cout<<"\tUsername should be greater than 5 symbols"<<endl<<"\n\tUsername : ";;
		cin>>username;
	}


}

void getPassword(){
	cout<<"\n\tLets have a password for your account..."<<endl<<"\n\tPassword : ";;
	cin>>password;
	while(password.length()<5){
		cout<<"\tPassword should be greater than 5 symbols"<<endl<<"\n\tPassword : ";;
		cin>>password;
	}
}

void CurrentStatus(){
	if(syncing == true)
		cout<<"\tiManualSync is active"<<endl;
	if(autosyncing == true) 
		cout<<"\tiAutoSync is active"<<endl;
	if(scheduledsyncing == true)
		cout<<"\tiScheduledSync is active"<<endl;
	if(syncing == false && autosyncing == false && scheduledsyncing == false)
		cout<<"\tNo Syncing is Active"<<endl;
}

void Prompt(int *resp){
	cout<<"---------------------------MAIN--------------------------------"<<endl;
	
	
	cout<<"\tPress 0 : Current Status"<<endl;
	cout<<"\tPress 1 : AUTO iSYNC"<<endl;
	cout<<"\tPress 2 : SCHEDULED iSYNC"<<endl;
	cout<<"\tPress 3 : MANUAL iSYNC"<<endl;
	cout<<"\tPress 4 : Back Sync"<<endl;
	cout<<"\tPress 5 : Settings"<<endl;
	cout<<"\tPress 6 : Delete Permanently"<<endl;
	cout<<"\tPress 9 : To Exit"<<endl<<"\n\tResponse : ";
	cin>>*resp;
}

void Settings(){
	cout<<"---------------------------SETTINGS----------------------------"<<endl;
	cout<<"\tUserName  : "<<username<<endl;
	cout<<"\tPassword  : "<<password<<endl;
	cout<<"\tPlan Type : "<<plantype<<endl;
}

void SyncOut(){
	string command = "rsync -ab iFolder/ ../iServer/" + username + password;	
	system(command.c_str());
}

void Setup(){
	cout<<"---------------------------SETUP------------------------------"<<endl;
	getName();
	getPassword();
	system("mkdir iFolder");
}	

void Login(){
	cout<<"---------------------------SETUP------------------------------"<<endl;
	getName();
	getPassword();
}

void* LetsBackSync(void* parameter){
	pthread_mutex_lock(&mutex);
	backsyncing = true;
	
	string command = "rsync -ab  ../iServer/" + username + password + "/  iFolder/";
	system(command.c_str());
	system("sleep 3");			//Just for testing purpose
	backsyncing = false;
		
	pthread_mutex_unlock(&mutex);
}




void* LetsSync(void* parameter){
	pthread_mutex_lock(&mutex);
	syncing = true;
	
	string command = "rsync -ab iFolder/ ../iServer/" + username + password;
	system(command.c_str());
	system("sleep 3");			//Just for testing purpose
	syncing = false;
		
	pthread_mutex_unlock(&mutex);
}



void iManualSync(){
	cout<<"------------------------MANUAL SYNC---------------------------"<<endl;
	if(syncing == true || autosyncing == true){
		cout<<"\tAlready Syncing..."<<endl;
	}
	else{
		pthread_t Syncerthread;
		cout<<"\tStart Syncing..."<<endl;
		pthread_create(&Syncerthread, NULL, LetsSync, NULL);
	}
}


void* LetsAutoSync(void* parameter){
	
	while(1){
		if(autosyncing == true){
			string command = "rsync -ab iFolder/ ../iServer/" + username + password;
			system(command.c_str());
			system("sleep 3");	//just for testing purpose		
		}else{
			break;
		}
	}	
}

void* LetsScheduledSync(void* parameter){
	
	string* timeinterval = (string *)parameter;

	while(1){
		if(autosyncing == true){
			string command = "rsync -ab iFolder/ ../iServer/" + username + password + "; sleep " + *timeinterval;
			cout<<command<<endl;
			system(command.c_str());
		}else{
			break;
		}
	}	
}


void iScheduledSync(){
	cout<<"----------------------SCHEDULED SYNC--------------------------"<<endl;

		pthread_t Syncerthread;
		int back;

		if(scheduledsyncing == true || autosyncing == true){
			cout<<"\tAlready Syncing..."<<endl;
			cout<<"\tPress 1. to Stop ScheduledSyncing \n\tPress 2. to go back"<<endl<<"\n\tResponse : ";
			cin>>back;
			if(back == 1){
				//kill scheduledsync thread;
				pthread_mutex_lock(&mutex);
				scheduledsyncing = false;
				autosyncing = false;
				pthread_mutex_unlock(&mutex);

			}
			else{

			}
		}
		else{	
			
			string timeinterval;
			cout<<"\tEnter the time interval between two syncings ... in Seconds"<<endl<<"\tResponse : ";
			cin>>timeinterval;
			cout<<"\tStart Syncing..."<<endl;
			pthread_create(&Syncerthread, NULL, LetsScheduledSync, &timeinterval);
			scheduledsyncing = true;
		}

}


void iAutoSync(){
	cout<<"-------------------------AUTO SYNC----------------------------"<<endl;
	
		pthread_t Syncerthread;
		int back;

		if(autosyncing == true || scheduledsyncing == true){
			cout<<"\tAlready Syncing..."<<endl;
			cout<<"\tPress 1. to Stop AutoSyncing \n\tPress 2. to go back"<<endl<<"\n\tResponse : ";
			cin>>back;
			if(back == 1){
				//kill autosync thread;
				pthread_mutex_lock(&mutex);
				scheduledsyncing = false;
				autosyncing = false;
				pthread_mutex_unlock(&mutex);

			}
			else{

			}
		}
		else{	
			
			cout<<"\tStart Syncing..."<<endl;
			pthread_create(&Syncerthread, NULL, LetsAutoSync, NULL);
			autosyncing = true;
		}

}
void getBack(){
	cout<<"----------------------SYNC BACK--------------------------"<<endl;

	pthread_t Syncerthread;
	int back;

	if(backsyncing == true){
		cout<<"\tAlready Back Syncing..."<<endl;
		cout<<"\tPress 1. to Stop Back Syncing \n\tPress 2. to go back"<<endl<<"\n\tResponse : ";
		cin>>back;
		if(back == 1){
			//kill scheduledsync thread;
			pthread_mutex_lock(&mutex);
			backsyncing = false;
			pthread_mutex_unlock(&mutex);

		}
		else{

		}
	}
	else{	
		
		cout<<"\tStart Syncing..."<<endl;
		pthread_create(&Syncerthread, NULL, LetsBackSync, NULL);
		backsyncing = true;
	}

}
int counter = 1;
void show(string dir_name,int i){
//	cout<<dir_name<<endl;
	DIR *dirp = opendir(dir_name.c_str());
	if(dirp == NULL)
		return;
	struct dirent *dp;
	
	while(dirp){
		if((dp = readdir(dirp)) !=NULL){

			if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
			{
				for(int j = 0; j<i; j++){
					cout<<" ";
				}
				cout<<"|"<<counter<<"|";
				counter ++;
				cout<<dp->d_name<<endl;
				//cout<<"|"<<endl;
				show(dir_name + "/" + dp->d_name,i+2);
			}
			
		}
		
		else{
			break;
		}				
	}
}
bool CreateASocket(int *socket_fd){
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(*socket_fd<0){
		cout<<"Error opening socket"<<endl;
		return false;
	}
	return true;
}

bool ConnectAServer(int *socket_fd, struct sockaddr_in *server_addr, struct hostent **server, int *port){
	*server = gethostbyname("localhost");

	if(*server<0){
		cout<<"No Such Host"<<endl;
		return false;
	}

	bzero((char*) server_addr, sizeof(*server_addr));
	(*server_addr).sin_family = AF_INET;
	bcopy((char *) (*server)->h_addr, (char*)&(*server_addr).sin_addr.s_addr,(*server)->h_length);
	(*server_addr).sin_port = htons(*port);

	socklen_t server_size = sizeof(*server_addr);
	//Connecting to the server
	if(connect(*socket_fd,(struct sockaddr *) server_addr, server_size)<0){
		cout<<"Error Connect"<<endl;
		return false;
	}
	return true;
}

void  Delete(){
	show("iFolder/",8);
	counter = 1;
	//if all syncing is false	
		//Call show()
		//Ask Sr.No  of the file to Delete
		//Send the file name/ sr no via a socket

				//Parallely on the server side receive the file name and run the system remove command
	if(autosyncing == false && syncing == false && scheduledsyncing == false && backsyncing == false){
		int socket_fd, port, n;
		struct sockaddr_in server_addr;
		struct hostent *server;
		int theNumber, Number;
		port = 5005;
		char theString[255];
		if(CreateASocket(&socket_fd) == true){
			if(ConnectAServer(&socket_fd, &server_addr, &server, &port) == true){
				cin>>Number;
				int theNumber = htonl(Number);
				n = write(socket_fd, &theNumber, sizeof(theNumber));
				if(n<0){
					cout<<"Error Writing to the socket"<<endl;
				}

				strcpy(theString, username.c_str());
				n = write(socket_fd, (void *)theString, 255);
				if(n<0){
					cout<<"Error Writing to the socket"<<endl;
				}
				strcpy(theString, password.c_str());
				n = write(socket_fd, (void *)theString, 255);
				if(n<0){
					cout<<"Error Writing to the socket"<<endl;
				}

			}
		}
		else{
			cout<<"Unable to fetch Content"<<endl;
		}
	}

}


void ShareWith(){
	//Ask for a user name to share with 
	//send sender's and receiver's username to the server and name of the file
		//Parallely on the server side add to a hidden file list the request(file and senders name) 

}
void ReceiveFrom(){
	//Add senders name to the shared items list and shared file name in the filename list
	//ask for acceptance
	//if response is true then ask for a an rsync with the sharersname and  
}


void SignOrLog(int *utype){
	cout<<"\t1. First Time"<<"\n\t2. Already a User\n\tUserType : ";
	cin>>*utype;

	int resp;
	if(*utype == 1){
		Setup();
		Prompt(&resp);
		while(resp != 9){
			switch(resp){
				case 0: CurrentStatus();    break;
				case 1:	iAutoSync();		break;
				case 2:	iScheduledSync();	break;
				case 3:	iManualSync();		break;
				case 4: getBack();			break;
				case 5: Settings(); 		break;
				case 6: Delete();			break; 
				default: break;
			}
			Prompt(&resp);
		}
	}
	if(*utype == 2){
		Login();	
		Prompt(&resp);
		while(resp != 9){
			switch(resp){
				case 0: CurrentStatus();    break;
				case 1:	iAutoSync();		break;
				case 2:	iScheduledSync();	break;
				case 3:	iManualSync();		break;
				case 4: getBack(); 			break;
				case 5: Settings(); 		break;
				case 6: Delete();			break;
				default: break;
			}
			Prompt(&resp);
		}
	}

}


int main(){
	int  utype;
	cout<<"\n\tWelcome to iDrive. Relax and backUp...\n"<<endl;
	SignOrLog(&utype);

	return 0;
}