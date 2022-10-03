#include <iostream>
#include <stdlib.h>
#include "header/inputbuffer.h"
#include "header/metacommands.h"
#include "header/preparestatement.h"
#include "header/statement.h"
using namespace std;


InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = new InputBuffer();
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() { cout << "db > "<< endl; }

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
  } 
  if (strncmp(input_buffer->buffer, "select", 6)==0) {
    statement->type = STATEMENT_SELECT;
  } 
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

// SQL Statementの実行
void execute_statement(Statement* statement){
  switch (statement->type)
  {
  case (STATEMENT_INSERT):
    printf("This is where we would do an Insert.\n");
    break;
  case (STATEMENT_SELECT):
    printf("This is where we would do an Select.\n");
    break;
  }
}



int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    /*cout << "" << endl;*/
    /* 戻り値が０ならば比較した二つの文字列は完全に一致している */
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (strcmp(input_buffer->buffer, ".exit") == 0) {
            close_input_buffer(input_buffer);
            /*exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);*/
        // "."コマンド=“meta-commands”
        if (input_buffer->buffer[0] == '.'){
          switch (do_metacommand(input_buffer))
          {
          case META_COMMAND_SUCCESS:
            /* code */
            continue;
          case META_COMMAND_UNRECOGNIZED_COMMAND:
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
            continue;
          }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
          /* code */
          break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
          printf("Unrecognized keyword at start of '%s'.\n",
                 input_buffer->buffer);
          continue;
        }
        execute_statement(&statement);
        printf("Executed.\n");
      }
    }
}