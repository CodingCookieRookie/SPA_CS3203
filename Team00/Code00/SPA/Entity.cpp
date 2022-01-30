#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include "./Types.h";
#include "./Entity.h";

using namespace std;

int Entity::insertNow(VAR_IDX v) {
	return 1;
}

int Entity::insertNow(PROC_IDX v) {
	return 2;
}

int* Entity::getInstance() {
	int i = 1;
	int* res = &i;
	return res;
}

void Entity::insertVar(string varName) {

}
string Entity::getVarName(int varIdx) {
}
int Entity::getVarIdx(string varName) {
}
unordered_set<int> Entity::getAllVars() {
}
void Entity::insertProc(string procName) {
}
string Entity::getProcName(int procIdx) {
}
int Entity::getProcIdx(string procName) {
}
unordered_set<int> Entity::getAllProcs() {
}
void Entity::insertConst(int const) {
}
unordered_set<int> Entity::getAllConsts() {
}
void Entity::insertStmt(string stmtType) {
}
unordered_set<int> Entity::getStmtIdxFromType(string stmtType) {
}
unordered_set<int> Entity::getAllStmts() {
}
unordered_set<int> Entity::getStmtsFromProc(int procIdx) {
}