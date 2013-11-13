#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::TermInfosWriter::TermInfosWriter(wstring dir,wstring name,FieldInfos* fis,int skipinterval){
     wstring file_tii=dir+name+IndexFileNames::D+IndexFileNames::TII;
     char* filename_tii=wstrToChars(file_tii);
     this->tii=new FSIndexOutput(filename_tii);

     wstring file_tis=dir+name+IndexFileNames::D+IndexFileNames::TIS;
     char* filename_tis=wstrToChars(file_tis);
     this->tis=new FSIndexOutput(filename_tis);

     this->fis=fis;
     this->skipinterval=skipinterval;
     this->termCount=0;
     init();
}
lucene_index::TermInfosWriter::~TermInfosWriter(){
     tis->seek(0);
     tis->writeInt(termCount);
     delete tii;
     delete tis;
}
void lucene_index::TermInfosWriter::init(){
     tis->writeInt(-1);
     tis->writeInt(skipinterval);
}

void lucene_index::TermInfosWriter::add(Term* t,TermInfo* tf){

    long tis_p=tis->getFilePointer();
    writeTerm(t);
    tis->writeInt(tf->docFreq);
    tis->writeLong(tf->freqPointer);
    tis->writeLong(tf->proxPointer);
    tis->writeLong(tf->skipPointer);
    //termCount++;
    if(termCount%skipinterval==0){
        tii->writeLong(tis_p);
    }
    termCount++;
}
void lucene_index::TermInfosWriter::writeTerm(Term* t){
    wstring tvalue=t->text;
    int fnum=fis->fieldNum(t->field);
    tis->writeWString(tvalue);
    tis->writeInt(fnum);
}

