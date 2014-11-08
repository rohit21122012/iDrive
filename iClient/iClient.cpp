#include <iostream>
#include <cstdlib>
#include <cstring>


#include <pthread.h>	//Header for POSIX threads

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool syncing = false;
bool autosyncing = false;
bool scheduledsyncing = false;

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
	cout<<"\tPress 4 : To Exit"<<endl;
	cout<<"\tPress 5 : Settings"<<endl<<"\n\tResponse : ";
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

void SignOrLog(int *utype){
	cout<<"\t1. First Time"<<"\n\t2. Already a User\n\tUserType : ";
	cin>>*utype;

	int resp;
	if(*utype == 1){
		Setup();
		Prompt(&resp);
		while(resp != 4){
			switch(resp){
				case 0: CurrentStatus();    break;
				case 1:	iAutoSync();		break;
				case 2:	iScheduledSync();	break;
				case 3:	iManualSync();		break;
				case 5: Settings(); 		break;
				default: break;
			}
			Prompt(&resp);
		}
	}
	if(*utype == 2){
		Login();	
		Prompt(&resp);
		while(resp != 4){
			switch(resp){
				case 0: CurrentStatus();    break;
				case 1:	iAutoSync();		break;
				case 2:	iScheduledSync();	break;
				case 3:	iManualSync();		break;
				case 5: Settings(); 		break;
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