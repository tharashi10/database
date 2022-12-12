#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#include <cstring>
#include <string>
class Row
{
public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};