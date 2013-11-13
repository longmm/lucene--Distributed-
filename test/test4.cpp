#include <iostream>
#include <map>

using namespace std;

class A{
   public:
     int i;
     A(int i){
       this->i=i;
     }
};

int main(){
 map<int,A>* tmp_map=new map<int,A>();
 A a(20);
 pair<int,A> tmp_p=make_pair(1,a);

 tmp_map->insert(tmp_p);

 map<int,A>::iterator iter;

 iter=tmp_map->find(1);

 (iter->second).i=35;

 iter=tmp_map->find(1);

 cout<<(iter->second).i<<endl;

 delete tmp_map;

}
