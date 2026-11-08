/*
Author: Caleb D
Purpose: Declaring a custom HashTable class keyed by normalized facility ID. 
Uses a string hash function & chaining for collision resolution.
*/

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "Facility.h"
#include <string>
#include <vector>

class HashTable {
public:
  static const int DEFAULT_CAPACITY = 11; // inorder to have good distribution

// Initializing table w/ a specified bucket capacity
explicit HashTable(int capacity = DEFAULT_CAPACITY);

// Custom string hash function 
int hashFunction(const std::string& key) const;

//Checks if a normalized uppercase key already exists 
bool contains(const std::string& normalizedKey) const;

//inserts a facility into hash table; returns false if duplicate ID exists)
bool insert(const Facility& facility);

// looks up a facility by ID number, returns pointer
const Facility* search(const std::string& normalizedKey) const;

//clears all entries & resets
void clear();

//Diagonstic getters
int getCapacity() const;
int getEntryCount() const;
int getCollisionCount() const;

private:
  int capacity;
  int entryCount;
  int collisionCount;
std::vector<std::vector<Facility> > buckets;
};
#endif // HASHTABLE_H
