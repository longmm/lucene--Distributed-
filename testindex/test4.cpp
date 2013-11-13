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
      
   SegmentReader* sr=new SegmentReader(name,dir,1);

   sr->init_V_Term();

   while(sr->termdocs->next()){
       cout<<sr->termdocs->doc<<"-------"<<sr->termdocs->freq<<endl;
   }

   while(sr->termposs->next()){
      cout<<sr->termposs->doc<<endl;
      int tmp_size=sr->termposs->poss->size();
     
      for(int i=0;i<tmp_size;i++){
         int pos=sr->termposs->poss->at(i);
         cout<<pos<<endl;
      }
   }
   

   delete sr; 
}
