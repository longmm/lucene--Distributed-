#include "lucene_search.h"

using namespace std;
using namespace lucene_search;

union CODE{
   float f;
   int i;
};
char lucene_search::Similarity::encodeNorm(float src){
      CODE c;
	  c.f=src;
	  int bits=c.i;
	  int smallfloat = bits >> (24-3);
      if (smallfloat < (63-15)<<3) {
        return (bits<=0) ? (char)0 : (char)1;
       }
       if (smallfloat >= ((63-15)<<3) + 0x100) {
         return -1;
       }
    return (char)(smallfloat - ((63-15)<<3));
}
float lucene_search::Similarity::decodeNorm(char src){
    if (src == 0) return 0.0f;
    int bits = (src&0xff) << (24-3);
    bits += (63-15) << 24;
    CODE c;
	c.i=bits;
    return c.f;
}
float lucene_search::Similarity::lengthNorm(int src){
      if(src==0){
          return 0.0f;
	  }
	  return (float)(1.0 /sqrt(src));
}