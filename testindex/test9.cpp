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
   wstring name(L"_1");
      
   IndexReader* ir=new IndexReader(dir);

   /*

   wstring fname(L"title");
   wstring fvalue(L"C");
   Term t(fname,fvalue);

   ir->seek(&t);


   ir->skipTo(39);

   cout<<ir->getDocID()<<endl;
   cout<<ir->getFreq()<<endl;

   */

   for(int i=0;i<ir->srs->size();i++){
       cout<<ir->starts[i]<<endl;
   }
   

   delete ir; 
}
