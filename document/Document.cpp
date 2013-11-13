#include <iostream>
#include <set>
#include <utility>
#include <string>
#include "document.h"


using namespace std;
using namespace document;


bool document::Document::add(Field* field){
        if(field==NULL){
            wcout<<"this field p is null"<<endl;
			return false;
		}

		pair<set<Field>::iterator,bool> p=(this->fields)->insert((*field));

        if(p.second==false){
			wcout<<"the insert have some problem....."<<endl;
            return false;
		}else{
			return true;
		}
}
wstring document::Document::get(wstring name)
{
	if(this->fields==NULL)
	{
		return NULL;
	}

	set<Field>::iterator iter;
	for(iter=fields->begin();iter!=fields->end();iter++)
	{
		wstring fieldname=iter->getName();
		if((fieldname)==(name)){
             return iter->getValue();
		   }
	 }
	 wstring res;
	 return res;
}
float document::Document::getBoost(){
      return this->boost;
}
Field* document::Document::getField(wstring name){
      if(name.size()==0){
         return NULL;
	  }
      if(this->fields==NULL)
	  {
		return NULL;
	  }
	  set<Field>::iterator iter;
	  for(iter=fields->begin();iter!=fields->end();iter++)
	  {
		wstring fieldname=iter->getName();
		if((fieldname)==(name)){
             return (Field*)(&(*iter));
		   }
	  }
		return NULL;
}
Field* document::Document::getField(int i){
      int fsize=fields->size();
     if(i>=fsize){
      return NULL;
     }

     set<Field>::iterator iter;
     iter=fields->begin();

     while((i--)>0){
       iter++;
     }
      return (Field*)(&(*iter));
      //return (Field*)iter;
}
set<Field>* document::Document::getFields()const{
       return this->fields;
}
bool document::Document::removeField(wstring name){
      if(name.size()==0){
         return false;
	  }
      if(this->fields==NULL)
	  {
		return false;
	  }
	  set<Field>::iterator iter;
	  for(iter=fields->begin();iter!=fields->end();iter++)
	  {
		wstring fieldname=iter->getName();
		if((fieldname)==(name)){
			 fields->erase(iter);
             return true;
		   }
	  }
		return false;
}
void document::Document::setBoost(float boost){
      this->boost=boost;
}
