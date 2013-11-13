#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::MultiLevelSkipListWriter::MultiLevelSkipListWriter(int  skipInterval,int maxSkipLevels,FSIndexOutput* freq_fsio,FSIndexOutput* prox_fsio){
  this->skipInterval=skipInterval;
  //this->df=df;
  this->maxSkipLevels=maxSkipLevels;

  this->prox_fsio=prox_fsio;
  this->freq_fsio=freq_fsio;
  skipCells=new vector<SkipCell>[maxSkipLevels];
}
void lucene_index::MultiLevelSkipListWriter::resetSkip(){
    for(int i=0;i<maxSkipLevels;i++){
       skipCells[i].clear();
    }
}
lucene_index::MultiLevelSkipListWriter::~MultiLevelSkipListWriter(){
   delete[] skipCells;
}
void lucene_index::MultiLevelSkipListWriter::setSkipData(int docid,int pos){
     this->cdocid=docid;
     cfreqp=freq_fsio->getFilePointer();
     cproxp=prox_fsio->getFilePointer();
     bufferSkip(pos);
}
void lucene_index::MultiLevelSkipListWriter::bufferSkip(int pos){
     int numLevels;

     for(numLevels=0;(pos%skipInterval)==0&&numLevels<maxSkipLevels;pos/=skipInterval){
         numLevels++;
     }

    int childp=0;
    for(int level=0;level<numLevels;level++){

        if(level==0){
             SkipCell sc(cdocid,cfreqp,cproxp,-1);
             skipCells[level].push_back(sc);
             childp=skipCells[level].size()-1;
        }else{
             SkipCell sc(cdocid,cfreqp,cproxp,childp);
             skipCells[level].push_back(sc);
             childp=skipCells[level].size()-1;
        }

    }
}
long lucene_index::MultiLevelSkipListWriter::writeSkip(FSIndexOutput* fsio){
     long startp=fsio->getFilePointer();
     int totalcells=0;
      if(skipCells==NULL){
          return startp;
      }

      int levels;
      for(levels=0;levels<maxSkipLevels;levels++){
         int sc_size=skipCells[levels].size();
         if(sc_size==0){
            break;
         }
      }

      fsio->writeInt(levels);

      for(int i=levels-1;i>=0;i--){
         int sc_size=skipCells[i].size();
         fsio->writeInt(sc_size);
      }

      for(int i=levels-1;i>=0;i--){
         int sc_size=skipCells[i].size();
         if(sc_size==0){
            continue;
         }

         for(int j=0;j<sc_size;j++){
             SkipCell sc=skipCells[i].at(j);
             fsio->writeInt(sc.docid);
             fsio->writeLong(sc.frqp);
             fsio->writeLong(sc.proxp);
             if(i==0){
               fsio->writeLong(-1);
             }else{
                long childp=fsio->getFilePointer()+4 +(sc_size-j-1)*16+(sc.childp)*16;
                fsio->writeLong(childp);
             }
         }


      }
      return startp;
}



