#pragma once

using namespace std;

#include <iostream>
#include <string>
#include "DesignEntity.h"

class PQLDEToken
{
private:
	string synonym;
	string designEntity;

public:
	//void setPQLDEToken(string);
	string getPQLDETokenString();
	void setSynonym(string s);
	void setDesignEntity(string s);
	static PQLDEToken getPQLDEToken(string);
	
};