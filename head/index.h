#ifndef MY_INDEX
#define MY_INDEX

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>


#include "document.h"
#include "store.h"
#include "lucene_util.h"
#include "lucene_search.h"

#define MERGE_FACTOR 2
#define MIN_MERGE 10
#define TERM_SKIPINTERVAL 16
#define MUL_SKIPINTERVAL 4
#define MAXSKIPLEVELS 4

namespace lucene_search{};

using namespace std;
using namespace store;
using namespace document;
using namespace lucene_search;
using namespace lucene_util;

namespace lucene_index{

class SegmentTermDocs;
class SegmentTermPositions;
class TermInfosReader;
class TermInfosWriter;
class MultiLevelSkipListWriter;
class MultiLevelSkipListReader;
class SegmentMergeInfo;
class MTermDocs;


class Term
{
    public:
		wstring field;
	    wstring text;

        Term();
		Term(wstring f,wstring t);
	    ~Term();
		Term(const Term& t);
        bool operator<(const Term& t)const;
		bool operator==(const Term& t)const;
		bool operator=(const Term& t);
		wstring getField()const;
		void setTerm(wstring f,wstring t);
		wstring getText()const;
};
class TermInfo
{
   public:
       wstring fname;
       wstring fvalue;
	   int docFreq;
       long freqPointer;
	   long proxPointer;
	   long skipPointer;

	   TermInfo();
	   ~TermInfo();
       TermInfo(int,long,long,long,wstring fname,wstring fvalue);
	   TermInfo(TermInfo& tf);
	   void setInfo(int,long,long,long,wstring fname,wstring fvalue);
	   void setInfo(TermInfo& t);
	   void operator=(const TermInfo& ti);
	   bool operator<(const TermInfo& ti);
	   bool operator==(const TermInfo& ti);
};
class FieldInfo
{
	 public:
        wstring name;
	    bool isIndexed;
        int number;
        FieldInfo(wstring fn,bool isi,int num);
		FieldInfo(const FieldInfo& fi);
		void operator=(const FieldInfo& fi);
		~FieldInfo();
};
class FieldInfos
{
    private:
         vector<FieldInfo>* byNumber;
	     map<wstring,FieldInfo>* byName;
    public:
		  static const char IS_INDEXED=0x1;
	      FieldInfos();
	      FieldInfos(wstring filename);
		  ~FieldInfos();
		  FieldInfo* addInternal(wstring,bool isIndexed);
		  FieldInfo* fieldInfo(int fieldNumber);
		  FieldInfo* fieldInfo(wstring fieldname);
		  wstring    fieldName(int fieldNumber);
		  void        init();
		  void        add(Document* );
		  void        add(wstring name,bool isIndexed);
		  void        add(FieldInfos*);
		  int         fieldNum(wstring fieldname);
		  void        readFields(IndexInput*);
		  wstring    readFieldNames(IndexInput*);
		  int         size();
		  void        writeFields(IndexOutput*);
		  void        writeFields(wstring filename);
		  void        writeFieldName(IndexOutput* , wstring);
};
class SegmentInfo
{
    public:
		wstring name;
	    int docCount;
		wstring dir;
		vector<wstring>* files;
		long delGen;
		bool isTMP;
		static int YES;
		static int NO;

        SegmentInfo(wstring name,int docCount,wstring dir,bool isTMP);
	    SegmentInfo(const SegmentInfo& si);
		SegmentInfo(wstring,IndexInput*);
		~SegmentInfo();
        void operator=(const SegmentInfo& si);
		vector<wstring>* getFiles();
		vector<wstring>* obfiles();
		wstring getDelFileName();
		wstring getNormFileName();
		void write(IndexOutput* io);
		//void advanceDelGen();
		//void clearDelGen();
		bool fileisExisted(wstring name);
		void deleteFiles();
};
class SegmentInfos
{
	public:
		  SegmentInfos(wstring);
	      SegmentInfos(const SegmentInfos&);
	      ~SegmentInfos();

		  SegmentInfo* info(int i);
		  SegmentInfo* info(wstring name);
		  void setInfo(int,SegmentInfo*);
		  int  size();
		  void put(SegmentInfo& si);
		  bool remove(int);
          void clear();
		  void addAll(SegmentInfos* sis);

