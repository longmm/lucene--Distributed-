#include <iostream>
#include <string>
#include <vector>
#include "index.h"

using namespace std;

wstring lucene_index::IndexFileNames::SEGMENTS(L"segments");
wstring lucene_index::IndexFileNames::SEGMENTS_GEN(L"segments.gen");
wstring lucene_index::IndexFileNames::DELETABLE(L"deletable");
wstring lucene_index::IndexFileNames::NORMS_EXTENSION(L"nrm");
wstring lucene_index::IndexFileNames::COMPOUND_FILE_EXTENSION(L"cfs");
wstring lucene_index::IndexFileNames::DELETES_EXTENSION(L"del");
wstring lucene_index::IndexFileNames::PLAIN_NORMS_EXTENSION(L"f");
wstring lucene_index::IndexFileNames::SEPARATE_NORMS_EXTENSION(L"s");
wstring lucene_index::IndexFileNames::FIELD_INFO(L"fnm");
wstring lucene_index::IndexFileNames::FDT(L"fdt");
wstring lucene_index::IndexFileNames::FDX(L"fdx");
wstring lucene_index::IndexFileNames::D(L".");
wstring lucene_index::IndexFileNames::TII(L"tii");
wstring lucene_index::IndexFileNames::TIS(L"tis");
wstring lucene_index::IndexFileNames::FRQ(L"frq");
wstring lucene_index::IndexFileNames::PRX(L"prx");

vector<wstring> lucene_index::IndexFileNames::INDEX_EXTENSIONS_IN_COMPOUND_FILE;
bool lucene_index::IndexFileNames::init_flag=true;
vector<wstring>* lucene_index::IndexFileNames::getIEICF(){
           if(init_flag==true){

	      init_flag=false;

              wstring a(L"fnm");
			  wstring b(L"fdx");
			  wstring c(L"fdt");
			  wstring d(L"tii");
			  wstring e(L"tis");
			  wstring f(L"frq");
			  wstring g(L"prx");
			  wstring h(L"tvx");
			  wstring i(L"tvd");
			  wstring j(L"tvf");
			  wstring k(L"nrm");
			  wstring l(L"del");

			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(a);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(b);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(c);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(d);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(e);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(f);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(g);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(h);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(i);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(j);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(k);
			  INDEX_EXTENSIONS_IN_COMPOUND_FILE.push_back(l);
	    }
            return &INDEX_EXTENSIONS_IN_COMPOUND_FILE;
}
