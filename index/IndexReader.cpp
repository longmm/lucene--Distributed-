#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;

lucene_index::IndexReader::IndexReader(wstring dir){
      this->dir=dir;
      this->sis=new SegmentInfos(dir);

      this->srs=new vector<SegmentReader*>();
	  init();
	  init_V_Term();
      IndexFileDeleter::incRef(sis);
}
lucene_index::IndexReader::~IndexReader(){
      int ssize=srs->size();
      for(int i=0;i<ssize;i++){
          SegmentReader* sr=srs->at(i);
          delete sr;
      }
      IndexFileDeleter::decRef(sis);
      delete srs;
      delete sis;
      delete[] isCur;
      delete[] starts;
      if(curT!=NULL){
         delete curT;
      }
}
void lucene_index::IndexReader::init(){
     curT=NULL;
     int ssize=sis->size();

     if(ssize==0){
      return;
     }

     for(int i=0;i<ssize;i++){
         SegmentInfo* si=sis->info(i);
         wstring name=si->name;
         int docNum=si->docCount;
         SegmentReader* sr=new SegmentReader(name,dir,docNum);
         srs->push_back(sr);
     }

     starts=new int[ssize];
     isCur=new int[ssize];

     docCount=0;
     delCount=0;

	 for(int i=0;i<ssize;i++){
         SegmentReader* sr=srs->at(i);
		 starts[i]=docCount;
		 isCur[i]=0;
         docCount+=sr->docCount;
         delCount+=sr->delCount;
     }

}
int lucene_index::IndexReader::readerIndex(int index){
    int rsize=sis->size();
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
Document* lucene_index::IndexReader::getDoc(int index){
    if(index>=docCount){
        return NULL;
	}
	int i=readerIndex(index);
	if(i==-1){
	    return NULL;
	}
	SegmentReader* sr=srs->at(i);
	Document* doc=sr->getDoc(index-starts[i]);
    return doc;
}
bool lucene_index::IndexReader::isDeleted(int index){
    if(index>=docCount){
       return false;
	}
    int i=readerIndex(index);
    if(i==-1){
        return false;
    }

	SegmentReader* sr=srs->at(i);
	bool isdeleted= sr->isDeleted(index-starts[i]);
	return isdeleted;
}
int lucene_index::IndexReader::read(int* docs,int*freqs,int len){
    int tmp_size=srs->size();

    int total_len=0;

    for(int i=0;i<tmp_size;i++){

        if(isCur[i]==1 || isCur[i]==2){
           continue;
        }

        SegmentReader* sr=srs->at(i);
        int tmp_df=sr->termdocs->df;
        int base=starts[i];

        //total_len+=emp_df;

        if(total_len+tmp_df<len){
           sr->termdocs->read(docs+total_len,freqs+total_len,tmp_df);
           for(int x=total_len;x<total_len+tmp_df;x++){
              docs[x]+=base;
           }
           total_len+=tmp_df;
        }else{
           sr->termdocs->read(docs+total_len,freqs+total_len,len-total_len);
           for(int x=total_len;x<len;x++){
                docs[x]+=base;
                total_len=len;
           }
           break;
        }

    }
    return total_len;
}
vector<int>* lucene_index::IndexReader::getPoss(){
    return this->poss;
}
int lucene_index::IndexReader::getFreq(){
    return this->freq;
}
int lucene_index::IndexReader::getDocID(){
     return this->docid;
}
bool lucene_index::IndexReader::nextTerm(){
    int tmp_size=srs->size();

    for(int i=0;i<tmp_size;i++){

        if(isCur[i]==2){
           continue;
        }
        if(isCur[i]==1){
             isCur[i]=0;
             continue;
        }
         SegmentReader* sr=srs->at(i);
         bool tmp_flag=sr->termdocs->nextTerm();

         if(tmp_flag==true){
             sr->termposs->nextTerm();
         }else{
            isCur[i]=2;
         }
    }

    bool res_flag=false;

    for(int i=0;i<tmp_size;i++){
        if(isCur[i]==0){
            res_flag=true;
        }
    }

    if(res_flag==true){
        selectTerm();
        Comput_df();
        pos=0;
        return true;
    }else{
       return false;
    }
}

bool lucene_index::IndexReader::skipTo(int target){

     if(target<0||target>docCount){
        return false;
     }

     int posi=readerIndex(target);

     if(posi==-1){
        return false;
     }


     if(isCur[posi]==1 || isCur[posi]==2){
         return false;
     }

     SegmentReader* sr=srs->at(posi);

     bool flag=sr->termdocs->skipTo(target-starts[posi]);

     if(flag==true){
        pos=posi;
        sr->termposs->skipTo(target-starts[posi]);
        docid=sr->termdocs->doc+starts[pos];
        freq=sr->termdocs->freq;
        poss=sr->termposs->poss;
        return true;

     }else{
        return false;
     }
}
bool lucene_index::IndexReader::next(){
     int tmp_size=srs->size();
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
            SegmentReader* tmp_sr=srs->at(pos);
            bool tmp_flag=tmp_sr->termdocs->next();

            if(tmp_flag==true){
               tmp_sr->termposs->next();
               docid=tmp_sr->termdocs->doc+starts[pos];
               freq=tmp_sr->termdocs->freq;
               poss=tmp_sr->termposs->poss;
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
bool lucene_index::IndexReader::seek(Term* t){
      if(t==NULL){
         return false;
      }
      bool flag=false;

      int tmp_size=srs->size();

      for(int i=0;i<tmp_size;i++){
          SegmentReader* sr=srs->at(i);

          bool tmp_flag=sr->termdocs->seek(t);
          sr->termposs->seek(t);

          if(tmp_flag==false){

             TermInfo* ti=sr->termdocs->cti;
             wstring fname=ti->fname;
             wstring fvalue=ti->fvalue;

             Term tmp_t(fname,fvalue);

             if(tmp_t<(*t)){
                isCur[i]=2;
             }else{
                isCur[i]=1;
             }
          }else{
             isCur[i]=0;
             flag=true;

             if(curT==NULL){
                curT=new Term(*t);
             }else{
                 delete curT;
                 curT=new Term(*t);
             }
          }
      }//for

      if(flag==false){
         if(curT!=NULL){
            delete curT;
         }
         curT=NULL;
         return false;
      }

      if(flag==true){
         Comput_df();
         pos=0;
         return true;
      }
}
void lucene_index::IndexReader::Comput_df(){
         this->df=0;
         this->del_df=0;

         int tmp_size=srs->size();
         for(int i=0;i<tmp_size;i++){
             if(isCur[i]==0){
                 SegmentReader* sr=srs->at(i);
                 this->df+=sr->termdocs->df;
                 this->del_df+=sr->termdocs->del_df;
             }
         }
}
bool lucene_index::IndexReader::init_V_Term(){
      int tmp_size=srs->size();
      bool flag=false;
      for(int i=0;i<tmp_size;i++){
          SegmentReader* sr=srs->at(i);
          bool tmp_flag=sr->init_V_Term();
          if(tmp_flag==false){
             isCur[i]=2;
          }else{
             isCur[i]=0;
             flag=true;
          }
      }
      if(flag==false){
         return false;
      }else{
          selectTerm();
          Comput_df();
          pos=0;
          return true;
      }
}
void lucene_index::IndexReader::selectTerm(){
    TermInfo* min_ti=NULL;
    int tmp_size=srs->size();

    for(int i=0;i<tmp_size;i++){
        if(isCur[i]==1|| isCur[i]==2){
          continue;
        }
        SegmentReader* sr=srs->at(i);
        TermInfo* tmp_ti=sr->termdocs->cti;

        if(min_ti==NULL){
           min_ti=sr->termdocs->cti;
        }else{
           if((*tmp_ti)<(*min_ti)){
                min_ti=tmp_ti;
           }
        }
    }

    if(min_ti==NULL){
       return ;
    }
    wstring tmp_fname=min_ti->fname;
    wstring tmp_value=min_ti->fvalue;
    if(curT!=NULL){
       delete curT;
    }else{
       curT=new Term(tmp_fname,tmp_value);
    }

    for(int i=0;i<tmp_size;i++){
        if(isCur[i]==1|| isCur[i]==2){
          continue;
        }

        SegmentReader* sr=srs->at(i);
        TermInfo* tmp_ti=sr->termdocs->cti;
        if((*tmp_ti)==(*min_ti)){
            continue;
        }else{
            isCur[i]=1;
        }
    }
}
void lucene_index::IndexReader::resetDocPos(){
    int tmp_size=srs->size();

     for(int i=0;i<tmp_size;i++){
         SegmentReader* sr=srs->at(i);
         sr->resetDocPos();
     }
}
MTermDocs* lucene_index::IndexReader::getMTDocs(){
   MTermDocs* mtds=new MTermDocs(srs);
   return mtds;
}