		  void SISread();
		  void SISread(wstring&);
		  void SISwrite(wstring&);

		  long getGen();
          long getGen(vector<wstring>*);
		  wstring getSegFile();

		  long namecounter;
		  long generation;
		  int docCount;
		  vector<SegmentInfo>* sivector;
		  wstring dir;
};
/*
class FindSegmentsFile
{
    public:
		wstring dir;
	    FindSegmentsFile(wstring dir);
		~FindSegmentsFile();
		long getGen();
		long getGen(vector<wstring>*);
		wstring getSegFile();
};
*/
class IndexFileNames
{
     public:
		 static wstring SEGMENTS;
	     static wstring SEGMENTS_GEN;
		 static wstring DELETABLE;
		 static wstring NORMS_EXTENSION;
		 static wstring COMPOUND_FILE_EXTENSION;
		 static wstring DELETES_EXTENSION;
		 static wstring PLAIN_NORMS_EXTENSION;
		 static wstring SEPARATE_NORMS_EXTENSION;
		 static wstring FIELD_INFO;
		 static wstring D;
		 static wstring FDT;
		 static wstring FDX;
		 static wstring TII;
		 static wstring TIS;
		 static wstring FRQ;
		 static wstring PRX;
		 static vector<wstring> INDEX_EXTENSIONS_IN_COMPOUND_FILE;
         static bool init_flag;
		 static vector<wstring>* getIEICF();
};
/*
class CommitPoint
{
    public:
		long gen;
	    vector<wstring>* files;
		bool deleted;
		wstring* segmentsFileName;


		CommitPoint(SegmentInfos*);
		CommitPoint(CommitPoint&);
		~CommitPoint();
		wstring getSegmentsFileName();
		void deleteCommitPoint();
		bool operator<(const CommitPoint& a);
		bool operator==(const CommitPoint& a);
};
*/

class IndexFileDeleter{
	public:
	   static wstring dir;
	   static map<wstring,int> refCounts;

	   static void setDir(wstring dir);
	   static void checkpoint(SegmentInfos*);
	   static void decRef(SegmentInfos*);
	   static void deleteFile(wstring);
	   static void incRef(SegmentInfos*);
	   static void deleteSegment(SegmentInfo*);
	   static void deleteSegment(wstring sname,SegmentInfo* si);
};
class IndexWriter
{
       public:
		   int maxMergeDocs;
	       int mergeFactor;
		   int minMergeDocs;
           SegmentInfos* sis;
           //SegmentInfos* oldsis;

		   FSLock* fslock;
		   wstring dir;
		   long oldgeration;

           IndexWriter(wstring);
		   ~IndexWriter();
		   void init();
		   //void abort();
		   void addDocument(Document* doc);
		   SegmentInfo* buildSingleDocSegment(Document* doc);
		   void commitTransaction();
		   void rollbackTransaction();
		   int  docCount();
		   void flush();
		   void maybeMergeSegments(int);
		   int mergeSegments(int,int);
		   void removeSegment(int,int);
		   void deleteSegment(int);
		   wstring newTmpSegmentName();
		   wstring newSegmentName();
		   void renameSegment();
		   void renameSegmentFiles(wstring newfile,wstring oldfile);
		   bool commit();
		   void deleteDocument(int num);
};
class Norm
{
	 public:
        float* norm_floats;
	    long num;
	    Norm(float*,int);
		Norm(const Norm&);
		void operator=(const Norm&);
		~Norm();
};
class SegmentReader
{
    public:
		wstring name;
	    wstring dir;
		int docCount;
		int delCount;
		FieldInfos* fis;
		FSIndexInput* fdt;
		long* fdx;
		bool* isDel;
		map<wstring,Norm>* norms_map;
        vector<int>* dels;

        SegmentTermDocs* termdocs;
        SegmentTermPositions* termposs;

		SegmentReader(wstring name,wstring dir,int docCount);
	    ~SegmentReader();
        float* getnorms(wstring);
        void getAllNorms();
		void init_fdx();
		void init_del();
		void delDoc(int i);
        //void writeDel();
		Document* getDoc(int);
		bool isDeleted(int i);
		bool init_V_Term();
		void resetDocPos();
};
class SegmentMerger
{
	public:
	wstring dir;
	wstring name;
	int docCount;
	int delCount;
	vector<SegmentReader*>* srs;
    FieldInfos* fis;

