#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "store.h"

using namespace std;

store::FSWDescriptor::FSWDescriptor(char* filename){
	        fd=open(filename,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
            if(fd==-1){
                wcout<<"error when open the file: "<<filename<<endl;
            }else{
                isOpen=true;
                position=0;
		        this->filename=filename;
                length=lseek(fd,0L,SEEK_END);
            }
}
store::FSWDescriptor::~FSWDescriptor(){
             int res=close(fd);
             if(res!=0){
				wcout<<"error when close the file"<<endl;
             }
			 delete[] filename;
}
int store::FSWDescriptor::seek(long pos){
             long res=lseek(fd,pos,SEEK_SET);
                if(res==-1){
                     wcout<<"error when seek the file"<<endl;
                 }else{
                          position=pos;
                 }
			return res;
}
int store::FSWDescriptor::writeChars(char* c,int len){
           int wlen=write(fd,(void*)c,len);
		   return wlen;
}
long store::FSWDescriptor::getLength(){
	      length=lseek(fd,0L,SEEK_END);
          return length;
}