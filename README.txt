Smart Campus Navigation and Facility Analytics System
-----------------------------------------------------

TEAM MEMBERS
------------
- Caleb Djan: facility records, custom hash table, ID search, name and category search, category report.
- Isiah Ndanzia: campus graph, BFS reachability, shortest path, connectivity report.
- Faith Paredes: department tree, traversals, department statistics

PROJECT STRUCTURE
-----------------
Project2/
  include/    -- Header Files (class declarations) --
  src/         -- Implementation Files & main.cpp --
  testdata/
  data/        -- the provided input files --
    buildings.txt
    departments.txt
    facilities.txt
    connections.txt
  README.txt
  CONTRIBUTION.txt
  design_document.txt
  test_results.txt
  .gitignore

HOW TO BUILD
-------------
From the Project1/ directory, on CELL or any g++ environment:
  g++ -std=c++11 -Wall -Wextra  -Iinclude src/*.cpp -o campus

HOW TO RUN
----------
  ./campus  - this will use the default data/
  ./campus testdata/<folder> - uses the testdata/

The program loads data/ files, then presents a menu driven interface:

=== Milestone 1 menu ===
1. View all facilities
2. Search facility by ID
3. View campus graph
4. Display graph counts and degrees
5. Find reachable buildings (BFS)
6. Add department
7. Search department
8. Display department hierarchy
9. Run preorder traversal
10. Exit



  
