#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::SegmentTermDocs::SegmentTermDocs(SegmentReader* sr){
    this->sr=sr;
    wstring file=sr->dir+sr->name+IndexFileNames::D+IndexFileNames::FRQ;
    char* filename=wstrToChars(file);
    freq_fsii=new FSIndexInput(filename);
    mreader=new MultiLevelSkipListReader(sr->dir,sr->name,MUL_SKIPINTERVAL,MAXSKIPLEVELS);
    treader=new TermInfosReader(sr->dir,sr->name, sr->fis);
    cti=NULL;
}
lucene_index::SegmentTermDocs::~SegmentTermDocs(){
    delete freq_fsii;
    delete mreader;
    delete treader;
    if(cti!=NULL){
     delete cti;
    }
}
bool lucene_index::SegmentTermDocs::seek(Term* t){
     TermInfo* ti=treader->get(t);
     if(ti==NULL){
        return false;
     }else {
        seek(ti);
        pos=-1;
       // return true;
     }

     if(ti->fname==t->field&&ti->fvalue==t->text){
       return true;
     }else{
       return false;
     }
}
void lucene_index::SegmentTermDocs::resetDocPos(){
     freq_fsii->seek(freq_start);
     pos=-1;
}
void lucene_index::SegmentTermDocs::seek(TermInfo* ti){
     if(cti==NULL){
       cti=ti;
     }else{
       delete cti;
       cti=ti;
     }

     df=ti->docFreq;
     freq_start=ti->freqPointer;
     skip_start=ti->skipPointer;

     freq_fsii->seek(freq_start);
     mreader->reset(skip_start);
     int del_size=sr->dels->size();

     del_df=0;
     for(int i=0;i<del_size;i++){
        int del_docid=sr->dels->at(i);
        bool flag=skipTo(del_docid);

        if(flag==true&&del_docid==doc){
            del_df++;
        }
     }
     freq_fsii->seek(freq_start);
}
bool lucene_index::SegmentTermDocs::skipTo(int target){
     mreader->skipTo(target);
     int docid=mreader->docid;
     if(docid==target){
         pos=(mreader->freqp-freq_start)/8;
         freq_fsii->seek(mreader->freqp);
         freq_fsii->readInt(&doc);
         freq_fsii->readInt(&freq);
         return true;
     }else{
          long term_p=mreader->freqp;

          freq_fsii->seek(term_p);
          int len=(term_p-freq_start)/8;

          for(int i=0;(i+len<df)&&(i<MUL_SKIPINTERVAL+1);i++){
              int tmp_doc;
              int tmp_freq;
              freq_fsii->readInt(&tmp_doc);
              freq_fsii->readInt(&tmp_freq);

              pos=(freq_fsii->getFilePointer()-freq_start)/8-1;
              doc=tmp_doc;
              freq=tmp_freq;

              if(tmp_doc>=target){
                  return true;
              }

          }
          return false;
     }

}
int lucene_index::SegmentTermDocs::read(int* docs,int*freqs,int len){
      freq_fsii->seek(freq_start);
      int i=0;
      for(i=0;i<len&&i<df;i++){
          int tmp_docid;
          int tmp_freq;

          freq_fsii->readInt(&tmp_docid);
          freq_fsii->readInt(&tmp_freq);

           docs[i]=tmp_docid;
           freqs[i]=tmp_freq;
      }
       return i;
}
bool lucene_index::SegmentTermDocs::nextTerm(){
     TermInfo* ti=treader->next();
     if(ti==NULL){
        pos=-1;
        return false;
     }else{
         seek(ti);
         pos=-1;
         return true;
     }
}
bool lucene_index::SegmentTermDocs::next(){
    if(pos>=df-1){
        return false;
    }
    pos++;
    freq_fsii->readInt(&doc);
    freq_fsii->readInt(&freq);
    return true;
}
bool lucene_index::SegmentTermDocs::init_V_Term(){

    TermInfo* ti=treader->get(0);
    if(ti==NULL){
       return false;
    }
    seek(ti);
    pos=-1;

    return true;
}














