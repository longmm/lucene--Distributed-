#ifndef MY_SEARCH
#define MY_SEARCH

#include <iostream>
#include <math.h>
#include <queue>

#include "index.h"

#define LAND 0
#define LOR 1
#define LNOT 2


namespace lucene_index{

   class IndexReader;
   class Term;
   class  MTermDocs;

};





using namespace std;
using namespace store;
using namespace document;
using namespace lucene_index;
using namespace lucene_util;

namespace lucene_search{

class Query;
class Score;
class Hits;


class Similarity
{
	public:
		static char encodeNorm(float);
	    static float decodeNorm(char);
		static float lengthNorm(int);
};
class IndexSearcher{
    public:
      Query* mq;
      Score* mc;
      IndexReader* ir;
      IndexSearcher(IndexReader* ir);
      ~IndexSearcher();
      Hits* search(Query* q);
      Score* make_score_tree(Query* tmp_q);
      void compute();
};
class Score{
    public:
       int docid;
       float doc_score;
       int score_type;
       virtual bool next()=0;
       virtual bool skipTo(int target)=0;
       virtual void compute_score()=0;
};
class Query{
    public:
        int query_type; // 0 termquery    1 boolquery
};
class BoolQuery:public Query{
    public:
       int type;
       vector<Query*>* querys;
       BoolQuery(int type);
       ~BoolQuery();
       void add(Query* q);
};
class TermQuery:public Query{
    public:
         Term* term;
         bool is_not;
         TermQuery(Term* t,bool is_not);
         ~TermQuery();
};
class TermScore:public Score{
    public:

        int freq;
        MTermDocs* mtermdocs;
        Term* term;
        bool is_not;
        TermScore(MTermDocs* m,Term* t,bool is_not);
        ~TermScore();
        bool next();
        bool skipTo(int target);
        void compute_score();
};
class BoolScore:public Score{

    public:
       int type; // 0 and      1 or
       int* flags;
       bool XH_flag;
       vector<Score*>* subScores;
       map<int,float>* res;
       map<int,float>::iterator pos_iter;
       set<int>* NOT_set;

       BoolScore(int type);
       ~BoolScore();

       void add(Score* sc);
       bool next();
       bool skipTo(int target);
       void compute();
       bool contain(int target);
       void compute_score();
};
class RESDOC{
       public:
          int docid;
          float score;
          RESDOC(int docid,float score);
          RESDOC(const RESDOC& adoc);
          bool operator<(const RESDOC& resdoc)const;
          bool operator>(const RESDOC& resdoc)const;
          void operator=(const RESDOC& resdoc);
};
class Hits{
    public:
       priority_queue<RESDOC>* p_que;
       IndexReader* ir;

       Hits(IndexReader* ir);
       ~Hits();
       void add(int docid,float score);
       Document* nextDoc();
};
};
#endif
