#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace store;
using namespace document;

lucene_index::SegmentMergeInfo::SegmentMergeInfo(int base,SegmentTermDocs* termDocs,SegmentTermPositions* termpos){
             this->base=base;
             this->termDocs=termDocs;
             this->termpos=termpos;
}
bool lucene_index::SegmentMergeInfo::nextTerm(){
     bool flag;
     flag=termDocs->nextTerm();

     if(flag==false){
        return false;
     }

     flag=termpos->nextTerm();

     if(flag==false){
       return false;
     }
     return true;
}
bool lucene_index::SegmentMergeInfo::next(){
     bool flag;
     flag=termDocs->next();
     if(flag==false){
         return false;
     }
     flag=termpos->next();
     if(flag==false){
         return false;
     }
     return true;
}
int lucene_index::SegmentMergeInfo::getDocId(){
    return termDocs->doc+base;
}
int lucene_index::SegmentMergeInfo::getFreq(){
     return termDocs->freq;
}
vector<int>* lucene_index::SegmentMergeInfo::getPoss(){
     return termpos->poss;
}

