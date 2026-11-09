/*
Author: Caleb Djan
Purpose: Manages loading, validation, storage, searching, & reporting of campus facility records alongside the custom ID HashTable
*/

#ifndef FACILITYMANAGER_H
#define FACILITYMANAGER_H

#include "Facility.h"
#include "HashTable.h"
#include <string>
#include <vector>

class FacilityManager {
public:
    FacilityManager(int hashCapacity = HashTable::DEFAULT_CAPACITY);

// Load & validate facilities.txt versus the list of accepted buildings
  void loadFromFile(const std::string& filepath,
                    const std::vector<std::string>& acceptedBuildings);

  // Menu Option 1: Display all accepted facilities in file order
  void displayAllFacilities() const;

  // Menu Option 2: Prompt user for an ID and search via custom HashTable
  void promptAndSearchById() const;
  void searchById(const std::string& rawId) const;

  // Accessors
  const std::vector<Facility>& getFacilities() const;
  const HashTable& getHashTable() const;

  // Utility validation methods
  static std::string trim(const std::string& str);
  static std::string toUpper(const std::string& str);
  static bool equalsIgnoreCase(const std::string& a, const std::string& b);
  static bool isValidFacilityIdFormat(const std::string& normalizedId);

private:
  std::vector<Facility> facilities; // Preserves file insertion order
  HashTable idTable;                // Custom hash table for ID lookup

  // Finds the building name from buildings.txt
  std::string findCanonicalBuilding(
      const std::string& buildingInput,
      const std::vector<std::string>& acceptedBuildings) const;
};

#endif // FACILITYMANAGER_H
