#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::SegmentReader::SegmentReader(wstring name,wstring dir,int docCount){
      this->name=name;
	  this->dir=dir;
	  this->docCount=docCount;
	  wstring filename_fnm=dir+name+IndexFileNames::D+IndexFileNames::FIELD_INFO;
	  fis=new FieldInfos(filename_fnm);
	  norms_map=new map<wstring,Norm>();
	  init_fdx();
	  init_del();
	  getAllNorms();
	  termdocs=new SegmentTermDocs(this);
	  termposs=new SegmentTermPositions(this);
}
/*
void lucene_index::SegmentReader::writeDel(){
wstring file=dir+name+IndexFileNames::D+IndexFileNames::DELETES_EXTENSION;
char* filename=wstrToChars(file);
remove(filename);
FSIndexOutput* fsio=new FSIndexOutput(filename);

fsio->writeInt(delCount);

    for(int i=0;i<docCount;i++){
        if(isDel[i]==true){
            fsio->writeInt(i);
        }
    }
    delete fsio;

}
*/
bool lucene_index::SegmentReader::isDeleted(int i){
return isDel[i];
}
void lucene_index::SegmentReader::delDoc(int i){
isDel[i]=true;
delCount++;
}
void lucene_index::SegmentReader::init_del(){
      dels=new vector<int>();
      delCount=0;
      isDel=new bool[docCount];
      for(int i=0;i<docCount;i++){
          isDel[i]=false;
      }
      wstring file=dir+name+IndexFileNames::D+IndexFileNames::DELETES_EXTENSION;
      char* filename=wstrToChars(file);
      FSIndexInput* fsii=new FSIndexInput(filename);
      //fsii->readInt(&delCount);

      while(true){
          int tmp;
          int res=fsii->readInt(&tmp);
          if(res==0){
             break;
          }
          delCount++;
           isDel[tmp]=true;
           dels->push_back(tmp);
      }

      delete fsii;
}
void lucene_index::SegmentReader::init_fdx(){
       fdx=new long[docCount];
	   wstring fdx_file=dir+name+IndexFileNames::D+IndexFileNames::FDX;
       char* fdx_filename=wstrToChars(fdx_file);

	   FSIndexInput* fsio=new FSIndexInput(fdx_filename);

	   for(int i=0;i<docCount;i++){
           long tmp;
		   fsio->readLong(&tmp);
		   fdx[i]=tmp;
	   }
       delete fsio;
	   wstring fdt_file=dir+name+IndexFileNames::D+IndexFileNames::FDT;
       char* fdt_filename=wstrToChars(fdt_file);
	   fdt=new FSIndexInput(fdt_filename);
}
lucene_index::SegmentReader::~SegmentReader(){
        //writeDel();
       delete norms_map;
	   delete[] fdx;
	   delete fdt;
	   delete[] isDel;
	   delete dels;
	   delete fis;
	   delete termdocs;
	   delete termposs;
}
void lucene_index::SegmentReader::getAllNorms(){
	   wstring file=dir+name+IndexFileNames::D+IndexFileNames::NORMS_EXTENSION;
	   char* filename=wstrToChars(file);
       FSIndexInput* fs=new FSIndexInput(filename);
       int fsize=fis->size();
       for(int i=0;i<fsize;i++){
		   FieldInfo* fi=fis->fieldInfo(i);
           wstring name=fi->name;
		   float* norms=new float[docCount];
           for(int x=0;x<docCount;x++){
              fs->readFloat(norms+x);
           }
		   Norm norm(norms,docCount);
		   pair<wstring,Norm> p=make_pair(name,norm);
           norms_map->insert(p);
	   }
	   delete fs;
}
float* lucene_index::SegmentReader::getnorms(wstring name){
	  map<wstring,Norm>::iterator iter;
	  iter=norms_map->find(name);

	  if(iter==norms_map->end()){
         return NULL;
	  }else{
         return (iter->second).norm_floats;
	  }
}
Document* lucene_index::SegmentReader::getDoc(int i){
	if(i>=docCount){
       return NULL;
	}

	Document* doc=new Document();

	long pointer=fdx[i];

	fdt->seek(pointer);

	int fieldcount;
	fdt->readInt(&fieldcount);

	for(int i=0;i<fieldcount;i++){

        int fieldnum;
		fdt->readInt(&fieldnum);

        wstring value;
		fdt->readWString(&value);
        FieldInfo* fi=fis->fieldInfo(fieldnum);

        bool isIndexed=fi->isIndexed;
		wstring name=fi->name;

		Field* f=new Field(name,value,true,isIndexed,true);

         doc->add(f);

         delete f;
	}
	return doc;
}
bool lucene_index::SegmentReader::init_V_Term(){
      bool flag1=termdocs->init_V_Term();
      if(flag1==false){
         return false;
      }
      bool flag2=termposs->init_V_Term();
      if(flag2==false){
         return false;
      }
      return true;
}
void lucene_index::SegmentReader::resetDocPos(){
     termdocs->resetDocPos();
     termposs->resetDocPos();
}
