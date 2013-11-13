#include "lucene_search.h"
#include "index.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;

lucene_search::IndexSearcher::IndexSearcher(IndexReader* ir){
    this->ir=ir;
}
lucene_search::IndexSearcher::~IndexSearcher(){
    if(mq->query_type==1){
       delete (BoolQuery*)mq;
    }else if(mq->query_type==0){
        delete (TermQuery*)mq;
    }
    if(mc->score_type==0){
       delete (TermScore*)mc;
    }else if(mc->score_type==1){
       delete (BoolScore*)mc;
    }
}
Hits* lucene_search::IndexSearcher::search(Query* q){
   this->mq=q;
   this->mc=make_score_tree(q);
   ((BoolScore*)(this->mc))->compute();

   Hits* hits=new Hits(ir);

   while(mc->next()){
       hits->add(mc->docid,mc->doc_score);
   }
   return hits;
}
Score* lucene_search::IndexSearcher::make_score_tree(Query* tmp_q){
     if(tmp_q==NULL){
         return NULL;
     }

     if(tmp_q->query_type==0){
        TermQuery* tq=(TermQuery*)tmp_q;
        MTermDocs* mtds=ir->getMTDocs();
        Score* tmp_sc=new TermScore(mtds,tq->term,tq->is_not);
        return tmp_sc;
     }

    BoolQuery* bq=(BoolQuery*)tmp_q;

     BoolScore* bsc=new BoolScore(bq->type);

     int tmp_qsize=bq->querys->size();


     for(int i=0;i<tmp_qsize;i++){
         Query* tmp_q2=bq->querys->at(i);
         Score* tmp_sc=make_score_tree(tmp_q2);
         bsc->add(tmp_sc);
     }

     return bsc;
}
