COMP GEO
Assignment 5,
By: Utkarsh Chanchlani 11CS10049

Readme file:

All necessary functions in hplane.h and hplane.cpp

to compile the files and run it with the default example, type in terminal:
make

it creates a executable file named assgn5

run:
./assgn5 <file containing half planes>(required) <file containing equation to be maximized>(optional)
to run any other sample inputs.

Some more sample inputs and their corresponging outputs have been provided in the folder named "more_samples". Note:
hinput<number>.in is the input halfplanes for example # <number>
einput<number>.in is the input equation for example # <number>
output<number>.out is the output for example # <number>



Half planes are stored in the format "a.x + b.y <= c", enter a followed by b and then c
Lines are internally stored as m,c satisfying "y = m.x + c"

Data Structures used:
1. From STL:
   i. vector is used to store the vector of hplanes and evaluate the solution.
   ii. pair is used to store the x, y coordinates of a point.

2. My custom Data Structures
   i. hplane: It is a class whose private members are a,b,c which satisfy the hplane "ax+by<=c". This class has various methods which includes:
      a. constructors, destructors and assignment operators
      b. is_satisfy: outputs a boolean which tells if the given point satisfies the equation of the halfplane.
      c. get methods: to get the a, b and c values.
      d. istream and ostream operators: to input and output the halfplane.

   ii. line: It is a class whose private members are a,m,c which satisfy the line equation "y=mx+c" (when a = 1.0) and the equations of a vertical line "mx+c=0" (when a=0.0). This class has various methods which includes:
       a. constructors, destructors and assignment operators
       b. get methods: to get a, m and c values.
       c. getx method: For the y coordinate of a particular point in the line, it returns it's corresponding x coordinate.
       d. gety method: <understood>
       e. is_intersect: outputs a boolean which tells if two lines intersect or not.
       f. get_intersect: returns the point of intersection of the two lines.
       g. get_range: takes as input a particular halfplane and outputs a "ray", which is satisfied by the input halfplane
       h. norm(): a function to normalize the values of the line to standard values (a=0.0 or a=1.0), to ease the calculations.
