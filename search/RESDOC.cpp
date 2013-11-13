#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;
using namespace document;

lucene_search::RESDOC::RESDOC(int docid,float score){
    this->docid=docid;
    this->score=score;
}
lucene_search::RESDOC::RESDOC(const RESDOC& adoc){
    this->docid=adoc.docid;
    this->score=adoc.score;
}
bool lucene_search::RESDOC::operator<(const RESDOC& resdoc)const{
    if(this->score==resdoc.score){
       return this->docid<resdoc.docid;
    }
    return this->score<resdoc.score;
}
bool lucene_search::RESDOC::operator>(const RESDOC& resdoc)const{
     if(this->score==resdoc.score){
       return this->docid>resdoc.docid;
    }
    return this->score>resdoc.score;
}
void lucene_search::RESDOC::operator=(const RESDOC& resdoc){
    this->docid=resdoc.docid;
    this->score=resdoc.score;
}
