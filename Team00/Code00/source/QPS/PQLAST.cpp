using namespace std;

#include <vector>
#include"PQLAST.h"

void PQLAST::addToAst(string token)
{
    PQLAST.push_back(token);
}

vector<string> PQLAST::getPQLAST()
{
    return PQLAST;
}