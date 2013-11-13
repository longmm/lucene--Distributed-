#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::SegmentTermPositions::SegmentTermPositions(SegmentReader* sr){
     this->sr=sr;
     wstring file_prox=sr->dir+sr->name+IndexFileNames::D+IndexFileNames::PRX;
     char* filename_prox=wstrToChars(file_prox);
     prox_fsii=new FSIndexInput(filename_prox);

     mreader=new MultiLevelSkipListReader(sr->dir,sr->name,MUL_SKIPINTERVAL,MAXSKIPLEVELS);
     treader=new TermInfosReader(sr->dir,sr->name, sr->fis);
     this->poss=new vector<int>();
     cti=NULL;
}
lucene_index::SegmentTermPositions::~SegmentTermPositions(){
     delete prox_fsii;
     delete mreader;
     delete treader;
     delete poss;
     if(cti!=NULL){
        delete cti;
     }
}
void lucene_index::SegmentTermPositions::seek(TermInfo* ti){
     if(cti==NULL){
       cti=ti;
     }else{
       delete cti;
       cti=ti;
     }

     df=ti->docFreq;
     prox_start=ti->proxPointer;
     skip_start=ti->skipPointer;

     prox_fsii->seek(prox_start);
     mreader->reset(skip_start);
     int del_size=sr->dels->size();

     del_df=0;
     for(int i=0;i<del_size;i++){
        int del_docid=sr->dels->at(i);
        bool flag=skipTo(del_docid);

        if(flag&&del_docid==doc){
            del_df++;
        }
     }
     prox_fsii->seek(prox_start);

}
void lucene_index::SegmentTermPositions::resetDocPos(){
     prox_fsii->seek(prox_start);
     pos=-1;
}
bool lucene_index::SegmentTermPositions::seek(Term* t){
     TermInfo* ti=treader->get(t);
     if(ti==NULL){
        return false;
     }else{
        seek(ti);
        pos=-1;

     }

     if(ti->fname==t->field&&ti->fvalue==t->text){
       return true;
     }else{
       return false;
     }
}
bool lucene_index::SegmentTermPositions::skipTo(int target){
     int pos_len=mreader->skipTo(target);
     int docid=mreader->docid;
     if(docid==target){
         pos=pos_len;
         prox_fsii->seek(mreader->proxp);
         int tmp_int=0;
         int tmp_doc=0;
         prox_fsii->readInt(&tmp_doc);
         prox_fsii->readInt(&tmp_int);
         for(int i=0;i<tmp_int;i++){
             int tmp_prox;
             prox_fsii->readInt(&tmp_prox);
             poss->push_back(tmp_prox);
         }
         return true;
     }else{
          long term_p=mreader->proxp;

          prox_fsii->seek(term_p);
          //int len=(term_p-freq_start)/8;

          for(int i=0;(i+pos_len<df)&&(i<MUL_SKIPINTERVAL);i++){
              int tmp_doc;
              int tmp_len;
              prox_fsii->readInt(&tmp_doc);
              prox_fsii->readInt(&tmp_len);

              if(tmp_doc==target){
                  pos=i+pos_len;
                  doc=tmp_doc;
                  poss->clear();
                  for(int x=0;x<tmp_len;x++){
                      int tmp_value;
                      prox_fsii->readInt(&tmp_value);
                      poss->push_back(tmp_value);
                  }
                  return true;
              }else{
                 for(int x=0;x<tmp_len;x++){
                    int tmp_value;
                    prox_fsii->readInt(&tmp_value);
                 }

              }

          }
          return false;
     }
}
bool lucene_index::SegmentTermPositions::nextTerm(){
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
bool lucene_index::SegmentTermPositions::next(){
     if(pos>=df-1){
        return false;
    }
    poss->clear();
    pos++;
    prox_fsii->readInt(&doc);
    int tmp_len;
    prox_fsii->readInt(&tmp_len);

    for(int i=0;i<tmp_len;i++){
        int tmp_doc;
        prox_fsii->readInt(&tmp_doc);
        poss->push_back(tmp_doc);
    }
    return true;
}
bool lucene_index::SegmentTermPositions::init_V_Term(){

     TermInfo* ti=treader->get(0);

     if(ti==NULL){
       return false;
     }

     seek(ti);
     pos=-1;

     return true;
}












