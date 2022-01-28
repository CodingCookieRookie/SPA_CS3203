using namespace std;

#include"Query.h"

void Query::setQuery(string userQuery)
{
    query = userQuery;
}

string Query::getQuery()
{
    return query;
}