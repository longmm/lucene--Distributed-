#include <iostream>
#include <string>
#include "lucene_util.h"

using namespace std;

void lucene_util::initwchars(wchar_t* buff,wchar_t t,int len){
     for(int i=0;i<len;i++){
        buff[i]=t;
	 }
	 return;
}
