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
  
   wstring file(L"/root/cwork/mylucene/testdata/");
      
   IndexWriter* iw=new IndexWriter(file);
   
   int i=0;

   for(int i=0;i<1000;i++){

    wstring fn1(L"title");
    wstring v1(L"this is my title");

    Field* f1=new Field(fn1,v1+LongToWstring((long)i),true,true,true);
  
    wstring fn2(L"head");
    wstring v2(L"this is my head");

    Field* f2=new Field(fn2,v2+LongToWstring((long)i),true,true,true);

    wstring fn3(L"body");   
    wstring v3(L"this is my body");
  
    Field* f3=new Field(fn3,v3+LongToWstring((long)i),true,true,true);
   

    Document* doc=new Document();
    doc->add(f1);
    doc->add(f2);
    doc->add(f3);
    iw->addDocument(doc);
    delete doc;
    delete f1;
    delete f2;
    delete f3;

   }
   iw->commitTransaction();
   delete iw;
}