    FSIndexOutput* freq_fsio;
    FSIndexOutput* prox_fsio;
    TermInfosWriter* tisw;
    MultiLevelSkipListWriter* mwriter;

	SegmentMerger(wstring dir,wstring name);
	void add(SegmentReader* sr);
	void mergeMetaFields();
	void mergeFieldsData();
	void mergeNorms();
	void mergeDel();
	void merge();
	void mergeTerms();
	void mergeTermInfos();
	void mergeTermInfo(vector<SegmentMergeInfo*>* smergeInfos,int n);
	int appendPostings(vector<SegmentMergeInfo*>* smergeInfos,int n);
	void stepToNextTerm(vector<SegmentMergeInfo*>* smergeInfos,int n);
	int sort_segmentmergeinfo(vector<SegmentMergeInfo*>* smergeInfos);
	~SegmentMerger();
};
class IndexReader
{
	public:
		int docCount;
	    int delCount;
		int* starts;
		vector<SegmentReader*>* srs;
		SegmentInfos* sis;
		wstring dir;
		IndexReader(wstring dir);
		~IndexReader();
		void init();
		Document* getDoc(int);
		bool isDeleted(int);
		int readerIndex(int);

		int docid;
		int freq;
		vector<int>* poss;
		int df;
		int del_df;
		int* isCur; //0 isCur 1 isNotCur 2 isTail
        Term* curT;
        int pos;


        bool next();
        bool nextTerm();
        int read(int* docs,int*freqs,int len);
        bool seek(Term* t);
        void seek(TermInfo* tf,Term* t);
        bool skipTo(int target);
        bool init_V_Term();
        int getDocID();
        int getFreq();
        vector<int>* getPoss();
        void Comput_df();
        void resetDocPos();
        void selectTerm();
        MTermDocs* getMTDocs();
};
class Posting{
      public:
         Term term;
         int freq;
         vector<int>* positions;
         Posting(Term t);
         Posting(const Posting& p);
         ~Posting();
         void add(int pos);
         bool operator<(const Posting& p);
         bool operator==(const Posting& p);
         void operator=(const Posting& p);
};
class DocumentWriter
{
   public:
	   wstring dir;
       float* fieldBoosts;
	   FieldInfos* fieldInfos;
	   int* fieldLengths;
	   int* fieldOffsets;
	   int maxFieldLength;
	   int fsize;

	   map<Term,Posting>* posts;
	   int pos;

	   DocumentWriter(wstring dir);
	   ~DocumentWriter();
	   void addDocument(wstring segment, Document* doc);
	   int getNumFields();
	   void writeNorms(wstring segment,float docBoost);
	   void writeFieldsData(wstring segment, Document* doc);
	   void writeFieldsMetaData(wstring segment);
	   void writeDel(wstring segment);
	   void invertDocument(Document* doc);
	   void writePositings(wstring segment);
	   void addPosition(Term t);
};
class SkipCell{
    public:
        int docid;
        long frqp;
        long proxp;
        int childp;
        SkipCell();
        SkipCell(int docid,long frqp,long proxp,int childp);
        SkipCell(const SkipCell& sc);
        void operator=(const SkipCell& sc);
};
class MultiLevelSkipListWriter{
       public:
           long cfreqp;
           long cproxp;
           int cdocid;
           FSIndexOutput* freq_fsio;
           FSIndexOutput* prox_fsio;

           int skipInterval;
           int maxSkipLevels;
           //int df;

           vector<SkipCell>* skipCells;
           MultiLevelSkipListWriter(int skipInterval,int maxSkipLevels,FSIndexOutput* freq_fsio,FSIndexOutput* prox_fsio);
           ~MultiLevelSkipListWriter();
           void resetSkip();
           // void writeSkipData(int level,SkipCell* sc);
           void setSkipData(int docid,int pos);
           void bufferSkip(int pos);
           long writeSkip(FSIndexOutput* fsio);
};
class MultiLevelSkipListReader{
      public:
          //int docCount;
          //int indexDocCount;
          int maxSkipLevels;
          int docid;
          int skipInterval;

