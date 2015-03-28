Assignment 2
Computational Geometry


This directory consists of various files:

-> input.in: first line of it consists of no. of lines n, and the next n lines contains its start and end points
-> temp.txt: used in gnuplot, also consists of all the randomly generated lines.
-> chans_intout.out: contains the intermediate output of the graham's scan / chan
-> chans_out.out: contains the final output of the code
-> output.out: intersection points of the Bentley Ottmann algorithm
-> makefile: a makefile, which is compiling all source files for us

-> point.cpp and point.h:
The point class and all its methods are declared in these files. (A point object is a 2-D point in the X-Y plane)
The chans and graham's algorithm are also declared/defined in these files

-> line.cpp and line.h:
The lsegment class and all its methods are declared in these files. (A lsegment object is a line segment in 2-D X-Y plane)
The myheap class and all its methods are also declared in these files. (A myheap object is as the name suggests min heap (according to x co-ords) of all the event points.)
The mybbst class and all its methods are also declared in these files. (A mybbst object is basically a binary search tree of lines.)
The functions for AVL trees customized to work with the mybbst class are written in these files
mybbst internally uses the AVL tree function calls for insertion and deletion
Some miscellaneous utility functions are also present in this class. The utility functions includes isintersect(to check if 2 line segments intersect) and getintersection(to get a point object corresponding the intersection point of the two lsegment objects.)

-> main.cpp: main file
The Bentley Ottmann algorithm which uses all the above classes/functions is defined in this file
The main first executes the Bentley Ottmann algorithm and gets the points. Then it runs Chans on those points obtained

-> README.txt: The file which we are reading right now

To run the algorithm, execute this command in the terminal:
make


To see the plot execute this command in the gnuplot:
plot 'temp.txt' u 1:2:($3-$1):($4-$2) notitle with vectors, "output.out" using 1:2, "chans_out.out" using 1:2 with lines


Submitted by:
Utkarsh Chanchlani
11CS10049
