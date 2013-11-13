#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_util;
using namespace store;
using namespace document;

lucene_index::SegmentMerger::SegmentMerger(wstring dir,wstring name){
    this->dir=dir;
	this->name=name;
    srs=new vector<SegmentReader*>();
	wstring filename=dir+name;
	fis=new FieldInfos();
	docCount=0;
	delCount=0;
}
void lucene_index::SegmentMerger::add(SegmentReader* sr){
    docCount+=sr->docCount;
    delCount+=sr->delCount;
    srs->push_back(sr);
}
lucene_index::SegmentMerger::~SegmentMerger(){
    int srs_size=srs->size();

    for(int i=0;i<srs_size;i++){
        SegmentReader* sr=srs->at(i);
        delete sr;
    }
    delete srs;
	delete fis;
	delete freq_fsio;
	delete prox_fsio;
    delete mwriter;
    delete tisw;
}
void lucene_index::SegmentMerger::mergeFieldsData(){
  wstring file_fdx=dir+name+IndexFileNames::D+IndexFileNames::FDX;
  char* filename_fdx=wstrToChars(file_fdx);
  FSIndexOutput* fsio_fdx=new FSIndexOutput(filename_fdx);

  wstring file_fdt=dir+name+IndexFileNames::D+IndexFileNames::FDT;
  char* filename_fdt=wstrToChars(file_fdt);
  FSIndexOutput* fsio_fdt=new FSIndexOutput(filename_fdt);


  int ssize=srs->size();
  if(ssize==0){
    return ;
  }
  long* tmp_fdx=new long[docCount];
  tmp_fdx[0]=0;
   int total=0;
  for(int i=0;i<ssize;i++){
    SegmentReader* sr=srs->at(i);
    int docNum=sr->docCount;
     for(int j=0;j<docNum;j++){
        long fdx_pos=fsio_fdt->getFilePointer();
        tmp_fdx[total+j]=fdx_pos;
        Document* doc=sr->getDoc(j);
        int fsize=doc->getFields()->size();
        fsio_fdt->writeInt(fsize);

         for(int x=0;x<fsize;x++){
           Field* f=doc->getField(x);
           wstring value=f->value;
           wstring name=f->name;

           int pos=fis->fieldNum(name);

           fsio_fdt->writeInt(pos);
           fsio_fdt->writeWString(value);
         }
         delete doc;
     }
      total+=docNum;
  }
  for(int i=0;i<docCount;i++){
      fsio_fdx->writeLong(tmp_fdx[i]);
  }
  delete fsio_fdt;
  delete fsio_fdx;
  delete[] tmp_fdx;
}
void lucene_index::SegmentMerger::mergeMetaFields(){
	int ssize=srs->size();
    for(int i=0;i<ssize;i++){
        SegmentReader* sr=srs->at(i);
		if(sr->fis!=NULL){
			fis->add(sr->fis);
		}
	}
	wstring fnm=dir+name+IndexFileNames::D+IndexFileNames::FIELD_INFO;
	fis->writeFields(fnm);
}
void lucene_index::SegmentMerger::mergeNorms(){
     int fsize=fis->size();

     wstring norm=dir+name+IndexFileNames::D+IndexFileNames::NORMS_EXTENSION;
     char* normfile=wstrToChars(norm);
     FSIndexOutput* fsio=new FSIndexOutput(normfile);

	 for(int i=0;i<fsize;i++){
        FieldInfo* fi=fis->fieldInfo(i);
		wstring name=fi->name;

		int ssize=srs->size();
		for(int j=0;j<ssize;j++){
            SegmentReader* sr=srs->at(j);
			int maxdoc=sr->docCount;
            float* norm_floats=sr->getnorms(name);
            if(norm_floats==NULL){
               for(int x=0;x<maxdoc;x++){
               fsio->writeFloat(0.0f);
             }

            }else{
              for(int x=0;x<maxdoc;x++){
                 fsio->writeFloat(norm_floats[x]);
               }
            }

		}
	}

    delete fsio;
}
void lucene_index::SegmentMerger::mergeDel(){
    wstring file=dir+name+IndexFileNames::D+IndexFileNames::DELETES_EXTENSION;
    char* filename=wstrToChars(file);
    FSIndexOutput* fsio=new FSIndexOutput(filename);

    //fsio->writeInt(delCount);

    int ssize=srs->size();
    if(ssize==0){
       delete fsio;
       return;
    }
    int total=0;
    for(int i=0;i<ssize;i++){
       SegmentReader* sr=srs->at(i);
       int docNum=sr->docCount;
       bool* isDel=sr->isDel;
       for(int j=0;j<docNum;j++){
             if(isDel[j]==true){
                fsio->writeInt(total+j);
             }

       }
       total+=docNum;
    }
    delete fsio;
}
void lucene_index::SegmentMerger::merge(){
      mergeDel();
      mergeMetaFields();
      mergeNorms();
      mergeFieldsData();
      mergeTerms();
}
void lucene_index::SegmentMerger::mergeTerms(){
     wstring file_frq=dir+name+IndexFileNames::D+IndexFileNames::FRQ;
     char* filename_frq=wstrToChars(file_frq);
     freq_fsio=new FSIndexOutput(filename_frq);

     wstring file_pro=dir+name+IndexFileNames::D+IndexFileNames::PRX;
     char* filename_prox=wstrToChars(file_pro);
     prox_fsio=new FSIndexOutput(filename_prox);

     tisw=new TermInfosWriter(this->dir,this->name,this->fis,MUL_SKIPINTERVAL);
     mwriter=new MultiLevelSkipListWriter(MUL_SKIPINTERVAL,MAXSKIPLEVELS,freq_fsio,prox_fsio);


     mergeTermInfos();
}
void lucene_index::SegmentMerger::mergeTermInfos(){
    int s_size=srs->size();
    vector<SegmentMergeInfo*>* smergeInfos=new vector<SegmentMergeInfo*>();
    int total=0;

    for(int i=0;i<s_size;i++){
        SegmentReader* sr=srs->at(i);
        sr->init_V_Term();
        SegmentMergeInfo* smi=new SegmentMergeInfo(total,sr->termdocs,sr->termposs);
        smergeInfos->push_back(smi);
        total+=sr->docCount;
    }

    while(smergeInfos->size()>0){
        int n=0;
        n=sort_segmentmergeinfo(smergeInfos);
        mergeTermInfo(smergeInfos,n);
        stepToNextTerm(smergeInfos,n);
    }

    delete smergeInfos;
}
int lucene_index::SegmentMerger::sort_segmentmergeinfo(vector<SegmentMergeInfo*>* smergeInfos){
    if(smergeInfos->size()==0){
       return 0;
    }
    sort(smergeInfos->begin(),smergeInfos->end(),ComSMI());

    SegmentMergeInfo* top_smi=smergeInfos->at(0);
    TermInfo* top_ti=top_smi->termDocs->cti;
    int n=0;
    int ssize=smergeInfos->size();

    for(int i=0;i<ssize;i++){
        SegmentMergeInfo* tmp_smi=smergeInfos->at(i);
        TermInfo* tmp_ti=tmp_smi->termDocs->cti;
        if((*top_ti)==(*tmp_ti)){
           n++;
        }else{
           break;
        }
    }
    return n;
}
void lucene_index::SegmentMerger::stepToNextTerm(vector<SegmentMergeInfo*>* smergeInfos,int n){

    vector<SegmentMergeInfo*>::iterator iter;
    int i=0;
    for(iter=smergeInfos->begin();i<n&&iter!=smergeInfos->end();i++,iter++){
        SegmentMergeInfo* smi=(*iter);
        bool flag=smi->nextTerm();
        if(flag==true){
           continue;
        }else{
            delete smi;
            smergeInfos->erase(iter);
            iter--;
        }
    }
}
void lucene_index::SegmentMerger::mergeTermInfo(vector<SegmentMergeInfo*>* smergeInfos,int n){
       long freqp=freq_fsio->getFilePointer();
       long proxp=prox_fsio->getFilePointer();

       int df=appendPostings(smergeInfos,n);

       long skipp=mwriter->writeSkip(freq_fsio);

       if(df>0){

          SegmentMergeInfo* smi=smergeInfos->at(0);

          wstring fname=smi->termDocs->cti->fname;
          wstring fvalue=smi->termDocs->cti->fvalue;

          Term tmp_t(fname,fvalue);

          TermInfo ti(df,freqp,proxp,skipp,fname,fvalue);

          tisw->add(&tmp_t,&ti);
       }
}
int lucene_index::SegmentMerger::appendPostings(vector<SegmentMergeInfo*>* smergeInfos,int n){

    mwriter->resetSkip();

    int msize=n;
    int pos=0;
    for(int i=0;i<msize;i++){
         SegmentMergeInfo* smif=smergeInfos->at(i);
         while(smif->next()){
              int docid=smif->getDocId();
              int freq=smif->getFreq();
              vector<int>* tmp_poss=smif->getPoss();

              if(pos%MUL_SKIPINTERVAL==0){
                    mwriter->setSkipData(docid,pos);
              }
              pos++;

              freq_fsio->writeInt(docid);
              freq_fsio->writeInt(freq);

              int prox_size=tmp_poss->size();

              prox_fsio->writeInt(docid);
              prox_fsio->writeInt(prox_size);

              for(int i=0;i<prox_size;i++){
                  int tmp_po=tmp_poss->at(i);
                  prox_fsio->writeInt(tmp_po);
              }

         }

    }
    return pos;
}
