#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace nwacc {

	template <typename T, typename K>
	class hash_table
	{
	public:
	    /**
		 * Create a new hash table with a max size of 50 elements.
		 * Then set every value within the hash_table to empty to allow
		 * later manipulation of the hash_table.
		 * @param size the max size of the hash table.
		 * @return the new hash_table.
		 */
		explicit hash_table(int size = 50) : array(next_prime(size))
		{
			this->make_empty();
		}

		/**
		 * Determine if the current position in the hash_table contains a matching key.
		 */
		bool contains(const K & key) const
		{
			return this->is_active(this->find_position(key));
		}

		/**
		 * Determine if the current position in the hash_table contains a matching value.
		 */
		bool contains(const T & value) const
		{
			return this->is_active(this->find_position(value));
		}

		/**
		 * Set the current active position in the hash_table equal to empty.
		 */
		void make_empty()
		{
			this->current_size = 0;
			for (auto & entry : this->array)
			{
				entry.type = kEmpty;
			}
		}

		/**
		* Insert the value, key, and set the is_active state of the current item in the hash_table.
		* @param value the data to be inserted.
		* @param key the key to be inserted.
		* @return true if the current position in the hash_table has a value, key, and state inserted.
		* @return false if the current position in the hash_table has not had a value, key, and state inserted.
		*/
		bool insert(const T & value, const K & key)
		{
			auto current_position = this->find_position(key);
			if (this->is_active(current_position))
			{
				if (this->array[current_position].key == key)
				{
					this->array[current_position].element = value;
				} // else, current key is not already on the hash_table, do_nothing();
				return false;
			}
			else
			{
				this->array[current_position].element = value;
				this->array[current_position].key = key;
				this->array[current_position].type = kActive;
			}

			if (++this->current_size > this->array.size() / 2)
			{ // this has a load factor of 50%
				this->rehash();
			} // else we are within the load factor do_nothing();

			return true;
		}

		/**
		 * Insert the value, key, and set the is_active state of the current item in the hash_table with move semantics.
		 * @param value the data to be inserted.
		 * @param key the key to be inserted.
		 * @return true if the current position in the hash_table has a value, key, and state inserted.
		 * @return false if the current position in the hash_table has not had a value, key, and state inserted.
		 */
		bool insert(T && value, K && key)
		{
		 	auto current_position = this->find_position(key);
		 	if (this->is_active(current_position))
		 	{
				if (this->array[current_position].key == key)
				{
					this->array[current_position].element = std::move(value);
				} // else, current key is not already on the hash_table, do_nothing();
		 		return false;
		 	}
		 	else
		 	{
		 		this->array[current_position] = std::move(value);
				this->array[current_position] = std::move(key);
		 		this->array[current_position].type = kActive;
		 	}
		
		 	if (++this->current_size > this->array.size() / 2)
		 	{
		 		this->rehash();
		 	} // else, still within the load factor, do_nothing();
		
		 	return true;
		}

		/**
		 * Removes the key at the current position in the hash_table.
		 * @param key the key to remove.
		 * @return true if the key has been set to value kDeleted.
		 * @return false if the key is not active.
		 */
		bool remove(const K & key)
		{
			auto current_position = this->find_position(key);
			if (!this->is_active(current_position))
			{
				return false;
			}
			else
			{
				this->array[current_position].type = kDeleted;
				return true;
			}
		}

		/**
		 * Removes the value at the current position in the hash_table.
		 * @param value the data to remove.
		 * @return true if the data has been set to value kDeleted.
		 * @return false if the value is not active.
		 */
		bool remove(const T & value)
		{
			auto current_position = this->find_position(value);
			if (!this->is_active(current_position))
			{
				return false;
			}
			else
			{
				this->array[current_position].type = kDeleted;
				return true;
			}
		}

		/**
		 * enum data structure containing the three types.
		 */
		enum entry_type { kActive, kEmpty, kDeleted };

		/**
		 * Returns the value of the key in the current position in the hash_table.
		 * If the key does not exist in the hash_table throw a length error.
		 */
		void get_key(const K & key)
		{
			auto current_position = this->find_position(key);
			if (array[current_position].type == !kActive)
			{
				throw std::length_error("Key not found....");
			} // else, key exists in the table do_nothing();
			return this->array[current_position].element;
		}

		/**
		* Returns the value of the element in the current position in the hash_table.
		* If the element does not exist in the hash_table throw a length error.
		*/
		void get_value(const T & value)
		{
			auto current_position = this->find_position(value);
			if(array[current_position].type == !kActive)
			{
				throw std::length_error("Value not found....");
			} // else, value exists in the table do_nothing();
			return this->array[current_position].key;
		}

		/**
		 * Print the hash_table forwards and backwards.
		 */
		void print() const
		{
			// Forwards
			std::cout << "Forwards:" << std::endl;
			for (auto i = 0; i <= array->size; i++)
			{
				std::cout << get_key(array[i]) << " | " << get_value(array[i]) << std::endl;
			}

			std::cout << std::endl;
			std::cout << std::endl;

			// Backwards
			std::cout << "Backwards" << std::endl;
			for (auto i = array->size; i >= 0; i--)
			{
				std::cout << get_key(array[i]) << " | " << get_value(array[i]) << std::endl;
			}
		}

		/**
		 * Overload the insertion operator for printing of the hash_table.
		 * @return the hash_table to the command line.
		 */
	    friend std::ostream operator<<(std::ostream & out, const hash_table & rhs)
	    {
			return out << rhs.print() << std::endl;
		}

		/**
		 * Overload the subscript operator to allow the return of the data in the current
		 * position in the hash_table. Or to allow us to change the data in the current
		 * position of the hash_table by returning the result of insert() at the current position.
		 * @param key the current key at the current position in the hash_table.
		 * @return the current key or new key.
		 */
		T &operator[](const K & key)
		{
			auto current_position = this->find_position(key);
			if (key != this->array[current_position].element)
			{
				return this->insert[current_position];
			}
			return this->array[current_position].element;
		}

	private:
		/**
		 * Determine if the current value is a prime number or not.
		 * @param number the value being checked for primeness.
		 * @return true if the current number is prime.
		 * @return false if the current number is not prime.
		 */
		static bool is_prime(const int number)
		{
			if (number == 2 || number == 3) return true;
			if (number == 1 || number % 2 == 0) return false;
			for (auto counter = 3; counter * counter <= number; counter += 2)
				if (number % counter == 0) return false;

			return true;
		}

		/**
		 * Determine if the next value is a prime number or not.
		 * @param number the value being checked for primeness.
		 * @return true if the current number is prime.
		 * @return false if the current number is not prime.
		 */
		static int next_prime(int number)
		{
			if (number % 2 == 0) ++number;
			while (!is_prime(number)) number += 2;
			return number;
		}

		/**
		 * Create a new struct of type entry for the hash_table, this entry
		 * will contain an element, key, and data type(active or inactive).
		 * @return a new entry.
		 */
		struct entry
		{
			T element;
			K key;
			entry_type type;

			entry(const T & e = T{}, const K & k = K{}, entry_type t = kEmpty)
				: element{ e }, key{ k }, type{ t } { }

			entry(T && e, K && k, entry_type t = kEmpty)
				: element{ std::move(e) }, key{ std::move(k) }, type{ t } { }
		};

		/**
		 * Gets the 'status' of the current value in the hash_table.
		 * The acceptable types are kEmpty, kActive, and kDeleted.
		 * @param place the data being checked.
		 * @return the 'status' of the current value in the hash_table.
		 */
		std::string get_type(const entry & place) const
		{
			if (place.type == kEmpty)
			{
				return "E";
			}
			
			if (place.type == kActive)
			{
				return "A";
			}

			return "D";
		}

	    /**
		 * Instantiate vector of entry type to hold is_active values.
		 */
		std::vector<entry> array;

		/**
		 * Overload the size_t operator to be the current size of the hash_table.
		 */
		std::size_t current_size{};

		/**
		 * Checks the active state of the current item in the hash_table.
		 * @param current_position the value being checked.
		 * @return true if the current value is active.
		 * @return false if the current value is inactive.
		 */
		bool is_active(int current_position) const
		{
			return this->array[current_position].type == kActive;
		}

		/**
		 * Find the current position in the hash_table.
		 * @param value the value whose position is being checked.
		 * @return the position of the value in the hash_table.
		 */
		int find_position(const T & value) const
		{
			auto off_set = 1;
			auto current_position = this->hash(value);

			while (this->array[current_position].type != kEmpty &&
				this->array[current_position].element != value)
			{
				current_position += off_set;
				off_set += 2;
				if (current_position >= this->array.size())
				{
					current_position -= this->array.size();
				} // else, we have not ran outside the size, do_nothing();
			}
			return current_position;
		}

		/**
		 * Resize the hash_table to be a more appropriate size for the data being inserted.
		 */
		void rehash()
		{
			auto old_array = this->array;
			this->array.resize(next_prime(2 * old_array.size()));
			for (auto & entry : this->array)
			{ // step 1 empty all the elements in the new array. 
				entry.type = kEmpty;
			}

			// rehash all the inserted items. 
			for (auto & entry : old_array)
			{
				if (entry.type == kActive)
				{
					this->insert(std::move(entry.element));
				} // else, the entry is not active, do_nothing();
			}

		}

		/**
		 * Stores the size fo the hash_table.
		 */
		std::size_t hash(const T & value) const
		{
			static std::hash<T> hash_object;
			return hash_object(value) % this->array.size();
		}
	};
}

#endif