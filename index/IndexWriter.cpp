#include <iostream>
#include "document.h"
#include "index.h"
#include "store.h"
#include "lucene_util.h"

using namespace std;
using namespace document;
using namespace lucene_index;
using namespace store;
using namespace lucene_util;

lucene_index::IndexWriter::IndexWriter(wstring dir){
       init();
       this->dir=dir;
       wstring wdir(dir);
	   wstring lockfile(L"write.lock");
	   fslock=new FSLock(lockfile,wdir);

       bool res=fslock->obtain();

	   if(res==true){
	      sis=new SegmentInfos(dir);
	      oldgeration=sis->generation;
		  //oldsis=new SegmentInfos(*sis);
	      //deleter=new IndexFileDeleter(dir);
	      //deleter->checkpoint(sis);
		  IndexFileDeleter::checkpoint(sis);
		  IndexFileDeleter::setDir(dir);
	   }else{
          wcout<<"obtain lock failed!!!"<<endl;
	   }

}
lucene_index::IndexWriter::~IndexWriter(){
	    fslock->release();
        delete fslock;
		delete sis;
		//delete oldsis;
}
void lucene_index::IndexWriter::init(){
               maxMergeDocs=INT_MAX;
	           mergeFactor=MERGE_FACTOR;
		       minMergeDocs=MIN_MERGE;
}
/*
void lucene_index::IndexWriter::abort(){
	   delete sis;
       sis=new SegmentInfos(*oldsis);
	   IndexFileDeleter::checkpoint(sis);
}
*/
SegmentInfo* lucene_index::IndexWriter::buildSingleDocSegment(Document* doc){
	   wstring newname=newTmpSegmentName();
	   //wstring* wdir=new wstring(charToWstring(dir));
	   DocumentWriter* dw=new DocumentWriter(dir);
	   dw->addDocument(newname,doc);
	   delete dw;
       SegmentInfo* si = new SegmentInfo(newname, 1,dir,true);
	   return si;
}
void lucene_index::IndexWriter::addDocument(Document* doc){
      SegmentInfo* si=buildSingleDocSegment(doc);
      sis->put(*si);
	  delete si;
      maybeMergeSegments(minMergeDocs);
}
void lucene_index::IndexWriter::commitTransaction(){
      bool flag=commit();
      if(flag==false){
            rollbackTransaction();
	  }
	  IndexFileDeleter::checkpoint(sis);
}
void lucene_index::IndexWriter::rollbackTransaction(){
       //this->abort();
}
int lucene_index::IndexWriter::docCount(){
        return sis->docCount;
}
void lucene_index::IndexWriter::flush(){
     maybeMergeSegments(minMergeDocs);
}
void lucene_index::IndexWriter::maybeMergeSegments(int startUpperBound){
	long lowerBound = -1;
    long upperBound = startUpperBound;
	while (upperBound < maxMergeDocs) {

	   int minSegment = sis->size();
       int maxSegment = -1;

           while (--minSegment >= 0) {
              SegmentInfo* si = sis->info(minSegment);

              if (maxSegment == -1 && si->docCount > lowerBound && si->docCount <= upperBound) {
                    maxSegment = minSegment;
              } else if (si->docCount > upperBound) {
                 break;
              }
           }

	   minSegment++;

	   int numSegments = maxSegment - minSegment+1;

	    if (numSegments < mergeFactor) {
        break;
      } else {
        bool exceedsUpperLimit = false;

		while (numSegments >= mergeFactor) {

               int docCount = mergeSegments(minSegment, minSegment + mergeFactor-1);
               numSegments -= mergeFactor;

               if (docCount > upperBound) {

                  minSegment++;
                  exceedsUpperLimit = true;
               } else {

               numSegments++;
               }
        }// while

		if (!exceedsUpperLimit) {
          // if none of the merged segments exceed upperBound, done
          break;
        }
	  }//else

	  lowerBound = upperBound;
      upperBound *= mergeFactor;
	}
}
int lucene_index::IndexWriter::mergeSegments(int start,int end){

      wstring newname=newTmpSegmentName();

      int total=0;
	  SegmentMerger* merger=new SegmentMerger(dir,newname);

	  for(int i=start;i<=end;i++){
          SegmentInfo* si=sis->info(i);
          total+=si->docCount;
          wstring tmp_s_name=si->name;
		  SegmentReader* sreader=new SegmentReader(tmp_s_name,dir,si->docCount);
		  merger->add(sreader);
	  }

	  merger->merge();

	  delete merger;

	  SegmentInfo* newsi=new SegmentInfo(newname,total,dir,true);

      removeSegment(start,end);

      sis->setInfo(start, newsi);

	  return total;
}
wstring lucene_index::IndexWriter::newTmpSegmentName(){
      long namecounter=(++(sis->namecounter));

	  wstring wstr=LongToWstring(namecounter);
	  wstring tmp(L"_");
	  wstring tmp1(L"tmp");
	  return tmp1+tmp+wstr;
}
wstring lucene_index::IndexWriter::newSegmentName(){
      long generation=(++(sis->generation));
	  wstring wstr=LongToWstring(generation);
	  wstring tmp(L"_");
	  return tmp+wstr;
}
bool lucene_index::IndexWriter::commit(){

	 renameSegment();
     long newgen=sis->generation;
	 //long oldgen=oldsis->generation;

	 if(newgen==oldgeration){
          return true;
	 }
	 wstring tmp=LongToWstring(newgen);
	 wstring tmp2(L"_");
	 wstring filename=dir+IndexFileNames::SEGMENTS+tmp2+tmp;

     sis->SISwrite(filename);

     oldgeration=newgen;

	 return true;
}
void lucene_index::IndexWriter::renameSegment(){

    int size=sis->size();

	for(int i=0;i<size;i++){
       SegmentInfo* si=sis->info(i);
	   if(si->isTMP==true){
		  si->isTMP=false;
		  wstring newname=newSegmentName();
		  renameSegmentFiles(si->name,newname);
          si->name=newname;
          si->obfiles();
	   }
	}
}
void lucene_index::IndexWriter::renameSegmentFiles(wstring oldseg,wstring newseg){
      vector<wstring>* ieif=IndexFileNames::getIEICF();

		   for(int i=0;i<ieif->size();i++){
                 wstring ext_tmp=ieif->at(i);
				 wstring d(L".");
				 wstring res=dir+oldseg+d+ext_tmp;
				 if(fileExist(res)){
                      wstring newres=dir+newseg+d+ext_tmp;

                      char* resfile=wstrToChars(res);
                      char* newresfile=wstrToChars(newres);
				      rename(resfile,newresfile);
				      delete[] resfile;
				      delete[] newresfile;
				 }
		   }
}
void lucene_index::IndexWriter::deleteSegment(int pos){

	SegmentInfo* si=sis->info(pos);
	wstring name=si->name;
    if(si->isTMP==false){
	    IndexFileDeleter::deleteSegment(name,si);
    }else{
         si->deleteFiles();
    }
	sis->remove(pos);
}
void lucene_index::IndexWriter::removeSegment(int start,int end){
	for(int i=start;i<=end;i++){
           deleteSegment(start);
	}
}
void lucene_index::IndexWriter::deleteDocument(int num){
     int sis_size=sis->size();
     int total=0;
     int i;
     for(i=0;i<sis_size;i++){
        SegmentInfo* si=sis->info(i);
        if(num>=total&&num<(total+si->docCount)){
             break;
        }else{
          total+=si->docCount;
        }
     }
     if(i==sis_size){
         wcout<<"the num is yue jie le"<<endl;
        return;
     }
     int newnum=num-total;

     SegmentInfo* si=sis->info(i);
     wstring sname=si->name;
     wstring file=dir+sname+IndexFileNames::D+IndexFileNames::DELETES_EXTENSION;
     char* filename=wstrToChars(file);

     FSIndexOutput* fsio=new FSIndexOutput(filename);

     fsio->writeInt(newnum);

     delete fsio;
}
