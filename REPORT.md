How to Compile
==============

```
./build.sh
```

How to run
==========

```
./linkstate FILENAME
```

Description of code structure
==============================

First, the command-line arguments are checked, and if found to be in
proper quantity, the program is started.

The program opens the file, and getToken(...) is called, which scans
the input file using an LL(1) parser of sorts, to generate either
tokens or numbers. The numbers are inserted into a linked list,
and after the entire file has been scanned, the linked list is copied
into an adjacency matrix. Afterwards, the linked list is freed.

Dijkstra's algorithm is then run. The program looks at the first node,
determines the weights of each connected edge, then updates adjacent
nodes' tentative distances if appropriate. Then it picks the node adjacent
to the current node that has the smallest tentative distance, breaking
ties by selecting the one with the smaller index number and repeats
the relaxation process.

If no such node exists, the unvisited node with the smallest index number
is picked for relaxation. If no such node exists, the program terminates.

After each relaxation process, the information about the state of the program
is printed.

Execution results
=================

Now, I have fixed these results up to make the tabulation more readable...

```
-------------------------------------------------------------------------------------
Step       N'           D(2),p(2)    D(3),p(3)    D(4),p(4)   D(5),p(5)    D(6),p(6)    
-------------------------------------------------------------------------------------
0          1            2,1          5,1          1,1                    
-------------------------------------------------------------------------------------
1          1,4          2,1          4,4          1,1          2,4          
-------------------------------------------------------------------------------------
2          1,2,4        2,1          4,4          1,1          2,4          
-------------------------------------------------------------------------------------
3          1,2,3,4      2,1          4,4          1,1          2,4          9,3
-------------------------------------------------------------------------------------
4          1,2,3,4,5    2,1          3,5          1,1          2,4          4,5
-------------------------------------------------------------------------------------
5          1,2,3,4,5,6  2,1          3,5          1,1          2,4          4,5
-------------------------------------------------------------------------------------
```

The results of this program show that the fastest path to 2 is of distance 2, to 3 is
of distance 3, to 4 is of 1, to 5 is of 2, and to 6 is of 4.

If you examine the final line of this output, following each predecessor, you can also
construct a shortest path for each:

```
2: 2 <- 1
3: 3 <- 5 <- 4 <- 1
4: 4 <- 1
5: 5 <- 4 <- 1
6: 6 <- 5 <- 4 <- 1
```

These seem to be correct judging by inspection of the graphic provided.

Bugs / Missing Items / Limitations
==================================

I do not believe there are any bugs except for a couple unhandled out 
of memory errors, but there are a couple limitations.

For one, the memory requirement is a bit larger than it needs to be. At
one point, everything is stored in a linked list and an adjacency matrix
at the same time, making the space complexity O(2*n). 

Now, the format that the graph is provided in lends itself well to storage
as an adjacency matrix, even though adjacency lists might be more space
efficient. But even assuming an adjacency matrix was to be used, I could
still have simply read the first line of the file once to determine the
number of vertices, allocated space for the adjacency matrix, then rewound
the stream, and read the entire file directly into the adjacency matrix,
without the need for an intermediary linked-list. That would bring the space
complexity down to O(n).

Additionally, because it seemed like creating properly spaced tabular data
in C was beyond the scope of the assignment, I simply delimited each data point
in the output with an ASCII tab character. In an ideal world with infinite
time and energy, it would be nice to have employed a pretty-printing tabular data
library to print the data out in a more readable form. As it is, the output
can simply be copied and pasted into a spreadsheet program such as Excel.
