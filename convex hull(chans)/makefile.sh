g++ main.cpp point.cpp -o chans
g++ gen_rand_points.cpp -o rndpts
./rndpts input.in temp.txt < input1.in
./chans < input.in

