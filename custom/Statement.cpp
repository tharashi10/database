#include <iostream>
#include <cstring>
#include <string>
#include "enum.cpp"
#include "Row.cpp"

class Statement
{
public:
    StatementType type;
    Row row_to_insert;
};