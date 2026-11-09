/*
Purpose: Define the facility data structure representing a campus resource 
w/ normalized ID, name, building name, and category.
Author: Caleb D
*/

#ifndef FACILITY_H
#define FACILITY_H
#include <string>

struct Facility{
  std::string id; //creating ID
  std::string name; // Creating name for the facility names
  std::string building; // For building spellings
  std::string category; // For facility category

Facility(){}

Facility(const std::string& id, const std::string& name, const std::string& building, const std::string& category): id(id), name(name), building(building), category(category){}
};

#endif // FACILITY_H
