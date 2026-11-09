/*
Author: Caleb Djan, Isiah Ndanzia, Faith Paredes
Purpose: Main entry point for the Smart Campus Navigation and Facility
         Analytics System. 
Loads campus data files in the required order and runs the interactive menu loop.
*/

#include "CampusGraph.h"
#include "DepartmentMenu.h"
#include "DepartmentTree.h"
#include "FacilityManager.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Collects the names of all accepted buildings from CampusGraph
static std::vector<std::string> getAcceptedBuildings(const std::string& buildingsPath, const CampusGraph& graph) {
  std::vector<std::string> accepted;
  std::ifstream inFile(buildingsPath.c_str());
  if (!inFile.is_open()) {
    return accepted;
  }

  std::string line;
  while (std::getline(inFile, line)) {
    std::string trimmed = FacilityManager::trim(line);
    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    // Extract first field (building name) if pipe-delimited or single-column
    size_t pipePos = trimmed.find('|');
    std::string candidate = (pipePos == std::string::npos)
                                ? trimmed
                                : FacilityManager::trim(trimmed.substr(0, pipePos));

    std::string canonical = graph.canonicalName(candidate);
    if (!canonical.empty()) {
      bool alreadyAdded = false;
      for (size_t i = 0; i < accepted.size(); ++i) {
        if (FacilityManager::equalsIgnoreCase(accepted[i], canonical)) {
          alreadyAdded = true;
          break;
        }
      }
      if (!alreadyAdded) {
        accepted.push_back(canonical);
      }
    }
  }
  return accepted;
}

static void printMenu() {
  std::cout << "\n=== Milestone 1 Menu ===" << std::endl;
  std::cout << "1. View all facilities" << std::endl;
  std::cout << "2. Search facility by ID" << std::endl;
  std::cout << "3. View campus graph" << std::endl;
  std::cout << "4. Display graph counts and degrees" << std::endl;
  std::cout << "5. Find reachable buildings (BFS)" << std::endl;
  std::cout << "6. Add department" << std::endl;
  std::cout << "7. Search department" << std::endl;
  std::cout << "8. Display department hierarchy" << std::endl;
  std::cout << "9. Run preorder traversal" << std::endl;
  std::cout << "10. Exit" << std::endl;
  std::cout << "Enter choice: ";
}

int main(int argc, char* argv[]) {
  // Determine data directory
  std::string dataDir = "data";
  if (argc > 1 && argv[1] != nullptr) {
    std::string arg = FacilityManager::trim(argv[1]);
    if (!arg.empty()) {
      // Remove trailing slash (if present)
      while (arg.length() > 1 && (arg[arg.length() - 1] == '/' || arg[arg.length() - 1] == '\\')) {
        arg.erase(arg.length() - 1);
      }
      dataDir = arg;
    }
  }

 // 1 & 2. Load buildings.txt and connections.txt via CampusGraph
  CampusGraph campusGraph;
  campusGraph.load(dataDir, std::cout);

// 3. Load facilities.txt via FacilityManager using accepted buildings
  std::vector<std::string> acceptedBuildings =
      getAcceptedBuildings(dataDir + "/buildings.txt", campusGraph);
  FacilityManager facilityManager;
  facilityManager.loadFromFile(dataDir + "/facilities.txt", acceptedBuildings);

// 4. Load departments.txt via DepartmentTree
  DepartmentTree departmentTree;
  departmentTree.loadFromFile(dataDir + "/departments.txt", "departments.txt");

// Interactive Menu Loop
  std::string input;
  while (true) {
    printMenu();
    if (!std::getline(std::cin, input)) {
      std::cout << "\nEnd of input reached. Exiting program." << std::endl;
      break;
    }

    std::string trimmedInput = FacilityManager::trim(input);
    if (trimmedInput.empty()) {
      std::cout << "Invalid choice: input cannot be blank. Please enter a number (1-10)."
                << std::endl;
      continue;
    }

// Checks & validates that the user entered an integer
    char* endPtr = nullptr;
    long choice = std::strtol(trimmedInput.c_str(), &endPtr, 10);
    if (*endPtr != '\0') {
      std::cout << "Invalid choice: '" << trimmedInput
                << "' is not a valid integer. Please enter a number (1-10)." << std::endl;
      continue;
    }

// Support both 10 (README menu) and 0 (UI-7 exit option)
    if (choice == 10 || choice == 0) {
      std::cout << "Exiting program. Goodbye!" << std::endl;
      break;
    }

    switch (choice) {
      case 1:
        facilityManager.displayAllFacilities();
        break;
      case 2:
        facilityManager.promptAndSearchById();
        break;
      case 3:
      case 4:
      case 5:
        campusGraph.handleMenuOption(static_cast<int>(choice), false, std::cin, std::cout);
        break;
      case 6:
        menuAddDepartment(departmentTree);
        break;
      case 7:
        menuSearchDepartment(departmentTree);
        break;
      case 8:
        menuDisplayHierarchy(departmentTree);
        break;
      case 9:
        menuPreorder(departmentTree);
        break;
      default:
        std::cout << "Invalid choice: " << choice
                  << " is out of range. Please select 1-10." << std::endl;
        break;
    }
  }

  return 0;
}
