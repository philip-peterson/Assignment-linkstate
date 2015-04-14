#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define ERR_USAGE 1
#define ERR_IO    2
#define ERR_OOM   3
#define ERR_INCON 4
#define ERR_ZEROE 5
#define ERR_INVAL 6

#define TOK_NEWLINE  -1
#define TOK_COMMA    -2
#define TOK_N        -3
#define TOK_EOF      -4
#define TOK_ERROR    -5
#define TOK_UNEXPECT -6

int *edges;

int getTok(FILE * stream) {
   int num;
   if ( fscanf(stream, "%d", &num) == 1 ) {
      return num;
   }

   if ( fscanf(stream, ",%n", &num) == 0 && num == 1 ) {
      return TOK_COMMA;
   }

   if ( fscanf(stream, ".%n", &num) == 0 && num == 1 ) {
      return TOK_NEWLINE;
   }

   if ( fscanf(stream, "N%n", &num) == 0 && num == 1 ) {
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

   struct node *tail = NULL;
   struct node *head = NULL;

   int N = 0;
   int count = 0;
   int row = 0;

   while (1) {
      int token = getTok(stream);
      if (token >= 0 || token == TOK_N) {
         struct node *newtail = malloc(sizeof(*tail));
         if (newtail == NULL) {
            fprintf(stderr, "Error: out of memory.\n");
            exit(ERR_OOM);
         }
         count++;
         newtail->next = NULL;
         if (token == TOK_N) {
            newtail->val = INT_MAX;
         }
         else {
            newtail->val = token;
         }

         if (tail == NULL) {
            head = newtail;
            tail = newtail;
         }
         else {
            tail->next = newtail;
            tail = newtail;
         }
      }
      else if (token == TOK_COMMA) {
         continue;
      }
      else if (token == TOK_NEWLINE) {

         if (row == 0) {
            N = count;
            if (N == 0) {
               fprintf(stderr, "Error: zero elements in first row.\n");
               exit(ERR_ZEROE);
            }
         }
         else {
            if (count != N) {
               fprintf(stderr, "Error: inconsistent element count.\n");
               exit(ERR_INCON);
            }
         }

         row++;
         count = 0;
         continue;
      }
      else if (token == TOK_EOF) {
         if (row != N) {
            fprintf(stderr, "Error: inconsistent element count (expected %d rows, got %d).\n", N, row);
            exit(ERR_INCON);
         }
         break;
      }
      else {
         fprintf(stderr, "Error: invalid input.\n");
         exit(ERR_INVAL);
      }
   }

   // copy data from LL to buffer
   int i;
   int j;
   struct node *cur = head;
   edges = malloc(sizeof(*edges)*(N*N));
   for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
         edges[i*N+j] = cur->val;
         cur = cur->next;
      }
   }

   {
   
      // now free linked list
      struct node *cur = head;
      while (cur != NULL) {
         struct node *next = cur->next;
         free(cur);
         cur = next;
      }

   }

   for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
         printf("%d ", edges[i*N+j]);
      }
      printf("\n");
   }
   printf("\n");


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

   return 0;
}
