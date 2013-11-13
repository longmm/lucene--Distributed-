#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace document;
using namespace store;

lucene_index::MultiLevelSkipListReader::MultiLevelSkipListReader(wstring dir,wstring name,int skipInterval,int maxSkipLevels){
         wstring file=dir+name+IndexFileNames::D+IndexFileNames::FRQ;
         char* filename=wstrToChars(file);
         fsii=new FSIndexInput(filename);
         this->skipInterval=skipInterval;
         this->maxSkipLevels=maxSkipLevels;
         levels_len=new vector<int>();
}
lucene_index::MultiLevelSkipListReader::~MultiLevelSkipListReader(){
       delete fsii;
       delete levels_len;
}
void lucene_index::MultiLevelSkipListReader::reset(long skipp){
         this->skipp=skipp;
         fsii->seek(skipp);
         int levels_size=0;
         fsii->readInt(&levels_size);

         levels_len->clear();

         for(int i=0;i<levels_size;i++){
             int tmp=0;
             fsii->readInt(&tmp);
             levels_len->push_back(tmp);
         }
         this->skipp=fsii->getFilePointer();
         //fsii->readInt(&docCount);
         //fsii->readInt(&indexDocCount);
}
int lucene_index::MultiLevelSkipListReader::skipTo(int target){
    return  skipIndex(0,0,target,skipp);
}
int lucene_index::MultiLevelSkipListReader::skipIndex(int level,int start,int target,long pos){

    int tmp_docid1=0;
    long tmp_freq1=0;
    long tmp_prox1=0;
    long childp1=0;

    int tmp_docid2=0;
    long tmp_freq2=0;
    long tmp_prox2=0;
    long childp2=0;

    fsii->seek(pos);

    int level_size=levels_len->at(level);

    for(int i=start;i<level_size;i++){
        tmp_docid1=tmp_docid2;
        tmp_freq1=tmp_freq2;
        tmp_prox1=tmp_prox2;
        childp1=childp2;

        fsii->readInt(&tmp_docid2);
        fsii->readLong(&tmp_freq2);
        fsii->readLong(&tmp_prox2);
        fsii->readLong(&childp2);

        if(target==tmp_docid2){
            docid=tmp_docid2;
            freqp=tmp_freq2;
            proxp=tmp_prox2;
            if(childp2==-1){
                return i*skipInterval;
            }
            return skipIndex(level+1,(i)*skipInterval,target,childp2);
        }

        if(target<tmp_docid2){

             if(childp2==-1){
                 docid=tmp_docid1;
                 freqp=tmp_freq1;
                 proxp=tmp_prox1;
                 return i*skipInterval;
             }
            return skipIndex(level+1,(i-1)*skipInterval,target,childp1);
        }else if(target>tmp_docid2){
            continue;
        }
    }
    if(childp2==-1){
       docid=tmp_docid2;
       freqp=tmp_freq2;
       proxp=tmp_prox2;
       return (level_size-1)*skipInterval;
    }
    return skipIndex(level+1,(level_size-1)*skipInterval,target,childp2);
}
