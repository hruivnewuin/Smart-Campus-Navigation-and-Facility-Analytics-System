/*
Purpose: Adding the custom Hashtable class & separating chaining for collision handling.
*/

#include "HashTable.h"
#include <cctype>

//Initializes the hash table w/ a capacity & empty buckets
HashTable::HashTable(int cap)
  : capacity(cap > 0 ? cap: DEFAULT_CAPACITY), entryCount(0), collisionCount(0), buckets(capacity){}

// calculates a valid bucket index from a character of the normalized uppercase key
int HashTable::hashFunction(const std::string& key) const{
  unsigned long hashValue = 0;
  for (size_t i = 0; i < key.length(); i++){
    unsigned char c = static_cast<unsigned char>(std::toupper(key[i])));
    hashValue = (hashValue * 31UL) + c;
  }
    return static_cast<int>(hashValue % static_cast<unsigned long>(capacity));
}

//checks whether an ID already exists in its hashed bucket
bool HashTable::contains(const std::string& normalizedKey) const{
  return search(normalizedKey) != nullptr;
}

// Inserts a facility record
bool HashTable::insert(const Facility& facility){
  int index = hashFunction(facility.id);
  std::vector<Facility>& chain == buckets[index];

// Checks for duplicate ID
for (size_t i =0; i < chain.size(); i++){
    if (chain[i].id == facility.id){
      return false;
    }
}

// Counts a collision when inserting a record
if (!chain.empty()){
  collisionCount++;
}

chain.push_back(facility);
entryCount++;
return true;
}

// Searches the appropriate bucket for a matching facility ID
const Facility* HashTable::search(const std::string& normalizedKey) const {
    int index = hashFunction(normalizedKey);
    const std::vector<Facility>& chain = buckets[index];

    for (size_t i = 0; i < chain.size(); ++i) {
        if (chain[i].id == normalizedKey) {
            return &chain[i];
        }
    }
    return nullptr;
}

// Resets the hash table to an empty state
void HashTable::clear() {
    for (size_t i = 0; i < buckets.size(); ++i) {
        buckets[i].clear();
    }
    entryCount = 0;
    collisionCount = 0;
}

int HashTable::getCapacity() const {
    return capacity;
}

int HashTable::getEntryCount() const {
    return entryCount;
}

int HashTable::getCollisionCount() const {
    return collisionCount;
}
