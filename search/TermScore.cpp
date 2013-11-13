#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;

lucene_search::TermScore::TermScore(MTermDocs* m,Term* t,bool is_not){
   this->term=t;
   this->mtermdocs=m;
   mtermdocs->seek(t);
   this->is_not=is_not;
   this->docid=0;
   this->doc_score=0.0f;
   this->score_type=0;
   this->freq=0;
}
lucene_search::TermScore::~TermScore(){
   delete mtermdocs;
}
bool lucene_search::TermScore::next(){
     bool flag=mtermdocs->next();

     if(flag==false){
        return false;
     }

     this->docid=mtermdocs->docid;
     freq=mtermdocs->freq;

     return true;

}
bool lucene_search::TermScore::skipTo(int target){
    bool flag=mtermdocs->skipTo(target);

    if(flag==false){
       return false;
    }

    this->docid=mtermdocs->docid;
    freq=mtermdocs->freq;

    return true;
}
void lucene_search::TermScore::compute_score(){
    int tmp_df=mtermdocs->getDF();
    doc_score=tmp_df*tmp_df*freq*(mtermdocs->getNorm(docid));
}
