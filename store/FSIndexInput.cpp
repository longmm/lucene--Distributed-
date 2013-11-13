#include <iostream>
#include <pthread.h>

#include "store.h"

using namespace std;

store::FSIndexInput::FSIndexInput(char* fsfile):BufferedIndexInput(){
         FSRDescriptor* fsdp=new FSRDescriptor(fsfile);
		 this->fsfile=fsdp;
		 mylock=PTHREAD_MUTEX_INITIALIZER;
}
store::FSIndexInput::FSIndexInput(char* fsfile,int buffersize):BufferedIndexInput(buffersize){
         FSRDescriptor* fsdp=new FSRDescriptor(fsfile);
		 this->fsfile=fsdp;
		 mylock=PTHREAD_MUTEX_INITIALIZER;
}
store::FSIndexInput::~FSIndexInput(){
       delete fsfile;
	   pthread_mutex_destroy(&mylock);
}
long store::FSIndexInput::length(){
        return fsfile->getLength();
}
int store::FSIndexInput::readInternal(char* c, int len){
    int total=0;
	do{
        int i=fsfile->readChars(c+total,len-total);
		if(i==-1)
		{
			wcout<<"error in fsindexinput when read"<<endl;
			break;
		}else if(i==0){
            wcout<<"EOF in  fsindexinput when read"<<endl;
			break;
		}else{
			total+=i;
		}
	}while(total < len);
		return total;
}
long store::FSIndexInput::seekInternal(long pos){
	return fsfile->seek(pos);
}



