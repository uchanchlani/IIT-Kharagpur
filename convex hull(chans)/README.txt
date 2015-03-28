This directory consists of various files:

input.in: first line of it consists of no. of points n, and the next n lines contains those points
input1.in: this is given as input to the gen_rand_points.cpp which inturn generates the input.in file for us.

temp.txt: used in gnuplot, also consists of all the randomly generated points.

intoutput.out: contains the intermediate output of the graham's scan
output.out: contains the final output of the code

makefile.sh: a script which will do all the work for you, which is compiling all source files, generating random points, and using them in chan's algorithm

gen_rand_points: takes two parameters: a file in which it will keep the random numbers and their count (input.in)0, and a file in which it will only keep the random numbers(temp.txt). It also takes input as hom many points you want and their upper and lower bound.
point.cpp and point.h: main chan's algorithm is in these files.
main.cpp: main function to run the chan's algorithm is in this file

README.txt: The file which we are reading right now.

In terminal type:
chmod +x makefile.sh

Then:
./makefile.sh

<Edit the input1.in file to give the number of input points and its range.>

to see the plot run this command in the gnuplot:
plot "output.out" using 1:2 with lines, "temp.txt" using 1:2


Submitted by:
Utkarsh Chanchlani
11CS10049


