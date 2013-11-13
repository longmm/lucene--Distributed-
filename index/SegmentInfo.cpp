#include <iostream>
#include "index.h"
#include "lucene_util.h"
#include <string>
#include <vector>


using namespace std;
using namespace lucene_index;
using namespace lucene_util;

int lucene_index::SegmentInfo::NO=-1;
int lucene_index::SegmentInfo::YES=1;
lucene_index::SegmentInfo::SegmentInfo(wstring dir,IndexInput* ii){
     this->dir=dir;
     ii->readWString(&(this->name));
	 ii->readInt(&(this->docCount));
	 ii->readLong(&(this->delGen));
	 isTMP=false;
	 files=NULL;
	 obfiles();
}
lucene_index::SegmentInfo::SegmentInfo(wstring name,int docCount,wstring dir,bool istmp)
{
      this->name=name;
	  this->docCount=docCount;
	  this->dir=dir;
	  this->isTMP=istmp;
	  this->delGen=0;
	  files=NULL;
      obfiles();
}
lucene_index::SegmentInfo::SegmentInfo(const SegmentInfo& si){
      this->name=si.name;
	  this->docCount=si.docCount;
	  this->dir=si.dir;
	  this->files=new vector<wstring>(*(si.files));
	  this->delGen=si.delGen;
	  this->isTMP=si.isTMP;
}
void lucene_index::SegmentInfo::operator=(const SegmentInfo& si){
     this->name=si.name;
	  this->docCount=si.docCount;
	  this->dir=si.dir;
	  if(this->files!=NULL){
	      delete files;
	  }
	  this->files=new vector<wstring>();
	  *(this->files)=(*(si.files));
	  this->delGen=si.delGen;
	  this->isTMP=si.isTMP;
}
lucene_index::SegmentInfo::~SegmentInfo(){
	  delete files;
}
vector<wstring>* lucene_index::SegmentInfo::getFiles(){
     if(files==NULL){
          obfiles();
	 }else{
          return files;
	 }
}
vector<wstring>* lucene_index::SegmentInfo::obfiles(){
      if(files!=NULL){
          files->clear();
	  }else{
           files=new vector<wstring>();
	  }
           vector<wstring>* ieif=IndexFileNames::getIEICF();

		   for(int i=0;i<ieif->size();i++){
                 wstring ext_tmp=ieif->at(i);
				 wstring d(L".");
				 wstring res=name+d+ext_tmp;
				 if(fileisExisted(res)){
                      files->push_back(res);
				 }
		   }

		   wstring segment_name=IndexFileNames::SEGMENTS+name;

		   if(fileisExisted(segment_name)){
                 files->push_back(segment_name);
		   }
		   return files;
}
bool lucene_index::SegmentInfo::fileisExisted(wstring filename){
       wstring wfilename=dir+filename;
       return fileExist(wfilename);
}
wstring lucene_index::SegmentInfo::getDelFileName(){
     wstring delname=(name)+(IndexFileNames::D)+ (IndexFileNames::DELETES_EXTENSION);
     return delname;
}
wstring lucene_index::SegmentInfo::getNormFileName(){
     wstring delname=(name)+(IndexFileNames::D)+ (IndexFileNames::NORMS_EXTENSION);
     return delname;
}
void lucene_index::SegmentInfo::write(IndexOutput* io){
     io->writeWString(name);
	 io->writeInt(docCount);
	 io->writeLong(delGen);
}
void lucene_index::SegmentInfo::deleteFiles(){

     if(files==NULL){
        return;
     }
     int fsize=files->size();

     for(int i=0;i<fsize;i++){
         wstring fname=files->at(i);
         wstring file=dir+fname;
         char* filename=wstrToChars(file);
         remove(filename);
         delete[] filename;
     }
}
/*
void lucene_index::SegmentInfo::advanceDelGen(){
       if (delGen == NO) {
      delGen = YES;
    } else {
      delGen++;
    }
}
void lucene_index::SegmentInfo::clearDelGen(){
     delGen = NO;
}
*/

