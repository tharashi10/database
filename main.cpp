/**
* @file main.cpp
* @brief Simple Database Main
* @author takahiro harashima
* @date Dec 1st,2022
*
* @details made original applications in cpp
* @note 
*/

#include <iostream>
#include <cstring>
#include <string>

using namespace std;

/**
 * @enum MetaCommandResult
 * Metaコマンドの結果の列挙体
 * Metaコマンドは Non-SQLなコマンド(.から始まる)ものとして定義
*/
enum MetaCommandResult
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

/**
 * @enum PrepareResult
 * SQLのパース時に事前に呼び出される
*/
enum PrepareResult
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT
};

/**
 * @enum
 * SQL文の列挙体
*/
enum StatementType
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

/**
 * @enum
 * SQL実行結果の列挙体
*/
enum ExecuteResult
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};

/**
 * @def
 * 列サイズのマクロ
*/
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

/** 行Class */
class Row
{
public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
    Row()
    {
        id = 0;
        username[0] = '\0';
        email[0] = '\0';
    }
    Row(uint32_t id, const char *username, const char *email)
    {
        this->id = id;
        strncpy(this->username, username, COLUMN_USERNAME_SIZE + 1);
        strncpy(this->email, email, COLUMN_EMAIL_SIZE + 1);
    }
};

/**
 * @def
 * 構造体の属性サイズを取得するマクロ
*/
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(Row &source, void *destination)
{
    memcpy((char *)destination + ID_OFFSET, &(source.id), ID_SIZE);
    memcpy((char *)destination + USERNAME_OFFSET, &(source.username), USERNAME_SIZE);
    memcpy((char *)destination + EMAIL_OFFSET, &(source.email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row &destination)
{
    memcpy(&(destination.id), (char *)source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination.username), (char *)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination.email), (char *)source + EMAIL_OFFSET, EMAIL_SIZE);
}

/**
 * @def
 * 表のページサイズのマクロ
*/
#define TABLE_MAX_PAGES 100

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

/** 表クラス */
class Table
{
public:
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
    Table()
    {   
        cout << "TABLE_MAX_PAGES: " << TABLE_MAX_PAGES << endl;
        num_rows = 0;
        for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
        {
            pages[i] = NULL;
        }
    }
    ~Table()
    {
        for (int i = 0; pages[i]; i++)
        {
            free(pages[i]);
        }
    }
};

/**
 * @fn
 * slot=Pageにおける行番号
 * slot=0ならば、そのPageの最初の行番号に一致する
*/
void *row_slot(Table &table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table.pages[page_num];
    if (page == NULL)
    {
        // Pageにアクセスする時だけ、メモリを割り当てる
        page = table.pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return (char *)page + byte_offset;
}

/** ステートメント Class */
class Statement
{
public:
    StatementType type;
    Row row_to_insert;
};

/** DB Class */
class DB
{
public:
    void start();
    void print_prompt();

    bool parse_meta_command(string &command);
    MetaCommandResult do_meta_command(string &command);

    PrepareResult prepare_statement(string &input_line, Statement &statement);
    bool parse_statement(string &input_line, Statement &statement);
    void execute_statement(Statement &statement, Table &table);
    ExecuteResult execute_insert(Statement &statement, Table &table);
    ExecuteResult execute_select(Statement &statement, Table &table);
};

/**
 * @fn
 * REPL スタートポイント
*/
void DB::print_prompt()
{
    cout << "SimpleDatabase > ";
}

bool DB::parse_meta_command(string &command)
{
    if (command[0] == '.')
    {
        switch (do_meta_command(command))
        {
        case META_COMMAND_SUCCESS:
            return true;
        case META_COMMAND_UNRECOGNIZED_COMMAND:
            cout << "Unrecognized command: " << command << endl;
            return true;
        }
    }
    return false;
}

/**
 * @fn
 * Meta Commandの実行
*/
MetaCommandResult DB::do_meta_command(string &command)
{
    if (command == ".exit")
    {
        cout << "Thanks for using simple database. Bye!" << endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
PrepareResult prepare_insert (string &input_line, Statement &statement) {
    statement.type = STATEMENT_INSERT;

    char *insert_line = (char *)input_line.c_str();
    char* keyword = strtok(insert_line, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement.row_to_insert = Row(id,username,email);
    return PREPARE_SUCCESS;
}

/**
 * @fn
 * Prepared Statement 事前にSQLテンプレートを持っておく
*/
PrepareResult DB::prepare_statement(string &input_line, Statement &statement)
{
    if (!input_line.compare(0, 6, "insert"))
    {
        return prepare_insert(input_line, statement);
    }
    else if (!input_line.compare(0, 6, "select"))
    {
        statement.type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    else
    {
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }
}

/**
 * @fn
 * 入力ストリームのSQLをパースする
*/
bool DB::parse_statement(string &input_line, Statement &statement)
{
    switch (prepare_statement(input_line, statement))
    {
    case PREPARE_SUCCESS:
        return false;
    case PREPARE_NEGATIVE_ID:
        cout << "ID must be positive." << endl;
        return true;
    case PREPARE_STRING_TOO_LONG:
        cout << "String is too long." << endl;
        return true;
    case PREPARE_SYNTAX_ERROR:
        cout << "Syntax Error. Could not parse statement." << endl;
        return true;
    case PREPARE_UNRECOGNIZED_STATEMENT:
        cout << "Unrecognized keyword at start of '" << input_line << "'." << endl;
        return true;
    }
    return false;
}

/**
 * @fn
 * Insert文実行
*/
ExecuteResult DB::execute_insert(Statement &statement, Table &table)
{
    if (table.num_rows >= TABLE_MAX_ROWS)
    {
        cout << "Error: Table is full." << endl;
        return EXECUTE_TABLE_FULL;
    }

    void *page = row_slot(table, table.num_rows);
    serialize_row(statement.row_to_insert, page);
    table.num_rows++;

    return EXECUTE_SUCCESS;
}

/**
 * @fn
 * Select文実行
*/
ExecuteResult DB::execute_select(Statement &statement, Table &table)
{
    for (uint32_t i = 0; i < table.num_rows; i++)
    {
        Row row;
        void *page = row_slot(table, i);
        deserialize_row(page, row);
        cout << "(" << row.id << ", " << row.username << ", " << row.email << ")" << endl;
    }

    return EXECUTE_SUCCESS;
}
void DB::execute_statement(Statement &statement, Table &table)
{
    ExecuteResult result;
    switch (statement.type)
    {
    case STATEMENT_INSERT:
        result = execute_insert(statement, table);
        break;
    case STATEMENT_SELECT:
        result = execute_select(statement, table);
        break;
    }

    switch (result)
    {
    case EXECUTE_SUCCESS:
        cout << "Executed." << endl;
        break;
    case EXECUTE_TABLE_FULL:
        cout << "Error: Table is full." << endl;
        break;
    }
}

/**
 * @fn
 * REPL実装
 * @return void
*/
void DB::start()
{
    Table table;

    while (true)
    {
        print_prompt();

        string input_line;
        /**
         * std::getline
         * @fn
         * 改行文字が現れるまでの文字を入力する
        */
        getline(cin, input_line);

        if (parse_meta_command(input_line))
        {
            continue;
        }

        Statement statement;

        if (parse_statement(input_line, statement))
        {
            continue;
        }

        execute_statement(statement, table);
    }
}

int main(int argc, char const *argv[])
{
    DB db;
    db.start();
    return 0;
}