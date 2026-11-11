#include "DepartmentMenu.h"
#include "StringUtils.h"
#include <iostream>
#include <string>

//Purpose: It prompts and puts out outputs for the department menu options - Made by Faith Paredes

//prompt for a name, it will return false if blank
static bool promptName(const std:: string& prompt, const std::string& what, std::string& out) {
  std::cout << prompt;
  std::string line;
  if (!std::getline(std::cin, line)) line.clear();
  out = StringUtils::trim(line);
  if (out.empty()) {
    std::cout << "Error: " << what << " cannot be blank.\n";
    return false;
  }
  return true;
}
//print a traversal as a one arrow seperated sequenced or it will be empty
static void printSequence(const std::string& title, const std::vector<std::string>& seq) {
  if (seq.empty()) {
    std::cout << "No departments are loaded.\n";
    return;
  }
  std::cout << title << ":\n";
  for (size_t i = 0; i < seq.size(); ++i) {
    if (i > 0) std::cout << " -> ";
    std::cout << seq[i];
  }
  std::cout << "\n";
}
//add a department in memory only
void menuAddDepartment(DepartmentTree& tree) {
  std::string name, parent, err;
  if (!promptName("Enter new Department name: ", "department name", name)) return;
  if (tree.isEmpty()) {
    if (tree.addRoot(name, err)) {
      std::cout << "Added '" << name << "' as the root department.\n";
    }
    else {
      std::cout << "Error " << err << ".\n";
    }
    return;
  }
  if (!promptName("Enter parent department name: ", "parent name", parent)) return;
  if (tree.addChild(name, parent, err)) {
    DepartmentInfo info;
    tree.getInfo(name, info);
    std::cout << "Added '" << info.name << "' under '" << info.parentName << "'.\n";
  }
  else {
    std::cout << "Error: " << err << ". Tree unchanged.\n";
  }
}
// searching by name while ignoring case
void menuSearchDepartment(const DepartmentTree& tree) {
  std::string name;
  if (!promptName("Enter department name to search: ", "departemnt name", name)) return;
  DepartmentInfo info;
  if (!tree.getInfo(name, info)) {
    if (tree.isEmpty()) {
      std::cout << "No departments are loaded.\n";
    }
    else {
      std::cout << "Department '" << name << "' not found.\n";
    }
    return;
  }
  std::cout << "Department: " << info.name << "\n" << "Parent: " << info.parentName << "\n" << "Direct children: " << info.childCount << "\n" << "Depth: " << info.depth << "\n";
}
//indented hierarchy view
void menuDisplayHierarchy(const DepartmentTree& tree) {
  if (tree.isEmpty()) { std::cout << "No departments are loaded.\n"; return; }
  tree.printHierarchy(std::cout);
}
//preorder sequence
void menuPreorder(const DepartmentTree& tree) {
  printSequence("Preorder traversal", tree.preorder());
}

//postorder sequence
void menuPostorder(const DepartmentTree& tree) {
  printSequence("Postorder traversal", tree.postorder());
}

//level order sequence
void menuLevelOrder(const DepartmentTree& tree) {
  printSequence("Level Order traversal", tree.LevelOrder());
}

void menuDepartmentStatistics(const DepartmentTree& tree) {
  if (tree.isEmpty()) {
    std::cout << "No departments are loaded.\n" << "Total departments: 0\n" << "Leaf departments: 0\n" << "Tree height: n/a\n";
    return;
  }
  std::cout << "Total departments: " << tree.size() << "\n" << "Leaf departments: " << tree.countLeaves() << "\n" << "Tree height: " << tree.height() << "\n";
}
    




