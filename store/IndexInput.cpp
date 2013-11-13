#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "store.h"
#include "lucene_util.h"

using namespace std;
using namespace lucene_util;

bool store::IndexInput::skipChars(int length){
         for(int i=0;i<length;i++){
              char c;
			  readChar(&c);
			  if ((c & 0x80) == 0){
                  //do nothing, we only need one byte
              }
			  else if ((c & 0xE0) != 0xE0) {
                  readChar(&c);//read an additional byte
              } else{
                  //read two additional bytes.
                  readChar(&c);
                  readChar(&c);
              }
			  }//for
			  return true;
}
int store::IndexInput::readWchars(wchar_t* c,int len){
	       int tmp_len=len*4;
           char tmp[tmp_len+1];
		   memset(tmp,0,tmp_len+1);
		   int res=readChars(tmp,tmp_len);
           mbstowcs(c,tmp,res/4);
		   return res/4;
}
int store::IndexInput::readWString(wstring* wc){
        int len;
		int res=this->readInt(&len);

		if(res==0){
            return 0;
		}

		wchar_t buff[len+1];
		initwchars(buff,(wchar_t)0,len+1);
		this->readWchars(buff,len);
		wstring name(buff);
		(*wc)=name;
        return len;
}
int store::IndexInput::readInt(int* i){
      char a;
	  char b;
	  char c;
	  char d;

	   int res;
	   res=readChar(&a);
	   if(res==0){
           return 0;
	   }
	   res=readChar(&b);
       if(res==0){
           return 0;
	   }
	   res=readChar(&c);
	   if(res==0){
           return 0;
	   }
	   res=readChar(&d);
	   if(res==0){
           return 0;
	   }

      *i=((a & 0xFF) << 24) | ((b & 0xFF) << 16)
         | ((c & 0xFF) <<  8) |  (d & 0xFF);

	  return 1;
}
int store::IndexInput::readFloat(float* f){
      char a;
	  char b;
	  char c;
	  char d;

	   int res;
	   res=readChar(&a);
	   if(res==0){
           return 0;
	   }
	   res=readChar(&b);
       if(res==0){
           return 0;
	   }
	   res=readChar(&c);
	   if(res==0){
           return 0;
	   }
	   res=readChar(&d);
	   if(res==0){
           return 0;
	   }

	   F2I f2i;

	   f2i.cs[0]=a;
	   f2i.cs[1]=b;
	   f2i.cs[2]=c;
	   f2i.cs[3]=d;
	   *f=f2i.f;
	   return 1;
}
int store::IndexInput::readLong(long* l){
      return readInt((int*)l);
}
