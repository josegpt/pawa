#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 4096

typedef enum {
  DATE,
  TIME,
  PERSON,
  MESSAGE,
  ERR,
  DONE
} TokenType;

typedef enum {
  INIT,
  INDATE,
  INTIME,
  INPERSON,
  INMESSAGE
} State;

typedef struct {
  TokenType tt;
  char* lexeme;
  int lnumber;
} Token;

Token get_token(FILE* fp, int* lnumber)
{
  char c;
  Token token;
  State state = INIT;
  char lex[MAX_SIZE] = "";
  while(!feof(fp)) {
    c = fgetc(fp);
    switch(state) {
    case INIT:
      if (isdigit(c)) {
        state = INDATE;
        strncat(lex, &c, 1);
      } else if (c == '-') {
        state = INPERSON;
      } else if (c == ':') {
        state = INMESSAGE;
      }
      break;
    case INDATE:
      if (c == ':') {
        state = INTIME;
        strncat(lex, &c, 1);
      } else if (c == ',') {
        token.tt = DATE;
        token.lnumber = *lnumber;
        /* FIX: Fix garbage at the end of the string */
        token.lexeme = lex;
        return token;
      } else {
        strncat(lex, &c, 1);
      }
      break;
    case INTIME:
      if (c == '-') {
        ungetc(c, fp);
        token.tt = TIME;
        token.lnumber = *lnumber;
        token.lexeme = lex;
        return token;
      } else if (c == ' ') {
        continue;
      } else {
        strncat(lex, &c, 1);
      }
      break;
    case INPERSON:
      if (c == ':') {
        ungetc(c, fp);
        token.tt = PERSON;
        token.lnumber = *lnumber;
        token.lexeme = lex;
        return token;
      } else if (c == '\n') {
        ungetc(c, fp);
        state = INMESSAGE;
      } else {
        strncat(lex, &c, 1);
      }
      break;
    case INMESSAGE:
      if (c == '\n') {
        token.tt = MESSAGE;
        token.lnumber = *lnumber;
        (*lnumber)++;
        /* FIX: Trim space from the front */
        token.lexeme = lex;
        return token;
      } else {
        strncat(lex, &c, 1);
      }
      break;
    default:
      token.tt = ERR;
      token.lexeme = "ERR: unknown token";
      token.lnumber = *lnumber;
      return token;
    }
  }
  token.tt = DONE;
  token.lexeme = "DONE";
  token.lnumber = *lnumber;
  return token;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf (stderr, "ERR: file_name was not provided");
    exit(1);
  }
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    fprintf(stderr, "ERR: could not open file: %s", argv[1]);
    exit(1);
  }
  int lnumber = 0;
  Token token;
  while(token.tt != ERR && token.tt != DONE) {
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
  fclose(fp);
  return 0;
}
