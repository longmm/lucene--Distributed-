#include <iostream>
#include "index.h"

using namespace std;

lucene_index::FieldInfo::FieldInfo(wstring fn,bool isi,int num){
     this->name=fn;
	 this->isIndexed=isi;
	 this->number=num;
}
lucene_index::FieldInfo::~FieldInfo(){
}
void lucene_index::FieldInfo::operator=(const FieldInfo& fi){
     this->name=fi.name;
     this->number=fi.number;
     this->isIndexed=fi.isIndexed;
}
lucene_index::FieldInfo::FieldInfo(const FieldInfo& fi){
   this->name=fi.name;
   this->number=fi.number;
   this->isIndexed=fi.isIndexed;
}


