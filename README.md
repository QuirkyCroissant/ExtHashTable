# Extendible Hash Table - ADS_set

## Overview

This project, which was implemented by me in context of a data structure course, realises an extendible hash table for a set of keys using C++. The data structure supports dynamic resizing to efficiently handle growing and shrinking datasets. It provides functionalities for insertion, deletion, searching, and iteration over the elements.

## Features

- **Extendible Hashing**: Dynamic hashing technique that expands and contracts the hash table as necessary.
- **Buckets and Directory**: The hash table consists of buckets that hold elements and a directory that points to these buckets.
- **Collision Handling**: Efficiently handles collisions through bucket splitting and directory expansion.
- **Iterator Support**: Includes an iterator to traverse the elements in the hash table.
- **Lazy Deletion**: Marks elements as `free_again` instead of immediately removing them, improving deletion efficiency.

## Class Overview

### `ADS_set`

Template class for the hash table.

#### Public Member Functions

- `ADS_set()`: Constructor to initialize the hash table.
- `ADS_set(std::initializer_list<key_type> ilist)`: Constructor with initializer list.
- `ADS_set(const ADS_set &other)`: Copy constructor.
- `~ADS_set()`: Destructor to clean up allocated resources.
- `ADS_set &operator=(const ADS_set &other)`: Assignment operator.
- `ADS_set &operator=(std::initializer_list<key_type> ilist)`: Assignment operator with initializer list.
- `size_type size() const`: Returns the number of elements in the set.
- `bool empty() const`: Checks if the set is empty.
- `void insert(std::initializer_list<key_type> ilist)`: Inserts elements from an initializer list.
- `std::pair<iterator, bool> insert(const key_type &key)`: Inserts a single element.
- `template<typename InputIt> void insert(InputIt first, InputIt last)`: Inserts elements from a range.
- `void clear()`: Clears the set.
- `size_type erase(const key_type &key)`: Erases an element by key.
- `size_type count(const key_type &key) const`: Counts the occurrences of a key (0 or 1).
- `iterator find(const key_type &key) const`: Finds an element by key.
- `void swap(ADS_set &other)`: Swaps the contents with another set.
- `const_iterator begin() const`: Returns an iterator to the beginning.
- `const_iterator end() const`: Returns an iterator to the end.
- `void dump(std::ostream &o = std::cerr) const`: Dumps the internal state for debugging.

### `ADS_set::Iterator`

Iterator class to traverse the hash table.

#### Public Member Functions

- `reference operator*() const`: Dereference operator.
- `pointer operator->() const`: Arrow operator.
- `Iterator &operator++()`: Pre-increment operator.
- `Iterator operator++(int)`: Post-increment operator.
- `friend bool operator==(const Iterator &lhs, const Iterator &rhs)`: Equality operator.
- `friend bool operator!=(const Iterator &lhs, const Iterator &rhs)`: Inequality operator.

}
