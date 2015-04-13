#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERR_USAGE 1
#define ERR_IO    2

#define TOK_NEWLINE  -1
#define TOK_COMMA    -2
#define TOK_N        -3
#define TOK_EOF      -4
#define TOK_ERROR    -5
#define TOK_UNEXPECT -5

int getTok(FILE * stream) {
   int num;
   if ( fscanf(stream, "%d", &num) == 1 ) {
      return num;
   }

   fscanf(stream, ",%n", &num);
   if ( num == 1 ) {
      return TOK_COMMA;
   }

   fscanf(stream, ".%n", &num);
   if ( num == 1 ) {
      return TOK_NEWLINE;
   }

   fscanf(stream, "N%n", &num);
   if ( num == 1 ) {
      return TOK_N;
   }

   if (feof(stream)) {
      return TOK_EOF;
   }

   if (ferror(stream)) {
      return TOK_ERROR;
   }

   return TOK_UNEXPECT;

}

int initializeBuffers(const char *fp) {
   FILE * stream = fopen(fp, "r");
   if (stream == NULL) {
      return 0;
   }

   struct node {
      int val;
      struct node *next;
   };

   if (fclose(stream) != 0) {
      return 0;
   }
   return 1;
}

int main(int argc, char **argv) {
	if (argc != 2) {
      fprintf(stderr, "Usage: linkstate FILE\n");
      return ERR_USAGE;
	}

   const char* filePath = argv[1];
   int success = initializeBuffers(filePath);
   if (success != 1) {
      char *err = strerror(errno);
      fprintf(stderr, "IO Error: %s\n", err);
      free(err);
      return ERR_IO;
   }
}
