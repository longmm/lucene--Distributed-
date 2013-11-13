#include "index.h"

using namespace std;
using namespace lucene_index;
using namespace lucene_search;
using namespace store;
using namespace lucene_util;
using namespace document;

lucene_index::DocumentWriter::DocumentWriter(wstring dir){
     this->dir=dir;
     posts=new map<Term,Posting>();
     pos=0;
}
lucene_index::DocumentWriter::~DocumentWriter(){
     delete[] fieldBoosts;
     delete fieldInfos;
     delete[] fieldLengths;
     delete[] fieldOffsets;
     delete posts;
}
void lucene_index::DocumentWriter::addDocument(wstring segment, Document* doc){
     fieldInfos=new FieldInfos();
     fieldInfos->add(doc);
	 fsize=fieldInfos->size();

	 fieldLengths = new int[fsize];
	 for(int i=0;i<fsize;i++){
         fieldLengths[i]=0;
	 }

	 fieldOffsets = new int[fsize];
     for(int i=0;i<fsize;i++){
        fieldOffsets[i]=0;
     }


	 fieldBoosts=new float[fsize];

	 for(int i=0;i<fsize;i++){
          Field* f=doc->getField(i);
          fieldBoosts[i]=f->boost;
	 }
	 writeDel(segment);
     writeFieldsMetaData(segment);

	 writeFieldsData(segment,doc);
	 invertDocument(doc);
	 writePositings(segment);

     writeNorms(segment,doc->getBoost());
}
void lucene_index::DocumentWriter::writeNorms(wstring segment,float docBoost){

	 wstring file=dir+segment+IndexFileNames::D+IndexFileNames::NORMS_EXTENSION;

	 char* filename=wstrToChars(file);

	 FSIndexOutput* fsio=new FSIndexOutput(filename);
	for(int i=0;i<fsize;i++){
        FieldInfo* fi = fieldInfos->fieldInfo(i);
		if(fi->isIndexed){
			float norm = docBoost*fieldBoosts[i]*Similarity::lengthNorm(fieldLengths[i]);
            fsio->writeFloat(norm);
		}
	}
	delete fsio;
}
void lucene_index::DocumentWriter::writeFieldsData(wstring segment,Document* doc){
     wstring file1=dir+segment+IndexFileNames::D+IndexFileNames::FDX;
     char*  filename1=wstrToChars(file1);
     FSIndexOutput* fsio1=new FSIndexOutput(filename1);
     fsio1->writeLong(0);
     delete fsio1;


     wstring file2=dir+segment+IndexFileNames::D+IndexFileNames::FDT;
     char* filename2=wstrToChars(file2);
     FSIndexOutput* fsio2=new FSIndexOutput(filename2);

      set<Field>* fields=doc->getFields();
      int fsize=fields->size();

       if(fsize==0){
          delete fsio2;
          return ;
       }

       fsio2->writeInt(fsize);

       for(int i=0;i<fsize;i++){
           Field* f=doc->getField(i);
           wstring value=f->value;
           fsio2->writeInt(i);
           fsio2->writeWString(value);
       }
     delete fsio2;
}
void lucene_index::DocumentWriter::writeFieldsMetaData(wstring segment){
    wstring file=dir+segment+IndexFileNames::D+IndexFileNames::FIELD_INFO;
    fieldInfos->writeFields(file);
}
void lucene_index::DocumentWriter::writeDel(wstring segment){
    wstring file=dir+segment+IndexFileNames::D+IndexFileNames::DELETES_EXTENSION;
    char* filename=wstrToChars(file);
    FSIndexOutput* fsio=new FSIndexOutput(filename);
    //fsio->writeInt(0);
    delete fsio;
}
void lucene_index::DocumentWriter::invertDocument(Document* doc){
     set<Field>* fields=doc->fields;
     if(fields==NULL|| fields->size()==0){
         return;
     }
     set<Field>::iterator iter;

     int x=0;
     int y=0;

     for(iter=fields->begin();iter!=fields->end();iter++){
         wstring fieldname=iter->name;
         wstring fieldvalue=iter->value;
         if(iter->indexed){
             if(iter->tokenized){
                 int vsize=fieldvalue.size();
                  for(int i=0;i<vsize;i++){
                      wchar_t tmp=fieldvalue.at(i);
                      wstring tmp_str(1,tmp);

                      if(StopWords::isStops(tmp_str)){
                          continue;
                      }

                      Term tmp_t(fieldname,tmp_str);
                      addPosition(tmp_t);
                      y++;
                  }
             }else{
                 Term tmp_t(fieldname,fieldvalue);
                 addPosition(tmp_t);
                 y++;
             }
         }
         fieldLengths[x]=y;
         x++;
         y=0;
     }
}
void lucene_index::DocumentWriter::addPosition(Term t){
    map<Term,Posting>::iterator iter;
    iter=posts->find(t);
    if(iter==posts->end()){
       Posting tmp_p(t);
       tmp_p.add(pos);
       pair<Term,Posting> tmp_pair=make_pair(t,tmp_p);
       posts->insert(tmp_pair);
    }else{
       (iter->second).add(pos);
    }
    pos++;
}
void lucene_index::DocumentWriter::writePositings(wstring segment){
    TermInfosWriter* tisw=new TermInfosWriter(dir,segment,fieldInfos,TERM_SKIPINTERVAL);
    wstring file_frq=dir+segment+IndexFileNames::D+IndexFileNames::FRQ;
    wstring file_prx=dir+segment+IndexFileNames::D+IndexFileNames::PRX;

    char* filename_frq=wstrToChars(file_frq);
    FSIndexOutput* fsio_frq=new FSIndexOutput(filename_frq);

    char* filename_prx=wstrToChars(file_prx);
    FSIndexOutput* fsio_prx=new FSIndexOutput(filename_prx);

    MultiLevelSkipListWriter* mwriter=new MultiLevelSkipListWriter(MUL_SKIPINTERVAL,MAXSKIPLEVELS,fsio_frq,fsio_prx);

    map<Term,Posting>::iterator iter;

    for(iter=posts->begin();iter!=posts->end();iter++){

        mwriter->resetSkip();

        Term t=iter->first;
        wstring field=(iter->first).field;
        wstring value=(iter->first).text;
        int df=1;
        long freqp=fsio_frq->getFilePointer();
        long proxp=fsio_prx->getFilePointer();

        int tf=(iter->second).freq;
        vector<int>* positions=(iter->second).positions;

        mwriter->setSkipData(0,0);


        fsio_frq->writeInt(0);
        fsio_frq->writeInt(tf);

        long skipp=fsio_frq->getFilePointer();
        TermInfo ti(df,freqp, proxp,skipp,field,value);
        tisw->add(&t,&ti);


        int pos_size=positions->size();
        fsio_prx->writeInt(0);
        fsio_prx->writeInt(pos_size);

        for(int i=0;i<pos_size;i++){
            int pos=positions->at(i);
            fsio_prx->writeInt(pos);
        }

        mwriter->writeSkip(fsio_frq);
    }
    delete tisw;
    delete fsio_frq;
    delete fsio_prx;
    delete mwriter;

}
