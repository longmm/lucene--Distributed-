#include <iostream>
#include <stdlib.h>
#include <string>
#include<string.h>
#include "store.h"

using namespace std;

int store::IndexOutput::writeWchars(wchar_t* wc,int len){
                  int mbs_length=len*4;
				  char res_chars[mbs_length+1];
				  memset(res_chars,0,mbs_length+1);
				  wcstombs(res_chars,wc,mbs_length);
				  int res=writeChars(res_chars,mbs_length);
                  return len;
}
int store::IndexOutput::writeInt(int i){
			    writeChar((char)(i >> 24));
				writeChar((char)(i >> 16));
				writeChar((char)(i >>  8));
				writeChar((char) i);
				return 1;
}
int store::IndexOutput::writeFloat(float f){
     F2I f2i;
     f2i.f=f;

     writeChar(f2i.cs[0]);
     writeChar(f2i.cs[1]);
     writeChar(f2i.cs[2]);
     writeChar(f2i.cs[3]);

     return 1;
}
int store::IndexOutput::writeLong(long i){
               writeInt((int) i);
			   return 1;
}
void store::IndexOutput::writeWString(wstring wc){
           int len=wc.size();
           this->writeInt(len);
		   wchar_t* buff=(wchar_t*)wc.c_str();
		   writeWchars(buff,len);
}
