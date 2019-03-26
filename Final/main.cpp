#include <iostream>
#include <functional>
#include <string>

#include "hash_table.h"



/**
 * Generate a random string that contains random
 * letters from the alphabet.
 */
std::string print_random_string()
{
	const auto alphabet_length = 26;
	const auto rand_string_size = 3;
	std::string new_string;

	char alphabet[alphabet_length] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
									   'h', 'i', 'j', 'k', 'l', 'm', 'n',
									   'o', 'p', 'q', 'r', 's', 't', 'u',
									   'v', 'w', 'x', 'y', 'z' };

	for (auto i = 0; i <= rand_string_size; i++)
	{
		new_string += new_string + alphabet[rand() % alphabet_length];
	}

	return new_string;
}

int main()
{
	/**
	 * Const representing the max size of the hash_table.
	 */
	const auto hash_size = 50;

	/**
	 * Instantiate a new instance of the hash table with
	 * data-type string for values, data-type double for keys.
	 * And a maximum size of 50 elements.
	 */
	nwacc::hash_table<std::string, double> table(hash_size);

	/**
	 * Insert random string keys and double values into the hash_table.
	 */
	for (auto x = 0; x <= hash_size; x++)
	{
		table.insert(print_random_string(), static_cast<double>(rand()));
	}

	/**
	 * Print the table in forwards and reverse order.
	 */
	table.print();

	/**
	 * Print the whole table to the console.
	 */
	std::cout << table << std::endl;
	
	return 0;
}
