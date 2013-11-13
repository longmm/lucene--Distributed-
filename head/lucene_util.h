#ifndef LUCENE_UTIL
#define LUCENE_UTIL
#include <string>
#include <set>
#include <iostream>
#include <vector>
#include<sys/types.h>
#include<dirent.h>

using namespace std;
namespace lucene_util{
   void initwchars(wchar_t*,wchar_t,int);
   bool fileExist(wstring file);
   wstring charToWstring(char*);
   wstring LongToWstring(long);
   long    wstrToLong(wstring& );
   char*   wstrToChars(wstring&);
   vector<wstring>* getFilesOfDir(char*);
   class StopWords{
         public:
           static set<wstring> stops;
           static bool init_flag;
           static bool isStops(wstring word);
   };
};
#endif
