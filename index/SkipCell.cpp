#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::SkipCell::SkipCell(){
}
lucene_index::SkipCell::SkipCell(int docid,long frqp,long proxp,int childp){
    this->docid=docid;
    this->frqp=frqp;
    this->proxp=proxp;
    this->childp=childp;
}
lucene_index::SkipCell::SkipCell(const SkipCell& sc){
    this->docid=sc.docid;
    this->frqp=sc.frqp;
    this->proxp=sc.proxp;
    this->childp=sc.childp;
}
void lucene_index::SkipCell::operator=(const SkipCell& sc){
    this->docid=sc.docid;
    this->frqp=sc.frqp;
    this->proxp=sc.proxp;
    this->childp=sc.childp;
}
