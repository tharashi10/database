#include <iostream>
#include <cstring>
#include <string>
#include "custom/enum.cpp"
#include "custom/Row.cpp"
#include "custom/Table.cpp"
#include "custom/Statement.cpp"
#include "custom/DB.cpp"


int main(int argc, char const *argv[])
{
    DB db;
    db.start();
    return 0;
}