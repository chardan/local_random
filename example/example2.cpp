/* 
Generate a random number between 0 and a specified max value. 

This example uses the object-oriented interface, with a function object.

*/

#include "local_random.hpp"

#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
try
{
 int max = (2 == argc) ? stoi(argv[1]) : 10;

 local_random::random_number_generator rng(max);

 cout << rng() << '\n';
}	
catch(std::out_of_range&)
{
	cerr << "invalid parameter: out of range\n";
	return 1;
}
catch(std::invalid_argument&)
{
	cerr << "invalid parameter: parse error\n";
	return 1;
}
