#include "lucene_search.h"

using namespace std;
using namespace lucene_search;

lucene_search::BoolScore::BoolScore(int type){
    this->type=type;
    subScores=new vector<Score*>();
    res=new map<int,float>();
    NOT_set=new set<int>();
    XH_flag=false;
    this->docid=0;
    this->doc_score=0.0f;
    this->score_type=1;
    flags=NULL;
}
lucene_search::BoolScore::~BoolScore(){
    int s_size=subScores->size();

    for(int i=0;i<s_size;i++){
        Score* tmp_sc=subScores->at(i);
        if(tmp_sc->score_type==0){
            delete (TermScore*)tmp_sc;
        }else{
            delete (BoolScore*)tmp_sc;
        }
    }

    delete subScores;
    delete res;
    delete NOT_set;
    delete[] flags;
}
void lucene_search::BoolScore::add(Score* sc){

      if(sc->score_type==0){
           TermScore* tmptsc=(TermScore*)sc;
           if(tmptsc->is_not==true){
                while(tmptsc->next()){
                    NOT_set->insert(tmptsc->docid);
                }
                return;
           }
      }

     subScores->push_back(sc);
}
bool lucene_search::BoolScore::skipTo(int target){
    map<int,float>::iterator iter;
    iter=res->lower_bound(target);

    if(iter==res->end()){
        return false;
    }

    this->docid=iter->first;
    this->doc_score=iter->second;
    this->pos_iter=iter;
    return true;
}
bool lucene_search::BoolScore::contain(int target){
   set<int>::iterator iter;
   iter=NOT_set->find(target);

   if(iter==NOT_set->end()){
      return false;
   }else{
      return true;
   }


}
bool lucene_search::BoolScore::next(){
    if(res->size()==0){
       return false;
    }
    if(XH_flag==false){
        pos_iter=res->begin();
        this->docid=pos_iter->first;
        this->doc_score=pos_iter->second;
        XH_flag=true;
        return true;
    }

    pos_iter++;

    if(pos_iter==res->end()){
        return false;
    }

    this->docid=pos_iter->first;
    this->doc_score=pos_iter->second;
    return true;
}
void lucene_search::BoolScore::compute(){
    int tmp_size=subScores->size();

    if(tmp_size==0){
        return ;
    }

    flags=new int[tmp_size];
    for(int i=0;i<tmp_size;i++){
        flags[i]=1;
    }

    for(int i=0;i<tmp_size;i++){
        Score* tmp_sc=subScores->at(i);
        if(tmp_sc->score_type==1){
           ((BoolScore*)tmp_sc)->compute();
        }
    }

    if(tmp_size==1){
        Score* tmp_sc=subScores->at(0);
        while(tmp_sc->next()){
            tmp_sc->compute_score();

            int tmp_docid=tmp_sc->docid;
            float tmp_score=tmp_sc->doc_score;
            if(contain(tmp_docid)){
                continue;
            }
            pair<int,float> tmp_p=make_pair(tmp_docid,tmp_score);
            res->insert(tmp_p);
        }
        return;
    }

    if(type==0){

        while(true){

           for(int i=0;i<tmp_size;i++){
              Score* tmp_sc=subScores->at(i);
              bool tmp_flag=tmp_sc->next();
              if(tmp_flag==false){
                  return ;
              }
           }


           while(true){
           int maxdocid=0;
           bool tmp_f2=true;
           float tmp_score=0.0f;

           for(int i=0;i<tmp_size;i++){
               Score* tmp_sc=subScores->at(i);
               if(tmp_sc->docid>maxdocid){
                    maxdocid=tmp_sc->docid;
               }
           }

           for(int i=0;i<tmp_size;i++){

               Score* tmp_sc=subScores->at(i);
               tmp_sc->compute_score();
               tmp_score+=tmp_sc->doc_score;

               if(tmp_sc->docid<maxdocid){
                   tmp_f2=false;
                   bool tmp_f1=tmp_sc->skipTo(maxdocid);
                   if(tmp_f1==false){
                      return ;
                   }
               }
           }
            if(tmp_f2==true){
                 if(!contain(maxdocid)){
                     pair<int,float> tmp_p=make_pair(maxdocid,tmp_score);
                     res->insert(tmp_p);
                 }
                 break;
            }

           }//while(true) inner




        }
    }

    if(type==1){

        while(true){

        bool tmp_f3=false;

        for(int i=0;i<tmp_size;i++){

            if(flags[i]==2 || flags[i]==0){
               continue;
            }

            Score* tmp_sc=subScores->at(i);
            bool tmp_flag=tmp_sc->next();
            if(tmp_flag==false){
                  flags[i]=2 ;
            }else{
                tmp_f3=true;
            }
        }

         if(tmp_f3==false){
            return;
         }

         int mindocid=0;

         for(int i=0;i<tmp_size;i++){
               if(flags[i]==2){
                   continue;
               }
               Score* tmp_sc=subScores->at(i);
               if(i==0){
                  mindocid=tmp_sc->docid;
               }else if(tmp_sc->docid<mindocid){
                    mindocid=tmp_sc->docid;
               }
          }

          float tmp_score=0.0f;
          for(int i=0;i<tmp_size;i++){
                if(flags[i]==2){
                    continue;
                }
                Score* tmp_sc=subScores->at(i);

                if(mindocid==tmp_sc->docid){
                    flags[i]=1;
                    tmp_sc->compute_score();
                    tmp_score+=tmp_sc->doc_score;
                }else{
                    flags[i]=0;
                }
          }

          if(!contain(mindocid)){
              pair<int,float> tmp_p=make_pair(mindocid,tmp_score);
              res->insert(tmp_p);
          }

        }//while
    }
}
void lucene_search::BoolScore::compute_score(){
}
