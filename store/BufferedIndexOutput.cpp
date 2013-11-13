#include <iostream>
#include <string>
#include <string.h>
#include "store.h"


using namespace std;

bool store::BufferedIndexOutput::close(){
     flush();
     return true;
}
bool store::BufferedIndexOutput::flush(){
	flushBuffer(buffer, bufferPosition);
    bufferStart += bufferPosition;
    bufferPosition = 0;
    return true;
}
long store::BufferedIndexOutput::getFilePointer(){
      return bufferStart + bufferPosition;
}
int store::BufferedIndexOutput::writeChar(char c){
      if (bufferPosition >= BUFFER_SIZE)
      flush();
    buffer[bufferPosition++] = c;
	return 1;
}
int store::BufferedIndexOutput::writeChars(char* c,int len){
      int bytesLeft = BUFFER_SIZE - bufferPosition;
	  if (bytesLeft >= len) {
			strncpy(buffer+bufferPosition,c,len);
			bufferPosition += len;
      if (BUFFER_SIZE - bufferPosition == 0){
            flush();
	  }
    } else if (len > BUFFER_SIZE) {
        if(bufferPosition > 0)
            flush();
        flushBuffer(c,len);
        bufferStart += len;
      }else{
         if(bufferPosition > 0)
		    flush();
            strncpy(buffer,c,len);
            bufferPosition += len;		  
	  }
	  return len;
}
store::BufferedIndexOutput::BufferedIndexOutput(int buffersize){
               this->BUFFER_SIZE=buffersize;
               buffer=new char[BUFFER_SIZE];
			   memset(buffer,0,BUFFER_SIZE);
			   init();
}
store::BufferedIndexOutput::BufferedIndexOutput(){
	   this->BUFFER_SIZE=DEF_BUF_SIZE;
       buffer=new char[BUFFER_SIZE];
	   memset(buffer,0,BUFFER_SIZE);
	   init();
}
void store::BufferedIndexOutput::init(){
        bufferStart=0;
		bufferPosition=0;
}
store::BufferedIndexOutput::~BufferedIndexOutput(){
		       delete[] buffer;
}
long store::BufferedIndexOutput::seek(long pos){
	flush();
	return seekInternal(pos);
}