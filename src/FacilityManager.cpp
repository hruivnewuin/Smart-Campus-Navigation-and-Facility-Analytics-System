/*
Author: Caleb Djan
Purpose: Implements facility file parsing, validation, full-list display, and custom hash table ID lookup
*/

#include "FacilityManager.h"
#include <cctype>
#include <fstream>
#include <iostream>

FacilityManager::FacilityManager(int hashCapacity)
    : idTable(hashCapacity) {}

// Trims leading and trailing spaces, tabs, and carriage returns
std::string FacilityManager::trim(const std::string& str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, last - first + 1);
}

// Converts a string to uppercase
std::string FacilityManager::toUpper(const std::string& str) {
  std::string result = str;
  for (size_t i = 0; i < result.length(); ++i) {
    result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
  }
  return result;
}

// ASCII string comparison
bool FacilityManager::equalsIgnoreCase(const std::string& a, const std::string& b) {
  if (a.length() != b.length()) {
    return false;
  }
  for (size_t i = 0; i < a.length(); ++i) {
    if (std::toupper(static_cast<unsigned char>(a[i])) !=
        std::toupper(static_cast<unsigned char>(b[i]))) {
      return false;
    }
  }
  return true;
}

// Validates that an uppercase ID is 'F' followed by exactly 3 digits
bool FacilityManager::isValidFacilityIdFormat(const std::string& normalizedId) {
  if (normalizedId.length() != 4 || normalizedId[0] != 'F') {
    return false;
  }
  for (size_t i = 1; i < 4; ++i) {
    if (!std::isdigit(static_cast<unsigned char>(normalizedId[i]))) {
      return false;
    }
  }
  return true;
}

// Matches a building name to buildings.txt and returns spelling
std::string FacilityManager::findCanonicalBuilding(
    const std::string& buildingInput,
    const std::vector<std::string>& acceptedBuildings) const {
  for (size_t i = 0; i < acceptedBuildings.size(); ++i) {
    if (equalsIgnoreCase(buildingInput, acceptedBuildings[i])) {
      return acceptedBuildings[i];
    }
  }
  return "";
}

