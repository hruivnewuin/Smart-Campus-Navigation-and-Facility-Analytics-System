#include "DepartmentTree.h"
#include "StringUtils.h"
#include <fstream>
#include <iostream>
using StringUtils::equalsIgnoreCase;
using StringUtils::trim;

//create an empty tree
DepartmentTree::DepartmentTree() : root_(nullptr) {}
//nodes_ is creates and destroys the node objects
DepartmentTree::~DepartmentTree() {
  for (size_t i = 0; i < nodes_.sizes(); ++i) delete nodes_[i];
  nodes_.clear();
  root_ = nullptr;
}
//when no department is loaded
bool DepartmentTree::isEmpty() const { return root_ == nullptr; }
//total # of departments
int DepartmentTree::size() const { return static_cast<int>(nodes_.size()); }
//ingores case while doing linear scan
DepartmentNode* DepartmentTree::findMutable(const std::string& name) const {
  std::string key = trim(name);
  for (size_t i = 0; i < nodes_.size(); ++i) {
    if (equalsIgnoreCase(nodes_[i]->name, key)) return nodes_[i];
  }
  return nullptr;
}
//public const lookup
const DepartmentNode* DepartmentTree::find(const std::string& name const {
  return findMutable(name);
}

//makes a department under an existing parent
bool DepartmentTree::addChild(const std::string& name, const std::string& parent Name, std::string& error) {
  std::string n + trim(name);
  std::string p + trim(parentName);
  if (n.empty()) { error = "department name is empty"; return false; }
  if (p.empty()) { error = "parent name is empty"; return false; }
  if (findMutable(n) != nullptr) {
    error = "duplicate department name '" + n + "'";
    return false;
  }
  DepartmentNode* parent = findMutable(p);
  if (parent == nullptr) {
    error = "unknown parent '" + p + "'";
    return false;
  }
  DepartmentNode* node = new DepartmentNode(n, parent, parent->depth + 1);
  parent->children.push_back(node);
  nodes_.push_back(node);
  return true;
}
//collect name, parent (except when root then its none), child count, and depth
bool DepartmentTree::getInfo(const std::string& name, DepartmentInfo& info) const {
  const DepartmentNode* n = find(name);
  if (n == nullptr) return false;
  info.name = n->name;
  info.parentName = (n->parent == nullptr) ? "None" : n->parent->name;
  info.childCount = static_cast<int>(n->children.size());
  info.depth = n->depth;
  return true;
}

//loads departments.txt
DepartmentLoadResult DepartmentTree::loadFromFile(const std::string& path, const std::string& displayName) {
  DepartmentLoadResult result;
  result.accepted = 0;
  result.rejected = 0;
  result.opened = false;
  std::ifstream in(path.c_str());
  if (!in) {
    std::cout << "Error: could not open " << displayName << " -treating it as empty.\n";
    std::cout << displayName << ": 0 accepted, 0 rejected\n";
    return result;
  }
  result.opened = true;
  std::string line;
  int lineNo = 0;
  while (std::getline(in, line)) {
    ++lineNo;
    if (StringUtils::isBlankOrComment(line)) continue;
    std::vector<std::string> f = StringUtils::splitFields(line);
    std::string reason;

    if (f.size() != 2) {
      reason = "expected two fields (DepartmentName|ParentName) but found " + std::to_string(f.size());
    }
    else if (f[0].empty() || f[1].empty()) {
      reason = "empty required field";
    }
    else if (findMutable(f[0]) != nullptr) {
      reason = "duplicate department name '" + f[0] + "'";
    }
    else if (root_ == nullptr) {
      if (!equalsIgnoreCase(f[1], "NONE")) { //valid record is the root and uses NONE as parent
        reason = "first department must be the root with parent NONE (found '" + f[1] + "')";
      }
    }
    else if (equalsIgnoreCase(f[1], "NONE")) {
      
    }



    
