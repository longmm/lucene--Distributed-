#include <iostream>
#include <string>
#include "lucene_util.h"
#include "store.h"

using namespace store;
using namespace lucene_util;
using namespace std;
int main(){
  wstring file(L"/root/cwork/mylucene/testdata/data1");
  char* filename=wstrToChars(file);
  FSIndexOutput* fsio=new FSIndexOutput(filename);
  //fsio->writeInt(1);
  //fsio->writeInt(2);
  //fsio->writeInt(3);
  fsio->writeInt(5);
  //
  delete fsio;
  return 0;
}
