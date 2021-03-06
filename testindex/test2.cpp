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

   for(int i=0;i<100;i++){

    wstring fn1(L"title");
    wstring v1(L"A B C");

    Field* f1=new Field(fn1,v1,true,true,true);
  
    wstring fn2(L"head");
    wstring v2(L"this is my head");

    Field* f2=new Field(fn2,v2,true,true,true);

    wstring fn3(L"body");   
    wstring v3(L"this is my body");
  
    Field* f3=new Field(fn3,v3,true,true,true);
   

    Document* doc=new Document();
    if(i%1==0)
    doc->add(f1);

    if(i%2==0)
    doc->add(f2);

    if(i%3==0)
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