          long skipp;
          long freqp;
          long proxp;
          FSIndexInput* fsii;
          vector<int>* levels_len;

          MultiLevelSkipListReader(wstring dir,wstring name,int skipInterval,int maxSkipLevels);
          ~MultiLevelSkipListReader();
          void reset(long skipp);
          int skipTo(int target);
          int skipIndex(int level,int start,int target,long pos);
};
class TermInfosWriter{

    public:
       FieldInfos* fis;
       FSIndexOutput* tii;
       FSIndexOutput* tis;
       int skipinterval;
       int termCount;

       TermInfosWriter(wstring dir,wstring name,FieldInfos* fis,int skipinterval);
       ~TermInfosWriter();
       void init();
       void add(Term* t,TermInfo* tf);
       void writeTerm(Term* t);
};
class TermInfosReader{
     public:
         FSIndexInput* fsii_tis;
         FSIndexInput* fsii_tii;
         int skipInterval;
         FieldInfos* fis;
         int size;
         int pos;
         TermInfosReader(wstring dir,wstring segment, FieldInfos* fis);
         ~TermInfosReader();
         TermInfo* get(int position);
         TermInfo* get(Term* t);
         TermInfo* next();
         int getSize();
         void init();
};
class SegmentTermDocs{

  public:
      int df;
      int del_df;
      int pos;

      int doc;
      int freq;

      long freq_start;
      long skip_start;

      SegmentReader* sr;
      FSIndexInput* freq_fsii;
      MultiLevelSkipListReader* mreader;
      TermInfosReader* treader;
      TermInfo* cti;

      SegmentTermDocs(SegmentReader* sr);
      ~SegmentTermDocs();
      bool next();
      bool nextTerm();
      int read(int* docs,int*freqs,int len);
      bool seek(Term* t);
      void seek(TermInfo* tf);
      bool skipTo(int target);
      bool init_V_Term();
      void resetDocPos();
};
class MTermDocs{
    public:
      vector<SegmentTermDocs*>* mtermdocs;

      int delCount;
      int docCount;
      int docid;
      int freq;
      int df;
      int del_df;
      int* isCur;
      int* starts;
      int pos;
      float* norms;
      vector<SegmentReader*>* srs;
      Term* t;

      MTermDocs(vector<SegmentReader*>* srs);
      ~MTermDocs();

      bool next();
      int read(int* docs,int*freqs,int len);
      bool seek(Term* t);
      void seek(TermInfo* tf,Term* t);
      bool skipTo(int target);
      int getDocID();
      int getFreq();
      void Comput_df();
      void resetDocPos();
      int readerIndex(int index);
      float getNorm(int docid);
      int getDF();
};
class SegmentTermPositions{
   public:
       int df;
       int del_df;
       int pos;

       int doc;
       vector<int>* poss;

       long prox_start;
       long skip_start;

       SegmentReader* sr;
       FSIndexInput* prox_fsii;
       MultiLevelSkipListReader* mreader;
       TermInfosReader* treader;
       TermInfo* cti;

       SegmentTermPositions(SegmentReader* sr);
       ~SegmentTermPositions();
       bool next();
       bool nextTerm();
       bool seek(Term* t);
       void seek(TermInfo* ti);
       bool skipTo(int target);
       bool init_V_Term();
       void resetDocPos();
};
class SegmentMergeInfo{
       public:
         int base;
         bool step_flag;
         SegmentTermDocs* termDocs;
         SegmentTermPositions* termpos;
         SegmentMergeInfo(int base,SegmentTermDocs* termDocs,SegmentTermPositions* termpos);
         bool nextTerm();
         bool next();
         int getDocId();
         int getFreq();
         vector<int>* getPoss();
};
class ComSMI{
      public:
      bool operator()(const SegmentMergeInfo* smi1,const SegmentMergeInfo* smi2)const{
           if((*(smi1->termDocs->cti))==(*(smi2->termDocs->cti))){
              return smi1->base<smi2->base;
           }else{
              return (*(smi1->termDocs->cti))<(*(smi2->termDocs->cti));
           }
      }
};
};
#endif
