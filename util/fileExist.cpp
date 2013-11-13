#include <iostream>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include "lucene_util.h"
using namespace std;

bool lucene_util::fileExist(wstring file){
      if(file.size()==0){
          return false;
	  }else{
            int len=file.size();
			const wchar_t* wc=file.c_str();
			int tmp_len=len*4;
            char tmp[tmp_len+1];
		    memset(tmp,0,tmp_len+1);
            wcstombs(tmp,wc,tmp_len);
			//delete wc;
            int res=access(tmp,F_OK);
            if(res==0){
                 return true;
			}else{
                 return false;
			}
	  }
}