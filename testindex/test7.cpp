#include <string>
#include "store.h"
#include "index.h"
#include "document.h"
#include "lucene_util.h"

using namespace std;
using namespace store;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;

int main(){
  
   wstring dir(L"/root/cwork/mylucene/testdata/");
   wstring name(L"_2");
  
   int docCount=22;

      
   SegmentReader* sr=new SegmentReader(name,dir,22);

   wstring fname(L"title");
   wstring fvalue(L"C");
   Term t(fname,fvalue);

   sr->termdocs->seek(&t);

   sr->skipTo();
   
   delete sr; 
}
