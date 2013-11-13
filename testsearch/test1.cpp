#include "lucene_search.h"

using namespace std;
using namespace lucene_search;
using namespace lucene_util;
using namespace store;
using namespace document;

int main(){

   wstring dir(L"/root/cwork/mylucene/testdata/");

   IndexReader* ir=new IndexReader(dir);

   IndexSearcher* is=new IndexSearcher(ir);

   
   wstring fname1(L"title");
   wstring fvalue1(L"B");

   Term t1(fname1,fvalue1);
   


   TermQuery* tq1=new TermQuery(&t1,false);

   wstring fname2(L"body");
   wstring fvalue2(L"s");

   Term t2(fname2,fvalue2);

   TermQuery* tq2=new TermQuery(&t2,false);

   BoolQuery * bq1=new BoolQuery(LAND);

   bq1->add(tq1);
   bq1->add(tq2);

   Hits* hits=is->search(bq1);

  Document* doc;

   while((doc=hits->nextDoc())!=NULL){

         set<Field>* tmp_fs=doc->getFields();
        
         wcout<<"================="<<endl;
          set<Field>::iterator iter;

          for(iter=tmp_fs->begin();iter!=tmp_fs->end();iter++){
               wcout<<L"field::"<<iter->name<<L"---------value::"<<iter->value<<endl;
          }

         wcout<<"================="<<endl; 
         delete doc; 
   }

   delete hits;
  
   delete is;
   delete ir;

   }
