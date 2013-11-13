#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_index;

lucene_search::BoolQuery::BoolQuery(int type){
      this->type=type;
      query_type=1;
      querys=new vector<Query*>();
}
lucene_search::BoolQuery::~BoolQuery(){
     int q_size=querys->size();

     for(int i=0;i<q_size;i++){
         Query* q=querys->at(i);
         delete q;
     }

      delete querys;
}
void lucene_search::BoolQuery::add(Query* q){
      querys->push_back(q);
}
