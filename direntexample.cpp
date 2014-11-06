#include <iostream>
#include <dirent.h>
#include <cstring>
using namespace std;

int listdir(const char *path){
	struct dirent *streamElement;
	DIR* directoryStreamPointer;
	directoryStreamPointer = opendir(path);
	if(directoryStreamPointer == NULL){
		cout<<"Error in opendir\n";
		return -1;
	}

	while(streamElement = readdir(directoryStreamPointer)){
		if(strcmp(streamElement->d_name,".") != 0 && strcmp(streamElement->d_name,"..") != 0)
			cout<<streamElement->d_name<<endl;
	}
	closedir(directoryStreamPointer);
	return(0);
}

int main()
{
	const char *path;
	path = "iClient/iFolder";
	listdir(path);
	return 0;
}