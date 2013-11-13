#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "store.h"

using namespace std;

store::FSRDescriptor::FSRDescriptor(char* filename){
		    fd=open(filename,O_RDONLY);
            if(fd==-1){
                wcout<<"error when open the file: "<<filename<<endl;
            }else{
                isOpen=true;
                position=0;
		        this->filename=filename;
                length=lseek(fd,0L,SEEK_END);
				lseek(fd,0L,SEEK_SET);
            }
}
store::FSRDescriptor::~FSRDescriptor(){
     int res=close(fd);
     if(res!=0){
		wcout<<"error when close the file"<<endl;
      }
	  delete[] filename;
}
long store::FSRDescriptor::seek(long pos){
      long res=lseek(fd,pos,SEEK_SET);
      if(res==-1){
        wcout<<"error when seek the file"<<filename<<pos<<endl;
      }else{
         position=pos;
       }
	   return res;
}
int store::FSRDescriptor::readChars(char* c,int len){
              int count=read(fd,(void*)c,len);
			  position+=count;
			  return count;
}
long store::FSRDescriptor::getLength(){

   return length;
}
