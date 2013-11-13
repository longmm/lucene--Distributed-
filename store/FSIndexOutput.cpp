#include <iostream>
#include <pthread.h>
#include "store.h"

store::FSIndexOutput::FSIndexOutput(char* filepath):BufferedIndexOutput(){
    fswfile=new FSWDescriptor(filepath);
}
store::FSIndexOutput::FSIndexOutput(char* filepath,int bufsize):BufferedIndexOutput(bufsize){
    fswfile=new FSWDescriptor(filepath);
}
store::FSIndexOutput::~FSIndexOutput(){
	BufferedIndexOutput::flush();
    delete fswfile;
}
long store::FSIndexOutput::length(){
	BufferedIndexOutput::flush();
    return fswfile->getLength();
}
long store::FSIndexOutput::seekInternal(long pos){
     return fswfile->seek(pos);
}
int store::FSIndexOutput::flushBuffer(char* c, int len){
   return fswfile->writeChars(c,len);
}
