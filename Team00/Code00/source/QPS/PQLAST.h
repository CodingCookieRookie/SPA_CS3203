
#include <iostream>
#include <string>
#include <vector>

using namespace std;


class PQLAST
{
	private:
		vector<string> PQLAST;
	public:
		void addToAst(string);
		vector<string> getPQLAST();
};