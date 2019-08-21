/* 
Generate a random number between 0 and a specified max value. 

This example uses the function interface, and shows straightforward
use of generate_random_number().

*/

#include "local_random.hpp"

#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
try
{
 int max = (2 == argc) ? stoi(argv[1]) : 10;

 cout << local_random::generate_random_number(max) << '\n';
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
