using namespace std;

#include"PQLAST.h"

void PQLAST::addToAst(string token)
{
    PQLAST.push_back(token);
}

void PQLAST::getPQLAST()
{
    return PQLAST;
}