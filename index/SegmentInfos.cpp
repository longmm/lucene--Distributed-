#include "index.h"

using namespace std;
using namespace store;
using namespace lucene_index;
using namespace lucene_util;

lucene_index::SegmentInfos::SegmentInfos(wstring dir){
   this->dir=dir;
   sivector=new vector<SegmentInfo>();
   docCount=0;
   generation=getGen();
   namecounter=generation;
   if(generation!=0){
		SISread();
   }
}
lucene_index::SegmentInfos::SegmentInfos(const SegmentInfos& si){
   this->dir=si.dir;
   namecounter=si.namecounter;
   generation=si.generation;
   docCount=si.docCount;
   sivector=new vector<SegmentInfo>(*(si.sivector));
}
lucene_index::SegmentInfos::~SegmentInfos(){
  delete sivector;
}
SegmentInfo* lucene_index::SegmentInfos::info(int i){
    SegmentInfo& tmp=sivector->at(i);
	return &tmp;
}
SegmentInfo* lucene_index::SegmentInfos::info(wstring name){
	vector<SegmentInfo>::iterator iter;
	for(iter=sivector->begin();iter!=sivector->end();iter++){
         wstring tmp_name=iter->name;
		 if(name==(tmp_name)){
              return (SegmentInfo*)&((*iter));
		 }
	}
	return NULL;
}
void lucene_index::SegmentInfos::put(SegmentInfo& si){
       sivector->push_back(si);
	   docCount+=si.docCount;
}
void lucene_index::SegmentInfos::setInfo(int pos,SegmentInfo* si){
      vector<SegmentInfo>::iterator iter;
	  iter=sivector->begin();
	  iter+=pos;
      sivector->insert(iter,*si);
	  delete si;
}
bool lucene_index::SegmentInfos::remove(int i){

	   if(i<0 || i>=this->size()){
            return false;
	   }else{
            vector<SegmentInfo>::iterator iter=sivector->begin();
			iter+=i;
			docCount-=iter->docCount;
			sivector->erase(iter);
			return true;
	   }
}
int  lucene_index::SegmentInfos::size(){
         return sivector->size();
}
void lucene_index::SegmentInfos::clear(){
         sivector->clear();
}
void lucene_index::SegmentInfos::SISread(){
        wstring segfilename=getSegFile();
        SISread(segfilename);
}
void lucene_index::SegmentInfos::SISread(wstring& wstr){
      wstring file=dir+wstr;

	  char* filename=wstrToChars(file);
	  FSIndexInput* fsii=new FSIndexInput(filename);

	  int segnum;
	  fsii->readInt(&segnum);
	  //int str_end=wstr.find_last_of(IndexFileNames::SEGMENTS);

	  //wstring tmp_dir=wstr.substr(0,str_end);

	  for(int i=0;i<segnum;i++){
          SegmentInfo* si=new SegmentInfo(dir,fsii);
		  put(*si);
		  delete si;
	  }
	  delete fsii;
}
void lucene_index::SegmentInfos::SISwrite(wstring& wstr){

	  char* filename=wstrToChars(wstr);

      FSIndexOutput* fsio=new FSIndexOutput(filename);

	  /*
	  if(generation == -1) {
          generation = 1;
      } else {
         generation++;
      }
      */

	  fsio->writeInt(this->size());

	  for (int i = 0; i < size(); i++) {
        info(i)->write(fsio);
      }
      delete fsio;
}
void lucene_index::SegmentInfos::addAll(SegmentInfos* sis){
	int size=sis->size();

	for(int i=0;i<size;i++){
        SegmentInfo* si=sis->info(i);
		this->put(*si);
	}
}
long lucene_index::SegmentInfos::getGen(){
    vector<wstring>* tmp_files=new vector<wstring>();
       long gen;
       char* dirname=wstrToChars(dir);

       DIR* mydir=opendir(dirname);
       struct dirent* ptr;
	   if(mydir==NULL){
          wcout<<"error when open the dir  "<<dirname<<endl;
		  return 0;
	   }
	   while((ptr=readdir(mydir))!=NULL){
            if(ptr->d_type==DT_REG){
				 wstring filename=charToWstring(ptr->d_name);
                 tmp_files->push_back(filename);
			}
			//delete ptr;
	   }
	   closedir(mydir);

	   gen=getGen(tmp_files);

       delete tmp_files;
	   delete[] dirname;

	   return gen;
}
long lucene_index::SegmentInfos::getGen(vector<wstring>* files){
	int maxgen=0;

	  vector<wstring>::iterator iter;

	   for(iter=files->begin();iter!=files->end();iter++){
              if(iter->find(IndexFileNames::SEGMENTS,0)!=-1&&(*iter)!=IndexFileNames::SEGMENTS_GEN){
                     int seg_size=(IndexFileNames::SEGMENTS).size();
					 int str_len=iter->size();
					 wstring str_gen=iter->substr(seg_size+1,str_len-1);
					 long gen=wstrToLong(str_gen);

					 if(gen>maxgen){
                          maxgen=gen;
					 }
			  }
	   }
	   return maxgen;
}
wstring lucene_index::SegmentInfos::getSegFile(){
	long gen=getGen();
     wstring gen_str=LongToWstring(gen);

	 wstring segmentFileName=(IndexFileNames::SEGMENTS)+L"_"+gen_str;
     return segmentFileName;
}
