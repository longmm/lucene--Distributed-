#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;
using namespace document;

lucene_search::Hits::Hits(IndexReader* ir){
   this->ir=ir;
   p_que=new priority_queue<RESDOC>();

}
lucene_search::Hits::~Hits(){
   delete p_que;
}
void lucene_search::Hits::add(int docid,float score){
    RESDOC tmpdoc(docid,score);
    p_que->push(tmpdoc);
}
Document* lucene_search::Hits::nextDoc(){
    if(p_que->size()==0){
        return NULL;
    }

    RESDOC tmp_doc=p_que->top();
    p_que->pop();

    return ir->getDoc(tmp_doc.docid);
}
