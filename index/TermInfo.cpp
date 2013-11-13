#include <iostream>
#include <string>
#include "index.h"

using namespace std;

lucene_index::TermInfo::TermInfo(){
       wstring tmp(L"");
       fname=tmp;
       fvalue=tmp;

	   docFreq=0;
       freqPointer=0;
	   proxPointer=0;
	   skipPointer=0;
}
lucene_index::TermInfo::~TermInfo(){
}
lucene_index::TermInfo::TermInfo(int df,long fp,long pp,long skipp,wstring fname,wstring fvalue){
       docFreq=df;
	   freqPointer=fp;
	   proxPointer=pp;
	   skipPointer=skipp;
	   this->fname=fname;
	   this->fvalue=fvalue;
}
lucene_index::TermInfo::TermInfo(TermInfo& t){
       this->docFreq=t.docFreq;
	   this->freqPointer=t.freqPointer;
	   this->proxPointer=t.proxPointer;
	   this->skipPointer=t.skipPointer;
	   this->fname=t.fname;
	   this->fvalue=t.fvalue;
}
void lucene_index::TermInfo::setInfo(int df,long fp,long pp,long skipp,wstring fname,wstring fvalue){
       docFreq=df;
	   freqPointer=fp;
	   proxPointer=pp;
	   skipPointer=skipp;
	   this->fname=fname;
	   this->fvalue=fvalue;
}
void lucene_index::TermInfo::setInfo(TermInfo& t){
       this->docFreq=t.docFreq;
	   this->freqPointer=t.freqPointer;
	   this->proxPointer=t.proxPointer;
	   this->skipPointer=t.skipPointer;
	   this->fname=t.fname;
	   this->fvalue=t.fvalue;
}
void lucene_index::TermInfo::operator=(const TermInfo& ti){
      this->docFreq=ti.docFreq;
	   this->freqPointer=ti.freqPointer;
	   this->proxPointer=ti.proxPointer;
	   this->skipPointer=ti.skipPointer;
	   this->fname=ti.fname;
	   this->fvalue=ti.fvalue;
}
 bool lucene_index::TermInfo::operator<(const TermInfo& ti){
       if(this->fname==ti.fname){
            return this->fvalue<ti.fvalue;
       }else{
            return this->fname<ti.fname;
       }
 }
 bool lucene_index::TermInfo::operator==(const TermInfo& ti){
       if(this->fname==ti.fname){
           return this->fvalue==ti.fvalue;
       }else{
           return false;
       }
 }
