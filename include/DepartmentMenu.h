#ifndef DEPARTMENT_MENU_H
#define DEPARTMENT_MENU_H
#include "DepartmentTree.h"

// contains declarations for menu handlers that are manage the tree department options
void menuAddDepartment(DepartmentTree& tree);
void menuSearchDepartment(const DepartmentTree& tree);
void menuDisplayHierarchy(const DepartmentTree& tree);
void menuPreorder(const DepartmentTree& tree);

#endif //DEPARTMENT_MENU_H
