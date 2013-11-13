#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "store.h"

using namespace std;

 int store::BufferedIndexInput::readChar(char* c){
      if((this->bufferPosition)>=(this->bufferLength)){
           int res=refill();
		   if(res==0){
                return 0;
		   }
	  }
	  (*c)=buffer[bufferPosition++];
	  return 1;
 }
 int store::BufferedIndexInput::readChars(char* c,int len){
	    int restotal=len;
        if(len<=(bufferLength-bufferPosition)){
		      strncpy(c,buffer+bufferPosition,len);
              bufferPosition+=len;
			  return len;
		}else{
              int available=bufferLength-bufferPosition;
			  if(available>0){
			       strncpy(c,buffer+bufferPosition,available);
			       c=c+available;
				   len-=available;
				   bufferPosition+=available;
			  }//if

              do{

				int refill_len=refill();
                if(refill_len==0){
                    return restotal-len;
				}

				if(bufferLength>=len){
					strncpy(c,buffer,len);
					bufferPosition=len;
				    return restotal;  
				}else if(bufferLength==bufferSize){
				    strncpy(c,buffer,bufferLength);
                    c+=bufferLength;
					len-=bufferLength;
				}else{
                    strncpy(c,buffer,bufferLength);
                    bufferPosition=bufferLength;
					len-=bufferLength;
					wcout<<"EOF in read in BufferedIndexInput"<<endl;
                    return restotal-len;
				}
              }while(true);
		}//else
 }
 int store::BufferedIndexInput::refill(){
         long start=bufferStart+bufferPosition;
		 long end=start+bufferSize;
		 if(end>length())
			 end=length();
		 bufferLength=(int)(end-start);

          if(bufferLength<=0){
		        return 0;
		  }
		  readInternal(buffer,bufferLength);
		  bufferStart=start;
		  bufferPosition=0;
		  return bufferLength;
 }
 long store::BufferedIndexInput::seek(long pos){
        if (pos >= bufferStart && pos < (bufferStart + bufferLength)){
				bufferPosition = (int)(pos - bufferStart);  // seek within buffer
				return pos;
		}else {
				bufferStart = pos;
				bufferPosition = 0;
				bufferLength = 0;				  // trigger refill() on read()
				long res=seekInternal(pos);
				return res;
		}
 }
/*
 bool store::BufferedIndexInput::setBufferSize(int newSize){
         if(newSize!=bufferSize){
             bufferSize=newSize;
			 if(buffer!=NULL){
                 char* newBuffer=new char[newSize];
				 int leftInBuffer=bufferLength-bufferPosition;
				 int numToCopy;
				 if(leftInBuffer>newSize)
					 numToCopy=newSize;
                 else
                     numToCopy=leftInBuffer;

				 strncpy(newBuffer,buffer+bufferPosition,numToCopy);
                 delete buffer;
				 bufferStart += bufferPosition;
                 bufferPosition = 0;
                 bufferLength = numToCopy;
                 buffer = newBuffer;
			 }
			 return true;
		 }else{
             return true;
		 }
 }
 */
store::BufferedIndexInput::BufferedIndexInput(int bufferSize){
	           bufferStart=0;
		       bufferLength=0;
		       bufferPosition=0;
			   this->bufferSize=bufferSize;
               this->BUFFER_SIZE=bufferSize;
			   this->buffer=new char[BUFFER_SIZE];
}
store::BufferedIndexInput::BufferedIndexInput(){
            bufferStart=0;
		    bufferLength=0;
		    bufferPosition=0;
			this->bufferSize=DEF_BUF_SIZE;
            this->BUFFER_SIZE=DEF_BUF_SIZE;
			this->buffer=new char[BUFFER_SIZE];
}

store::BufferedIndexInput::~BufferedIndexInput(){
           delete[] buffer;
}
int store::BufferedIndexInput::getBufferSize(){
		   return this->BUFFER_SIZE;
}
long store::BufferedIndexInput::getFilePointer(){
              return bufferStart+bufferPosition;
}