#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_search;
using namespace store;
using namespace lucene_util;
using namespace document;

lucene_index::MTermDocs::MTermDocs(vector<SegmentReader*>* srs){
     this->srs=srs;
     int s_size=srs->size();
     mtermdocs=new vector<SegmentTermDocs*>();
     int total=0;
     starts=new int[s_size];
     this->delCount=0;
     this->docCount=0;

     for(int i=0;i<s_size;i++){
         SegmentReader* sr=srs->at(i);
         SegmentTermDocs* stds=new SegmentTermDocs(sr);
         stds->init_V_Term();
         mtermdocs->push_back(stds);
         starts[i]=total;
         total+=sr->docCount;
         docCount+=sr->docCount;
         delCount+=sr->delCount;
     }

     isCur=new int[s_size];
     for(int i=0;i<s_size;i++){
         isCur[i]=0;
     }
}
lucene_index::MTermDocs::~MTermDocs(){
    int tmp_size=mtermdocs->size();
    for(int i=0;i<tmp_size;i++){
       SegmentTermDocs* stds=mtermdocs->at(i);
       delete stds;
    }
    delete mtermdocs;
    delete[] isCur;
    delete[] starts;
}
bool lucene_index::MTermDocs::seek(Term* t){
      this->t=t;
      int tmp_size=mtermdocs->size();
      bool total_flag=false;

      for(int i=0;i<tmp_size;i++){
          SegmentTermDocs* stds=mtermdocs->at(i);
          bool flag=stds->seek(t);
          if(flag==true){
             isCur[i]=0;
             total_flag=true;
          }else{
             isCur[i]=1;
          }
       }
      if(total_flag==true){
         Comput_df();
         pos=0;
         return true;
      }
}
int lucene_index::MTermDocs::readerIndex(int index){
    int rsize=mtermdocs->size();
    for(int i=0;i<rsize;i++){
        if(i+1<rsize&&index>=starts[i]&&index<starts[i+1]){
             return i;
		}else if(i+1==rsize&&index<docCount){
             return i;
		}else{
            continue;
		}
	}
	return -1;
}
bool lucene_index::MTermDocs::next(){
     int tmp_size=mtermdocs->size();
     /*
     if(pos>=tmp_size){
        return false;
     }

     SegmentReader* sr=srs->at(pos);

     bool flag=sr->termdocs->next();
     if(flag==true){
        sr->termposs->next();
        docid=sr->termdocs->doc+starts[pos];
        freq=sr->termdocs->freq;
        poss=sr->termposs->poss;
        return true;
     }
     */

     //pos++;
     while(pos<tmp_size){
         if(isCur[pos]==0){
            SegmentTermDocs* tmp_stds=mtermdocs->at(pos);
            bool tmp_flag=tmp_stds->next();

            if(tmp_flag==true){
               tmp_stds->next();
               docid=tmp_stds->doc+starts[pos];
               freq=tmp_stds->freq;
               return true;
            }else{
               pos++;
            }
         }else{
            pos++;
         }
     }
     return false;
}
bool lucene_index::MTermDocs::skipTo(int target){

     if(target<0||target>docCount){
        return false;
     }

     int s_size=mtermdocs->size();

     int posi=readerIndex(target);

     if(posi==-1){
          return false;
     }

     if(isCur[posi]==1){
         if(posi==s_size-1){
              return false;
         }else{
             pos=posi;
             bool tmp_f=next();
             return tmp_f;
         }
     }

     SegmentTermDocs* tmp_termdocs=mtermdocs->at(posi);

     bool flag=tmp_termdocs->skipTo(target-starts[posi]);

     if(flag==true){
        pos=posi;
        docid=tmp_termdocs->doc+starts[pos];
        freq=tmp_termdocs->freq;
        return true;
     }else{
         pos=posi;
         bool tmp_f=next();
         return tmp_f;
     }
}
void lucene_index::MTermDocs::resetDocPos(){
    int tmp_size=mtermdocs->size();

     for(int i=0;i<tmp_size;i++){
         SegmentTermDocs* stds=mtermdocs->at(i);
         stds->resetDocPos();
     }
}
void lucene_index::MTermDocs::Comput_df(){
         this->df=0;
         this->del_df=0;

         int tmp_size=mtermdocs->size();
         for(int i=0;i<tmp_size;i++){
             if(isCur[i]==0){
                 SegmentTermDocs* sterms=mtermdocs->at(i);
                 this->df+=sterms->df;
                 this->del_df+=sterms->del_df;
             }
         }
}
int lucene_index::MTermDocs::getDocID(){
     return docid;
}
int lucene_index::MTermDocs::getFreq(){
     return freq;
}
float lucene_index::MTermDocs::getNorm(int docid){

     int posi=readerIndex(docid);

     if(posi==-1){
          return 0.0f;
     }

     wstring tmp_f=t->field;

     SegmentReader* tmp_sr=srs->at(posi);

     float* tmp_norms=tmp_sr->getnorms(tmp_f);

     if(tmp_norms==NULL){
         return 0.0f;
     }

     return tmp_norms[docid-starts[posi] ];
}
int lucene_index::MTermDocs::getDF(){
    return this->df-this->del_df;
}
