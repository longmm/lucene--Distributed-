#ifndef MY_DOCUMENT
#define MY_DOCUMENT

#include <iostream>
#include <set>
#include <string>

using namespace std;

namespace document{

class Fieldable
{
     public:
		 virtual float getBoost()=0;
	      virtual bool getOmitNorms()=0;
		 virtual bool isIndexed()=0;
		 virtual bool isLazy()=0;
		 virtual bool isStored()=0;
		 virtual bool isTokenized()=0;
         virtual wstring getName()const=0;
		 virtual void setBoost(float boost)=0;
};
class AbstractField:public Fieldable
{
    public:
		 wstring name;
	         wstring value;
		 bool omitNorms;
                 bool stored;
		 bool indexed;
		 bool lazy;
		 bool tokenized;
		 float boost;

	         float getBoost();
                 bool getOmitNorms();
                 bool isIndexed();
                 bool isLazy();
                 bool isStored();
                 bool isTokenized();
                 wstring getName()const;
	         void setBoost(float);
	         void setOmitNorms(bool);
};
class Field:public AbstractField
{
     public:
            Field(wstring name,wstring value,bool isStore,bool isIndexed,bool isTokenized)
	     {
               this->name=name;
	       this->value=value;
	       this->stored=isStore;
	       this->indexed=isIndexed;
	       this->tokenized=isTokenized;
	       if(this->tokenized==true){
                    this->indexed=true;
	       }
		   boost=1.0f;
	     }
		 ~Field(){
             //delete name;
			 //delete value;
		 }
		 Field(const Field& f){
            //wstring* newname=new wstring(*(f.name));
			this->name=f.name;
			//wstring* newvalue=new wstring(*(f.value));
			this->value=f.value;
			this->stored=f.stored;
			this->indexed=f.indexed;
			this->tokenized=f.tokenized;
			this->boost=f.boost;
		 }
         wstring getValue() const
		 {
              return  this->value;
		 }
		 void operator=(const Field& f){
            this->name=f.name;
			//wstring* newvalue=new wstring(*(f.value));
			this->value=f.value;
			this->stored=f.stored;
			this->indexed=f.indexed;
			this->tokenized=f.tokenized;
			this->boost=f.boost;
		 }
         bool operator==(const Field& f)const{
			 return (this->name)==(f.getName());
         }

		 bool operator<(const Field& f)const{
             return (this->name)<(f.getName());
		 }
};
class Document
{
     public :
		 float boost;
         set<Field>*  fields;

		  Document()
		  {
              boost=1.0f;
              fields=new set<Field>();
	      }
		  Document(const Document& doc){
              this->boost=doc.boost;
			  this->fields=new set<Field>(*(doc.getFields()));
		  }
          ~Document()
	      {
               delete fields;
	      }
          bool add(Field* field);
		  wstring get(wstring name);
          float getBoost();
		  Field* getField(wstring name);
		  Field* getField(int i);
		  set<Field>* getFields() const;
          bool removeField(wstring name);
		  void setBoost(float boost);
};
};
#endif
