#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;

lucene_search::TermQuery::TermQuery(Term* t,bool is_not){
    this->term=t;
    this->query_type=0;
    this->is_not=is_not;
}
lucene_search::TermQuery::~TermQuery(){
}
