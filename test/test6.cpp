#include <map>
#include <string>
#include <iostream>

using namespace std;

int main(){

   map<int,wstring>* mmap=new map<int,wstring>();

   int a=10;
   wstring a_str(L"aaa");
   pair<int,wstring> p1=make_pair(a,a_str);
   mmap->insert(p1);

   int b=12;
   wstring b_str(L"bbb");
   pair<int,wstring> p2=make_pair(b,b_str);
   mmap->insert(p2);


   int c=15;
    wstring c_str(L"ccc");
   pair<int,wstring> p3=make_pair(c,c_str);
   mmap->insert(p3);


   int d=1;
   wstring d_str(L"ddd");
   pair<int,wstring> p4=make_pair(d,d_str);
   mmap->insert(p4);

   
   map<int,wstring>::iterator iter;
   iter=mmap->lower_bound(20);

   
   if(iter==mmap->end()){
     cout<<"dd"<<endl;
   }

   /*
   wcout<<iter->first<<"---------"<<iter->second<<endl;
   iter++;
   wcout<<iter->first<<"---------"<<iter->second<<endl;
   */
   delete mmap;

}
