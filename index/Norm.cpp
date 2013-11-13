#include "index.h"

using namespace std;
using namespace lucene_index;


lucene_index::Norm::Norm(float* tmp,int num){
	this->num=num;
    this->norm_floats=tmp;
}
lucene_index::Norm::Norm(const Norm& n){
    this->num=n.num;
	this->norm_floats=new float[num];

    for(int i=0;i<num;i++){
       norm_floats[i]=n.norm_floats[i];
    }
}
lucene_index::Norm::~Norm(){
    delete[] norm_floats;
}
void lucene_index::Norm::operator=(const Norm& n){
    this->num=n.num;
	delete norm_floats;
    this->norm_floats=new float[num];
	 for(int i=0;i<num;i++){
       norm_floats[i]=n.norm_floats[i];
    }
}
