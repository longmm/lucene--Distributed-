#include "index.h"
#include "lucene_util.h"
#include <stdio.h>

using namespace lucene_index;
using namespace std;
using namespace lucene_util;

map<wstring,int> lucene_index::IndexFileDeleter::refCounts;
wstring lucene_index::IndexFileDeleter::dir;

void lucene_index::IndexFileDeleter::setDir(wstring d){
     lucene_index::IndexFileDeleter::dir=d;
}
 void lucene_index::IndexFileDeleter::checkpoint(SegmentInfos* sis){
		 int size=sis->size();
		 for(int i=0;i<size;i++){
            SegmentInfo* si=sis->info(i);
            map<wstring,int>::iterator iter;
			iter=refCounts.find((si->name));
			if(iter==refCounts.end()){
              pair<wstring,int> tmp((si->name),0);
			  refCounts.insert(tmp);
			}
		 }
}
void lucene_index::IndexFileDeleter::decRef(SegmentInfos* sis){

	  int size=sis->size();

      for(int i=0;i<size;i++){
            SegmentInfo* si=sis->info(i);
            map<wstring,int>::iterator iter;
			iter=refCounts.find((si->name));
			if(iter==refCounts.end()){
              wcout<<"error in decRef sis"<<endl;
			}else{
              int count=iter->second;
			  count--;
			  if(count<0){
                 refCounts.erase(iter);
                 deleteSegment(si);
			  }else{
                  pair<wstring,int> tmp((si->name),count);
                  refCounts.insert(tmp);
			  }//else
			}//else
	   }//for
}
void lucene_index::IndexFileDeleter::incRef(SegmentInfos* sis){

	int size=sis->size();

	for(int i=0;i<size;i++){
        SegmentInfo* si=sis->info(i);
        map<wstring,int>::iterator iter;
        iter=refCounts.find((si->name));
	    if(iter==refCounts.end()){
            pair<wstring,int> tmp((si->name),1);
            refCounts.insert(tmp);
		}else{
            int count=iter->second;
			count++;

			pair<wstring,int> tmp((si->name),count);
            refCounts.insert(tmp);
		}
	}
}
void lucene_index::IndexFileDeleter::deleteFile(wstring filename){
	 wstring file=dir+filename;
	 char* cfile=wstrToChars(file);

	 int res=remove(cfile);

	 delete[] cfile;
}
void lucene_index::IndexFileDeleter::deleteSegment(SegmentInfo* si){
	 vector<wstring>* files=si->getFiles();
	 vector<wstring>::iterator iter;
	 for(iter=files->begin();iter!=files->end();iter++){
            deleteFile(*iter);
	 }
}
void lucene_index::IndexFileDeleter::deleteSegment(wstring sname,SegmentInfo* si){

            map<wstring,int>::iterator iter;
			iter=refCounts.find(sname);
			if(iter==refCounts.end()){
              return ;
			}else{

              int count=iter->second;
			  count--;
			  if(count<0){
                 refCounts.erase(iter);
                 deleteSegment(si);
			  }else{
                  pair<wstring,int> tmp((si->name),count);
                  refCounts.insert(tmp);
			  }//else
			}//else
}
