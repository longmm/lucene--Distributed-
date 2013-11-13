#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::TermInfosReader::TermInfosReader(wstring dir,wstring segment, FieldInfos* fis){
    wstring file_tii=dir+segment+IndexFileNames::D+IndexFileNames::TII;
    char* filename_tii=wstrToChars(file_tii);
    this->fsii_tii=new FSIndexInput(filename_tii);

    wstring file_tis=dir+segment+IndexFileNames::D+IndexFileNames::TIS;
    char* filename_tis=wstrToChars(file_tis);
    this->fsii_tis=new FSIndexInput(filename_tis);

    this->fis=fis;
    init();
}
lucene_index::TermInfosReader::~TermInfosReader(){
    //delete fis;
    delete fsii_tii;
    delete fsii_tis;
}
TermInfo* lucene_index::TermInfosReader::get(int position){
     if(position>=size){
        return NULL;
     }
     this->pos=position;
     int tmp_pos=position/skipInterval;
     int res=position%skipInterval;
     fsii_tii->seek(tmp_pos);

     int tmpp=0;
     fsii_tii->readInt(&tmpp);

     fsii_tis->seek(tmpp);

     for(int i=0;i<res;i++){
         wstring tmpwstr;
         int fnum;

         fsii_tis->readWString(&tmpwstr);
         fsii_tis->readInt(&fnum);

         int tmpInt;
         long tmpLong;

         fsii_tis->readInt(&tmpInt);
         fsii_tis->readLong(&tmpLong);
         fsii_tis->readLong(&tmpLong);
         fsii_tis->readLong(&tmpLong);
     }

     wstring fname;
     wstring fvalue;
     int fnum;
     int docFreq;
     long freqPointer;
     long proxPointer;
     long skipPointer;

     fsii_tis->readWString(&fvalue);
     fsii_tis->readInt(&fnum);
     fname=fis->fieldName(fnum);
     fsii_tis->readInt(&docFreq);
     fsii_tis->readLong(&freqPointer);
     fsii_tis->readLong(&proxPointer);
     fsii_tis->readLong(&skipPointer);

     TermInfo* ti=new TermInfo(docFreq,freqPointer,proxPointer,skipPointer,fname,fvalue);
     return ti;
}
TermInfo* lucene_index::TermInfosReader::get(Term* t){
    if(t==NULL){
         return NULL;
    }
    //TermInfo* lastti;
    //long last_tp;

    fsii_tis->seek(8);

    this->pos=0;

    while(pos<size){
          //last_tp=fsii_tis->getFilePointer();

          wstring fname;
          wstring fvalue;
          int fnum;
          int docFreq;
          long freqPointer;
          long proxPointer;
          long skipPointer;

          int res=fsii_tis->readWString(&fvalue);

          fsii_tis->readInt(&fnum);
          fsii_tis->readInt(&docFreq);
          fsii_tis->readLong(&freqPointer);
          fsii_tis->readLong(&proxPointer);
          fsii_tis->readLong(&skipPointer);
          fname=fis->fieldName(fnum);

          Term tmp_t(fname,fvalue);

          if(tmp_t==(*t)){
            TermInfo* ti=new TermInfo(docFreq,freqPointer,proxPointer,skipPointer,fname,fvalue);
            return ti;
          }else if(tmp_t<(*t)){
            if(pos==size-1){
               TermInfo* ti=new TermInfo(docFreq,freqPointer,proxPointer,skipPointer,fname,fvalue);
               return ti;
            }
            pos++;
          }else if((*t)<(tmp_t)){
            TermInfo* ti=new TermInfo(docFreq,freqPointer,proxPointer,skipPointer,fname,fvalue);
            return ti;
          }
    }
    return NULL;
}
TermInfo* lucene_index::TermInfosReader::next(){
       if(pos+1>=size){
         return NULL;
       }
       pos++;

       wstring fname;
       wstring fvalue;
       int fnum;
       int docFreq;
       long freqPointer;
       long proxPointer;
       long skipPointer;

       int res=fsii_tis->readWString(&fvalue);
          if(res==0){
             return NULL;
          }

          fsii_tis->readInt(&fnum);
          fsii_tis->readInt(&docFreq);
          fsii_tis->readLong(&freqPointer);
          fsii_tis->readLong(&proxPointer);
          fsii_tis->readLong(&skipPointer);
          fname=fis->fieldName(fnum);

       TermInfo* ti=new TermInfo(docFreq,freqPointer,proxPointer,skipPointer,fname,fvalue);
       return ti;
}
int lucene_index::TermInfosReader::getSize(){
  return size;
}
void lucene_index::TermInfosReader::init(){
    fsii_tis->readInt(&size);
    fsii_tis->readInt(&skipInterval);
    pos=-1;
}
