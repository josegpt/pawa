#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 4096

const char name[] = "pawa";
const char version[] = "0.0.1";
const char usage[] =
  "usage: ./pawa [...OPTIONS] <filename>\n"
  "options:\n"
  "    -h print this message and exits.\n"
  "    -v print version and exits.\n"
  "    -t print tokenize messages.\n";

typedef enum {
  DATE,
  TIME,
  PERSON,
  MESSAGE,
  ERROR,
  DONE
} TokenType;

typedef enum {
  INIT,
  INDATE,
  INTIME,
  /* INPERSON, */
  /* INMESSAGE */
} State;

typedef struct {
  TokenType tt;
  char lexeme[MAX_SIZE];
  int lnumber;
} Token;

Token get_token(FILE* fp, int* lnumber)
{
  Token token = {0};
  State state = INIT;
  char lexeme[MAX_SIZE];
  int lexeme_length = 0;
  int c;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') (*lnumber)++;
    switch(state) {
    case INIT:
      if (isspace(c)) continue;
      if (isdigit(c)) {
        state = INDATE;
      } else {
        token.tt = ERROR;
        sprintf(token.lexeme, "ERROR: unknown token `%c' on line %d\n", c, *lnumber);
        token.lnumber = *lnumber;
        return token;
      }
    case INDATE:
      if (isdigit(c) || c == '/') {
        lexeme[lexeme_length++] = c;
      } else if (c == ',') {
        token.tt = DATE;
        token.lnumber = *lnumber;
        strcpy(token.lexeme, lexeme);
        return token;
      } else if (c == ':') {
        ungetc(c, fp);
        state = INTIME;
      } else {
        token.tt = ERROR;
        token.lnumber = *lnumber;
        sprintf(token.lexeme, "ERROR: found `%c' while tokenizing `date' on line %d\n", c, *lnumber);
        return token;
      }
      break;
    case INTIME:
      if (c == ' ') continue;
      if (isdigit(c) || c == ':' || c == 'A' || c == 'P' || c == 'M') {
        lexeme[lexeme_length++] = c;
      } else if (c == '-') {
        token.tt = TIME;
        token.lnumber = *lnumber;
        strcpy(token.lexeme, lexeme);
        return token;
      } else {
        token.tt = ERROR;
        token.lnumber = *lnumber;
        sprintf(token.lexeme, "ERROR: found `%c' while tokenizing `time' on line %d\n", c, *lnumber);
        return token;
      }
      break;
    }
  }
  if (feof(fp)) {
    token.tt = DONE;
    strcpy(token.lexeme, "DONE");
    token.lnumber = *lnumber;
    return token;
  }
  token.tt = ERROR;
  strcpy(token.lexeme, "ERROR: something went wrong while performing I/O");
  token.lnumber = *lnumber;
  return token;
}

int main(int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, "hv")) != -1) {
    switch(opt) {
    case 'h':
      printf(usage);
      exit(EXIT_SUCCESS);
    case 'v':
      printf("%s version: %s", name, version);
      exit(EXIT_SUCCESS);
    }
  }
  const char* file_path = argv[optind];
  if (file_path == NULL) {
    fprintf(stderr, "ERROR: file_path not provided");
    exit(EXIT_FAILURE);
  }
  FILE *fp = fopen(file_path, "r");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: could not open file: `%s'", file_path);
    exit(EXIT_FAILURE);
  }
  int lnumber = 0;
  Token token;
  while(token.tt != ERROR && token.tt != DONE) {
    token = get_token(fp, &lnumber);
    switch(token.tt) {
    case DATE:
      printf("%d: DATE(%s)\n", token.lnumber, token.lexeme);
      break;
    case TIME:
      printf("%d: TIME(%s)\n", token.lnumber, token.lexeme);
      break;
    case PERSON:
      printf("%d: PERSON(%s)\n", token.lnumber, token.lexeme);
      break;
    case MESSAGE:
      printf("%d: MESSAGE(%s)\n", token.lnumber, token.lexeme);
      break;
    case DONE:
      printf("%d: %s\n", token.lnumber, token.lexeme);
      break;
    default:
      printf("%d: %s\n", token.lnumber, token.lexeme);
    }
  }
  /* fclose(fp); */
  return EXIT_SUCCESS;
}
