#include <iostream>
#include <string>

using namespace std;

int main(){

   wstring hh(L"_1.del");
   wstring bb(L"segments");

   int i=hh.find(bb,0);
  // wstring str=hh.substr(1,3);

   wcout<<i<<endl;

}
