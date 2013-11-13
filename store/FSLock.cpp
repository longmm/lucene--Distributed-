#include <iostream>

#include <string>

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <stdlib.h>

#include "store.h"

using namespace std;

store::FSLock::FSLock(wstring lockfile,wstring lockdir){
     this->lockfile=lockfile;
	 this->lockdir=lockdir;
	 wstring tmp=(lockdir)+(lockfile); //tmp 会在构造函数后自动析构
	 
	 int len=tmp.size()*4+1;
	 filename=new char[len];
	 memset(filename,0,len);
	 const wchar_t* res_chars=tmp.c_str();
	 wcstombs(filename,res_chars,len);
	 isLock=false;
}
store::FSLock::~FSLock(){
	delete[] filename;
}
bool store::FSLock::obtain(){
     int res=access(filename,F_OK);
	 if(res==0){
        wcout<<"obtain lock fail"<<endl;
		return false;
	 }

	 res=open(filename,O_CREAT|O_RDWR,S_IWUSR|S_IRUSR);

	 if(res==-1){
          wcout<<"error when creat lock file"<<endl;
		  return false;
	 }
	 close(res);
	 isLock=true;
	 return true;
}
bool store::FSLock::isLocked(){
      return this->isLock;
}
bool store::FSLock::release(){
      if(isLock==false){
         return false; 
	  }
	  int res=remove(filename);
	  if(res==0){
         return true;
	  }else{
          wcout<<"error when remove the lockfile"<<endl;
		  return false;
	  }
}