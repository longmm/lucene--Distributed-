#include "lucene_util.h"

using namespace lucene_util;
using namespace std;

set<wstring> lucene_util::StopWords::stops;
bool lucene_util::StopWords::init_flag=false;
bool lucene_util::StopWords::isStops(wstring word){
    if(init_flag==false){
       wstring a(L" ");
       stops.insert(a);
       init_flag=true;
    }

    set<wstring>::iterator iter;

    iter=stops.find(word);

    if(iter==stops.end()){
       return false;
    }else{
       return true;
    }
}
