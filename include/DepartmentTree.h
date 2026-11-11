#ifndef DEPARTMENT_TREE_H
#define DEPARTMENT_TREE_H
#include <ostream>
#include <string>
#include <vector>

//Purpose: Its the general tree for the department hierarchy - Made by Faith Paredes

//In this department, it has things sorted in insertion order
struct DepartmentNode {
  std::string name; //spelling comes from the first accepted record
  DepartmentNode* parent; // nullptr the root
  std::vector<DepartmentNode*> children; // any number of children
  int depth; //root is 0 and this is for the edge from root

  DepartmentNode(const std::string& n, DepartmentNode* p, int d) : name(n), parent(p), depth(d) {}
};

//details shown by the search department
struct DepartmentInfo {
  std::string name;
  std::string parentName; // none for root
  int childCount;
  int depth;
};

// when loading departments.txt
struct DepartmentLoadResult {
  int accepted;
  int rejected;
  bool opened;
};

class DepartmentTree {
public:
  DepartmentTree();
  ~DepartmentTree(); //frees nodes
  DepartmentTree(const DepartmentTree&) = delete;
  DepartmentTree& operator=(const DepartmentTree&) = delete;
  bool isEmpty() const;
  int size() const;

  //add the root when tree is empty and it will return false and set failure with error
  bool addRoot(const std::string& name, std::string& error);
  //matched ignoring case
  //will add a child if theres an existing parent
  bool addChild(const std::string& name, const std::string& parentName, std::string& error);
  //find department by name while ignoring case, if missing then nullptr
  const DepartmentNode* find(const std::string& name) const;
  //filling info for a departemnt
  //if not found then false
  bool getInfo(const std::string& name, DepartmentInfo& info) const;

  //loads departments.txt
  //full path to open
  DepartmentLoadResult loadFromFile(const std::string& path, const std::string& displayName);
  // traversals returns the department names
  //insertion order for visiting siblings
  //level order uses queue
  std::vector<std::string> preorder() const;
  std::vector<std::string> postorder() const;
  std::vector<std::string> levelOrder() const;
  //print hierachy in preorder
  //will use four indentation spaces for each line
  void printHierarchy(std::ostream& out) const;
  //height() is -1 for empty trees
  int countLeaves() const;
  int height() const;

private:
  DepartmentNode* root_;
  std::vector<DepartmentNode*> nodes_; //own all nodes in insertion order
  DepartmentNode* findMutable(const std::string& name) const;
  std::vector<const DepartmentNode*> preorderNodes() const;
};

#endif



  


