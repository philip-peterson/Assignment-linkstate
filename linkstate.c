#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include "linkstate.h"

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

#define CHECKERROR() \
   if (ferror(stream)) { \
      return TOK_ERROR; \
   }

int *edges;

/*
 * Returns either a token type or a number. Token types are values
 * corresponding to TOK_* macros. If the return values is less than zero,
 * it's a token type. Otherwise it's a number. Numbers represent data
 * read from the file.
 * 
 * When the stream has been exhausted, this function will return TOK_EOF's forever.
 * If a TOK_ERROR or TOK_UNEXPECT is returned by this function at any point for 
 * a given stream, the results of subsequent calls to this function with the same
 * stream are undefined.
 */
int getTok(FILE * stream) {
   if (feof(stream)) {
      return TOK_EOF;
   }

   int num;

   int c;
   do {
      c = fgetc(stream);
      CHECKERROR();
      if (feof(stream)) {
         return TOK_EOF;
      }
   } while (isspace(c));

   if ( c == ',' ) {
      return TOK_COMMA;
   }
   else if ( c == '.' ) {
      return TOK_NEWLINE;
   }
   else if ( c == 'N' ) {
      return TOK_N;
   }
   else if ( c == 'E' ) {
      if (fscanf(stream, "OF.%n", &num) == 0 && num == 3) {
         return TOK_EOF;
      }
      else {
         return TOK_UNEXPECT;
      }
   }
   else {
      ungetc(c, stream);
      CHECKERROR();
      if ('0' <= c && c <= '9') {
         fscanf(stream, "%d", &num);
         CHECKERROR();
         return num;
      }
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
         struct node *newtail = (struct node*)malloc(sizeof(*tail));
         if (newtail == NULL) {
            fprintf(stderr, "Error: out of memory.\n");
            exit(ERR_OOM);
         }
         count++;
         newtail->next = NULL;
         if (token == TOK_N) {
            newtail->val = -1;
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
   edges = (int*)malloc(sizeof(*edges)*(N*N));
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

   dijkstra(edges, N);

   if (fclose(stream) != 0) {
      return 0;
   }
   return 1;
}

int ceil_log10(int x, int& power) {
   int log = 1;
   power = 1;
   while (x > power) {
      log++;
      power *= 10;
   }
   return log;
}

int getNumSpacesToPrintNumbersFromOneToNJoinedByCommas(int n) {
   if (n == 1) {
      return 1;
   }
   if (n < 1) {
      return 0;
   }

   int numSpaces = 0;

   int power;
   int currentDigits = ceil_log10(n, power);
   power /= 10;
   currentDigits--;

   while (power != 1) {
      numSpaces += (n - power + 1)*currentDigits; // each number
      numSpaces += currentDigits; // for the commas
      n = power;
      power /= 10;
      currentDigits--;
   }

   numSpaces += (n-1)*2; // 1 digit numbers (greater than zero) followed by commas
   numSpaces--; // no need for a final trailing comma

   return numSpaces;
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

void dijkstra(int* edges, int n) {
   printTableHeaders(n);

   int visited[n];
   int predecessors[n];
   int distance[n];

   for (int i = 0; i < n; i++) {
      visited[i] = 0;
      predecessors[i] = -1;
      distance[i] = INT_MAX;
   }

   distance[0] = 0;

   int currentlyVisiting = 0;
   int step = -1;

   while (1) {
      step++;

      int indexOfMinimum = -1;
      int minimum = INT_MAX;

      // Relax, while noting the minimum tentative distance of nodes that are unvisited.
      // We visit backwards (n-1 to 0) because we want to prefer the node with a smaller
      // index number, per the directions.
      for (int i = n-1; i >= 0; i--) {
         if (i == currentlyVisiting) {
            continue;
         }

         int weight = edges[currentlyVisiting*n + i];
         
         if (weight == -1) {
            // Check to make sure the node is connected to the current node
            continue;
         }

         int myDist = distance[currentlyVisiting];
         int contenderDistance = myDist+weight;
         if (distance[i] > contenderDistance) {
            distance[i] = contenderDistance;
            predecessors[i] = currentlyVisiting;
         }

         // Keep track of minimum-tentative-distance unvisited node
         if (distance[i] < minimum && !visited[i]) {
            minimum = distance[i];
            indexOfMinimum = i;
         }
      }

      visited[currentlyVisiting] = 1;
      printStep(step, n, visited, distance, predecessors);

      if (indexOfMinimum == -1) {
         // No adjacent node to <currentlyVisiting> is unvisited, so check to see
         // if there are any non-adjacent nodes that haven't been visited.
         
         bool foundUnvisited = false;
         for (int j = 0; j < n; j++) {
            if (!visited[j]) {
               foundUnvisited = true;
               currentlyVisiting = j;
               break;
            }
         }

         if (!foundUnvisited) {
            // If we've reached here, there are no unvisited nodes. We're done!
            break;
         }
      }
      else {
         currentlyVisiting = indexOfMinimum;
      }
   }
}

void printStep(int step, int n, int *visited, int *distance, int *predecessors) {
   printf("%d\t", step);

   bool first = true;
   for (int i = 0; i < n; i++) {
      if (!visited[i]) {
         continue;
      }
      if (!first) {
         printf(",");
      }
      printf("%d", (i+1));
      first = false;
   }

   for (int i = 1; i < n; i++) {
      printf("\t");
      if (predecessors[i] == -1) {
         continue;
      }
      printf("%d,%d", distance[i], predecessors[i]+1);
   }


   printDashes();
}

void printDashes() {
   printf("\n---------------------------------------------------------------------------------\n");
}

void printTableHeaders(int n) {
   printDashes();
   printf(
      "Step"   "\t"
      "N'"     "\t"
   );
   for (int i = 1; i < n; i++) {
      printf("D(%d),p(%d)\t", i+1, i+1);
   }
   printDashes();
}
