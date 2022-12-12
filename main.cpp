#include <iostream>
#include <stdlib.h>
#include "header/inputbuffer.hpp"
#include "header/metacommands.hpp"
#include "header/preparestatement.hpp"
#include "header/statement.hpp"
#include "header/rows.hpp"
#include "header/tables.hpp"
using namespace std;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = new InputBuffer();
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() { cout << "db > "; }

/*ssize_tはvalidなサイズであるか、エラーを示す負の値となる関数の戻り値として使われる*/
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

// Wrapper for existing functionality that leaves room for more commands
MetaCommandResult do_metacommand(InputBuffer* input_buffer){
  if (strcmp(input_buffer->buffer, ".exit")==0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}


// SQLコンパイラ(InsertとSelectのみ解釈可能)
PrepareResult prepare_statement(InputBuffer* input_buffer,Statement* statement){
  if (strncmp(input_buffer->buffer, "insert", 6)==0) {
    statement->type = STATEMENT_INSERT;
    int args_assigned = sscanf(
      input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
      statement->row_to_insert.username, statement->row_to_insert.email
    );
    if (args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS; // Ex. 'insert hoge foo'に対して、PREPARE_SUCCESSを返す 
  } 
  if (strcmp(input_buffer->buffer, "select")==0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS; 
  } 
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row_to_insert = &(statement->row_to_insert);

  serialize_row(row_to_insert, row_slot(table, table->num_rows));
  table->num_rows += 1;

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    deserialize_row(row_slot(table, i), &row);
    print_row(&row);
  }
  return EXECUTE_SUCCESS;
}

void print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}


// SQL Statementの実行
ExecuteResult execute_statement(Statement* statement, Table* table){
  switch (statement->type)
  {
  case (STATEMENT_INSERT):
    cout << " ------- " << endl;
    printf("This is where we would do an Insert.\n");
    break;
    return execute_insert(statement, table);
  case (STATEMENT_SELECT):
    printf("This is where we would do an Select.\n");
    break;
    return execute_insert(statement, table);
  }
}


int main(int argc, char* argv[]) {
    Table* table = new_table();
    InputBuffer* input_buffer = new_input_buffer();
    cout << "======" << endl;
    /* 戻り値が０ならば比較した二つの文字列は完全に一致している */
    while (true) {
        print_prompt();
        read_input(input_buffer);

        // "."コマンド=“meta-commands”
        if (input_buffer->buffer[0] == '.'){
          switch (do_metacommand(input_buffer))
          {
          case META_COMMAND_SUCCESS:
            /* code */
            continue;
          case META_COMMAND_UNRECOGNIZED_COMMAND:
            printf("Unrecognized command '%s'\n", input_buffer->buffer);
            continue;
          }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
          /* code */
          break;
        case (PREPARE_SYNTAX_ERROR):
          printf("Syntax error. Could not parse statement.\n");
         continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
          printf("Unrecognized keyword at start of '%s'.\n",
                 input_buffer->buffer);
          continue;
        }
        execute_statement(&statement,table);

        switch (execute_statement(&statement, table))
        {
        case (EXECUTE_SUCCESS):
          printf("Executed.\n");
          break;
    
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table full.\n");
            break;
        }
    }
}