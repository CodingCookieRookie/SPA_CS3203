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
	PQLDEToken getPQLDEToken(string);
};