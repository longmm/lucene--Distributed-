#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<dirent.h>
#include <sstream>

#include "lucene_util.h"

using namespace std;

wstring lucene_util::charToWstring(char* c){
   wstringstream hh;
   hh<<c;
   return hh.str();
}
wstring lucene_util::LongToWstring(long c){
   wstringstream hh;
   hh<<c;
   return hh.str();
}
long lucene_util::wstrToLong(wstring& wstr){
   int num;
   wstringstream hh(wstr);
   hh>>num;
   return num;
}
char* lucene_util::wstrToChars(wstring& wstr){
	   int len=wstr.size();
       const wchar_t* wc=wstr.c_str();
       int mbs_length=len*4;
	   char* res_chars=new char[mbs_length+1];
	   memset(res_chars,0,mbs_length+1);
	   wcstombs(res_chars,wc,mbs_length);
      // delete wc;
	   return res_chars;
}
vector<wstring>* lucene_util::getFilesOfDir(char* dir){
       vector<wstring>* tmp=new vector<wstring>();

	   DIR* mydir=opendir(dir);
       struct dirent* ptr;
	   if(mydir==NULL){
          wcout<<"error when open the dir    "<<dir<<endl;
		  return NULL;
	   }
	   while((ptr=readdir(mydir))!=NULL){
            if(ptr->d_type==DT_REG){
				 wstring filename=charToWstring(ptr->d_name);
                 tmp->push_back(filename);
			}
			delete ptr;
	   }
	   closedir(mydir);
	   return tmp;
}