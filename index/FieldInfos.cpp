#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <string.h>
#include <utility>
#include "store.h"
#include "document.h"
#include "index.h"
#include "lucene_util.h"

using namespace std;
using namespace store;
using namespace document;
using namespace lucene_index;
using namespace lucene_util;

lucene_index::FieldInfos::FieldInfos(){
      init();
}
void lucene_index::FieldInfos::init(){
     byNumber=new vector<FieldInfo>();
	 byName=new map<wstring,FieldInfo>();
}
lucene_index::FieldInfos::FieldInfos(wstring filename){
	init();
    char* file=wstrToChars(filename);
    FSIndexInput* fsi=new FSIndexInput(file);
	readFields(fsi);
	delete fsi;
}
lucene_index::FieldInfos::~FieldInfos(){
   delete byNumber;
   delete byName;
}
FieldInfo* lucene_index::FieldInfos::addInternal(wstring fieldname,bool isIndexed){
   FieldInfo* fi=new FieldInfo(fieldname,isIndexed,byNumber->size());
   byNumber->push_back((*fi));
   pair<wstring,FieldInfo> p=make_pair(fieldname,(*fi));

   byName->insert(p);
   delete fi;
   return fi;
}
FieldInfo* lucene_index::FieldInfos::fieldInfo(int fieldNumber){
    if(fieldNumber<byNumber->size()){
            return &(byNumber->at(fieldNumber));
	}else{
            return NULL;
	}
}
FieldInfo* lucene_index::FieldInfos::fieldInfo(wstring fieldname){
   map<wstring,FieldInfo>::iterator iter=byName->find(fieldname);
   if(iter==byName->end()){
         return NULL;
   }else{
         return &(iter->second);
   }
}
wstring lucene_index::FieldInfos::fieldName(int fieldNumber){
	 if(fieldNumber<byNumber->size()){
            return (byNumber->at(fieldNumber)).name;
	}else{
	        wstring tmp_str(L"");
            return tmp_str;
	}
}
void  lucene_index::FieldInfos::add(Document* doc){
     set<Field>* setfp=doc->getFields();
	 if(setfp==NULL || setfp->size()==0){
          return;
	 }
	 set<Field>::iterator iter;
	 for(iter=setfp->begin();iter!=setfp->end();iter++){
         wstring name=iter->getName();
		 bool isIndexed=iter->indexed;
		 add(name,isIndexed);
	 }
}
void   lucene_index::FieldInfos::add(wstring name,bool isIndexed){
	  FieldInfo* fi=fieldInfo(name);
	  if(fi==NULL){
          addInternal(name,isIndexed);
	  }else{
          fi->isIndexed=isIndexed;
	  }
}
void  lucene_index::FieldInfos::add(FieldInfos* fis){
       int fsize=fis->size();
       for(int i=0;i<fsize;i++){
           FieldInfo* fs=fis->fieldInfo(i);
		   wstring name=fs->name;
		   bool isIndexed=fs->isIndexed;
		   add(name,isIndexed);
	   }
}
int  lucene_index::FieldInfos::fieldNum(wstring fieldname){
        FieldInfo* fi=fieldInfo(fieldname);
		if(fi==NULL){
            return -1;
		}else{
           return fi->number;
		}
}
int  lucene_index::FieldInfos::size(){
        return byNumber->size();
}
void lucene_index::FieldInfos::readFields(IndexInput* fsindexinput){
	int size;
	fsindexinput->readInt(&size);

	for(int i=0;i<size;i++){
		wstring fieldname=readFieldNames(fsindexinput);
		char flag;
		fsindexinput->readChar(&flag);
		bool isIndexed=((flag & IS_INDEXED) != 0);
		addInternal(fieldname,isIndexed);
	}

}
wstring lucene_index::FieldInfos::readFieldNames(IndexInput* fsindexinput){
		   wstring name;
           fsindexinput->readWString(&name);
		   return name;
}
void  lucene_index::FieldInfos::writeFields(IndexOutput* fsio){
           fsio->writeInt(this->size());
		   for(int i=0;i<this->size();i++){
              FieldInfo* fi=fieldInfo(i);
              writeFieldName(fsio,fi->name);
			  char flag=0x0;
              if (fi->isIndexed) flag |= IS_INDEXED;
			  fsio->writeChar(flag);
		   }
}
void lucene_index::FieldInfos::writeFieldName(IndexOutput* fsio, wstring name){
		   fsio->writeWString(name);
}
void  lucene_index::FieldInfos::writeFields(wstring filename){
       char* file=wstrToChars(filename);
       FSIndexOutput* fsio=new FSIndexOutput(file);
	   writeFields(fsio);
	   delete fsio;
}
