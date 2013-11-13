#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::Posting::Posting(Term t){
   this->term=t;
   positions=new vector<int>();
   freq=0;
}
lucene_index::Posting::Posting(const Posting& p){
  this->term=p.term;
  this->freq=p.freq;
  this->positions=new vector<int>(*(p.positions));
}
void lucene_index::Posting::operator=(const Posting& p){
   this->term=p.term;
  this->freq=p.freq;
  if(positions!=NULL){
    delete positions;
  }
  this->positions=new vector<int>(*(p.positions));
}
bool lucene_index::Posting::operator<(const Posting& p){
     return (this->term)<(p.term);
}
bool lucene_index::Posting::operator==(const Posting& p){
     return (this->term)==(p.term);
}
lucene_index::Posting::~Posting(){
   delete positions;
}
void lucene_index::Posting::add(int pos){
    freq++;
    positions->push_back(pos);
}