// Loads facilities.txt, validates records, populates vector and hash table, and prints summary
void FacilityManager::loadFromFile(const std::string& filepath,
                                   const std::vector<std::string>& acceptedBuildings) {
  facilities.clear();
  idTable.clear();

  // Extract display filename
  std::string displayFilename = filepath;
  size_t slashPos = filepath.find_last_of("/\\");
  if (slashPos != std::string::npos) {
    displayFilename = filepath.substr(slashPos + 1);
  }

  std::ifstream inFile(filepath.c_str());
  if (!inFile.is_open()) {
    std::cout << "Error: Could not open file " << displayFilename << std::endl;
    std::cout << displayFilename << ": 0 accepted, 0 rejected (hash capacity "
              << idTable.getCapacity() << ", entries 0, collisions 0)" << std::endl;
    return;
  }

  int acceptedCount = 0;
  int rejectedCount = 0;
  int lineNumber = 0;
  std::string line;

  // Keep track of first-accepted category spellings
  std::vector<std::string> canonicalCategories;

  while (std::getline(inFile, line)) {
    lineNumber++;
    std::string trimmedLine = trim(line);

    // Ignore blank lines and comment lines
    if (trimmedLine.empty() || trimmedLine[0] == '#') {
      continue;
    }

    // Split line by '|'
    std::vector<std::string> fields;
    size_t start = 0;
    size_t pipePos = line.find('|');
    while (pipePos != std::string::npos) {
      fields.push_back(trim(line.substr(start, pipePos - start)));
      start = pipePos + 1;
      pipePos = line.find('|', start);
    }
    fields.push_back(trim(line.substr(start)));

    // 1. Check field count (must be 4: FacilityID|FacilityName|BuildingName|Category)
    if (fields.size() != 4) {
      std::cout << "Warning [" << displayFilename << " line " << lineNumber
                << "]: Expected 4 fields, found " << fields.size() << ". Record rejected." << std::endl;
      rejectedCount++;
      continue;
    }

    std::string rawId = fields[0];
    std::string facName = fields[1];
    std::string bldgName = fields[2];
    std::string catName = fields[3];

    // 2. Check for empty required fields
    if (rawId.empty() || facName.empty() || bldgName.empty() || catName.empty()) {
      std::cout << "Warning [" << displayFilename << " line " << lineNumber
                << "]: Empty required field. Record rejected." << std::endl;
      rejectedCount++;
      continue;
    }

    // 3. Normalize ID to uppercase, then validate format
    std::string normalizedId = toUpper(rawId);
    if (!isValidFacilityIdFormat(normalizedId)) {
      std::cout << "Warning [" << displayFilename << " line " << lineNumber
                << "]: Invalid facility ID format '" << rawId
                << "' (must be F followed by 3 digits). Record rejected." << std::endl;
      rejectedCount++;
      continue;
    }

    // 4. Validate building reference against accepted buildings from buildings.txt
    std::string canonicalBuilding = findCanonicalBuilding(bldgName, acceptedBuildings);
    if (canonicalBuilding.empty()) {
      std::cout << "Warning [" << displayFilename << " line " << lineNumber
                << "]: Unknown building reference '" << bldgName << "'. Record rejected." << std::endl;
      rejectedCount++;
      continue;
    }

    // 5. Check for duplicate facility ID in the custom hash table
    if (idTable.contains(normalizedId)) {
      std::cout << "Warning [" << displayFilename << " line " << lineNumber
                << "]: Duplicate facility ID '" << normalizedId << "'. Record rejected." << std::endl;
      rejectedCount++;
      continue;
    }

    // Preserve spelling from first accepted record for categories
    std::string finalCategory = catName;
    bool foundCat = false;
    for (size_t i = 0; i < canonicalCategories.size(); ++i) {
      if (equalsIgnoreCase(catName, canonicalCategories[i])) {
        finalCategory = canonicalCategories[i];
        foundCat = true;
        break;
      }
    }
    if (!foundCat) {
      canonicalCategories.push_back(catName);
    }

    // Accept record and store in both file-order vector and custom ID hash table
    Facility newFacility(normalizedId, facName, canonicalBuilding, finalCategory);
    idTable.insert(newFacility);
    facilities.push_back(newFacility);
    acceptedCount++;
  }

  // Print required LOAD-4 and HASH-7 summary line
  std::cout << displayFilename << ": " << acceptedCount << " accepted, "
            << rejectedCount << " rejected (hash capacity " << idTable.getCapacity()
            << ", entries " << idTable.getEntryCount()
            << ", collisions " << idTable.getCollisionCount() << ")" << std::endl;
}

// FAC-2: Display every accepted facility in file order, showing all fields and the total
void FacilityManager::displayAllFacilities() const {
  if (facilities.empty()) {
    std::cout << "No facilities are currently loaded." << std::endl;
    return;
  }

  std::cout << "\n--- All Accepted Facilities ---" << std::endl;
  for (size_t i = 0; i < facilities.size(); ++i) {
    std::cout << facilities[i].id << " | "
              << facilities[i].name << " | "
              << facilities[i].building << " | "
              << facilities[i].category << std::endl;
  }
  std::cout << "Total facilities: " << facilities.size() << std::endl;
}

// UI-5 & FAC-3: Prompt for ID and search
void FacilityManager::promptAndSearchById() const {
  std::cout << "Enter facility ID: ";
  std::string input;
  if (!std::getline(std::cin, input)) {
    return;
  }
  searchById(input);
}

// FAC-3: Trim and uppercase entered ID; distinguish invalid format, not found, and found
void FacilityManager::searchById(const std::string& rawId) const {
  std::string trimmed = trim(rawId);
  if (trimmed.empty()) {
    std::cout << "Error: Facility ID cannot be blank." << std::endl;
    return;
  }

  std::string normalizedId = toUpper(trimmed);
  if (!isValidFacilityIdFormat(normalizedId)) {
    std::cout << "Invalid format: an ID is F followed by exactly three digits (e.g., F001)." << std::endl;
    return;
  }

  const Facility* found = idTable.search(normalizedId);
  if (found == nullptr) {
    std::cout << "Not found: no facility with ID " << normalizedId << std::endl;
  } else {
    std::cout << "Found: " << found->id << " | " << found->name << " | "
              << found->building << " | " << found->category << std::endl;
  }
}

const std::vector<Facility>& FacilityManager::getFacilities() const {
  return facilities;
}

const HashTable& FacilityManager::getHashTable() const {
  return idTable;
}
