#include <iostream>
#include <string>
#include "document.h"

using namespace std;

float document::AbstractField::getBoost()
{
   return this->boost;
}
bool document::AbstractField::getOmitNorms()
{
    return this->omitNorms;
}
bool document::AbstractField::isIndexed()
{
     return this->indexed;
}
bool document::AbstractField::isLazy()
{
     return this->lazy;
}
bool document::AbstractField::isStored()
{
	return this->stored;
}
bool document::AbstractField::isTokenized()
{
	return this->tokenized;
}
wstring document::AbstractField::getName()const
{
    return this->name;
}
void document::AbstractField::setBoost(float boost)
{  
     this->boost=boost;
}
void  document::AbstractField::setOmitNorms(bool omitnorms)
{
	this->omitNorms=omitnorms;
}
/*
bool document::Field::operator==(const Field& f)const{
   wstring* ws=(&f)->getName(); 
   return (*(this->name))==(*(f.getName()));
}
bool document::Field::operator<(const Field& f)const{
   return return (*(this->name))<(*(f.getName()));
}
*/
