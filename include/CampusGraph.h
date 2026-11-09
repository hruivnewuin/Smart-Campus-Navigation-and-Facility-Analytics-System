// Purpose: Campus walkway graph and Member B menu interface.
// Author: Isiah Ndanzia (Member B)
#ifndef CAMPUS_GRAPH_H
#define CAMPUS_GRAPH_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

class CampusGraph {
public:
    // Load buildings and connections, replacing any previously loaded graph.
    void load(const std::string& directory, std::ostream& out);

    // Return the canonical building spelling, or an empty string for an unknown name.
    std::string canonicalName(const std::string& name) const;

    // Print the adjacency list in building and neighbor insertion order.
    void printGraph(std::ostream& out) const;

    // Print graph totals and each building's degree.
    void printDegrees(std::ostream& out) const;

    // Print reachable buildings in BFS order with their distances.
    void printReachable(const std::string& start, std::ostream& out) const;

    // Print a minimum-hop route between two named buildings.
    void printShortestPath(const std::string& start, const std::string& destination,
                           std::ostream& out) const;

    // Print totals, highest-degree buildings, isolates, and component count.
    void printConnectivity(std::ostream& out) const;

    // Handle a graph option using the chosen phase's numbering.
    // Return false if this option belongs to another module.
    bool handleMenuOption(int choice, bool finalMenu, std::istream& in,
                          std::ostream& out) const;

private:
    using Index = std::size_t;
    static constexpr Index missing = static_cast<Index>(-1);

    struct Building {
        std::string name;
        std::string key;
        std::vector<Index> neighbors;
    };

    struct Visit {
        Index distance = missing;
        Index parent = missing;
    };

    std::vector<Building> buildings;
    std::size_t connections = 0;

    // Find a building by its normalized name.
    Index findBuilding(const std::string& name) const;

    // Validate a user-entered building name and report an error if needed.
    Index checkBuilding(const std::string& name, std::ostream& out) const;

    // Mark vertices when enqueued and return this traversal's visit order.
    std::vector<Index> bfs(Index start, std::vector<Visit>& visits) const;

    // Print building and undirected edge totals.
    void printTotals(std::ostream& out) const;
};

#endif
