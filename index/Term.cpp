#include <iostream>
#include <string>
#include "index.h"

using namespace std;
using namespace lucene_index;

lucene_index::Term::Term(wstring f,wstring t){
      this->field=f;
	  this->text=t;
}
lucene_index::Term::Term(){
}
lucene_index::Term::~Term(){
      //delete field;
	  //delete text;
}
lucene_index::Term::Term(const Term& t){
      this->field=t.field;
      this->text=t.text;
}
bool lucene_index::Term::operator<(const Term& t)const{
	 wstring tfield=t.field;
	 if((field)==(tfield)){
         return (text)<((t.text));
	 }else{
		 return (field)<((t.field));
	 }
}
bool lucene_index::Term::operator==(const Term& t)const{
      if(field==t.field){
         return text==t.text;
	 }else{
		 return false;
	 }
}
bool lucene_index::Term::operator=(const Term& t){
    this->field=t.field;
    this->text=t.text;
}
wstring lucene_index::Term::getField()const{
       return field;
}
void lucene_index::Term::setTerm(wstring f,wstring t){
       this->field=f;
	   this->text=t;
}
wstring lucene_index::Term::getText()const{
        return text;
}
