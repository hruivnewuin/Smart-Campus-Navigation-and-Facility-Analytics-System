// Purpose: Load and query the undirected campus adjacency list.
// Author: Isiah Ndanzia (Member B)
#include "CampusGraph.h"

#include <algorithm>
#include <fstream>
#include <ostream>
#include <queue>

// Remove surrounding whitespace while preserving spaces inside the name.
static std::string trim(const std::string& value) {
    const char* whitespace = " \t\r\n\f\v";
    const auto first = value.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return {};
    }
    return value.substr(first, value.find_last_not_of(whitespace) - first + 1);
}

// Normalize ASCII case for comparison.
static std::string nameKey(const std::string& name) {
    std::string key = trim(name);
    for (char& ch : key) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return key;
}

// Preserve empty fields so malformed records can be rejected.
static std::vector<std::string> splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t start = 0;
    while (true) {
        const auto end = line.find('|', start);
        fields.push_back(trim(line.substr(start, end - start)));
        if (end == std::string::npos) {
            return fields;
        }
        start = end + 1;
    }
}

// Apply common load rules. The record callback returns a rejection reason or empty text.
template <typename AddRecord>
static void loadRecords(const std::string& directory, const std::string& filename,
                        std::size_t expectedFields, AddRecord add, std::ostream& out) {
    const std::string path = directory + "/" + filename;
    std::ifstream file(path);
    if (!file) {
        out << "Error: cannot open " << path << '\n';
    }

    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t lineNumber = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty() || line.front() == '#') {
            continue;
        }

        const auto fields = splitFields(line);
        std::string reason;
        if (fields.size() != expectedFields) {
            reason = "wrong number of fields";
        } else if (std::find(fields.begin(), fields.end(), "") != fields.end()) {
            reason = "empty required field";
        } else {
            reason = add(fields);
        }

        if (reason.empty()) {
            ++accepted;
        } else {
            ++rejected;
            out << "Warning: " << filename << ':' << lineNumber << ": " << reason << '\n';
        }
    }
    out << filename << ": " << accepted << " accepted, " << rejected << " rejected\n";
}

// Load vertices before validating edges against them.
void CampusGraph::load(const std::string& directory, std::ostream& out) {
    buildings.clear();
    connections = 0;

    loadRecords(directory, "buildings.txt", 1,
        [this](const std::vector<std::string>& fields) -> std::string {
            const auto& name = fields[0];
            if (findBuilding(name) != missing) {
                return "duplicate building " + name;
            }
            buildings.push_back({name, nameKey(name), {}});
            return {};
        }, out);

    loadRecords(directory, "connections.txt", 2,
        [this](const std::vector<std::string>& fields) -> std::string {
            const Index a = findBuilding(fields[0]);
            const Index b = findBuilding(fields[1]);
            if (a == missing || b == missing) {
                return "unknown building " + fields[a == missing ? 0 : 1];
            }
            if (a == b) {
                return "self-connection " + fields[0];
            }
            auto& neighbors = buildings[a].neighbors;
            if (std::find(neighbors.begin(), neighbors.end(), b) != neighbors.end()) {
                return "duplicate connection " + fields[0] + " | " + fields[1];
            }
            neighbors.push_back(b);
            buildings[b].neighbors.push_back(a);
            ++connections;
            return {};
        }, out);
}

// Compare normalized keys in building insertion order.
CampusGraph::Index CampusGraph::findBuilding(const std::string& name) const {
    const std::string key = nameKey(name);
    for (Index i = 0; i < buildings.size(); ++i) {
        if (buildings[i].key == key) {
            return i;
        }
    }
    return missing;
}

// Supply the graph's spelling for a building reference from another module.
std::string CampusGraph::canonicalName(const std::string& name) const {
    const Index index = findBuilding(name);
    return index == missing ? std::string{} : buildings[index].name;
}

// Distinguish blank input from an unknown name.
CampusGraph::Index CampusGraph::checkBuilding(const std::string& name,
                                              std::ostream& out) const {
    if (trim(name).empty()) {
        out << "Error: building name cannot be blank.\n";
        return missing;
    }
    const Index index = findBuilding(name);
    if (index == missing) {
        out << "Error: unknown building " << trim(name) << ".\n";
    }
    return index;
}

// Display each vertex with its neighbors in file order.
void CampusGraph::printGraph(std::ostream& out) const {
    if (buildings.empty()) {
        out << "No buildings are loaded.\n";
    }
    for (const auto& building : buildings) {
        out << building.name << ':';
        if (building.neighbors.empty()) {
            out << " (no connections)";
        }
        for (Index neighbor : building.neighbors) {
            out << ' ' << buildings[neighbor].name;
        }
        out << '\n';
    }
}

// Display the shared totals used by both statistics commands.
void CampusGraph::printTotals(std::ostream& out) const {
    out << "Total buildings: " << buildings.size()
        << "\nTotal connections: " << connections << '\n';
}

// A vertex's degree is the number of entries in its adjacency list.
void CampusGraph::printDegrees(std::ostream& out) const {
    printTotals(out);
    for (const auto& building : buildings) {
        out << building.name << ": " << building.neighbors.size() << '\n';
    }
}

// Distance doubles as the visited marker. Record the first parent at enqueue.
std::vector<CampusGraph::Index> CampusGraph::bfs(Index start,
                                                std::vector<Visit>& visits) const {
    std::queue<Index> pending;
    std::vector<Index> order;
    visits[start].distance = 0;
    pending.push(start);
    while (!pending.empty()) {
        const Index current = pending.front();
        pending.pop();
        order.push_back(current);
        for (Index neighbor : buildings[current].neighbors) {
            if (visits[neighbor].distance == missing) {
                visits[neighbor].distance = visits[current].distance + 1;
                visits[neighbor].parent = current;
                pending.push(neighbor);
            }
        }
    }
    return order;
}

// Print all vertices reached by one BFS.
void CampusGraph::printReachable(const std::string& start, std::ostream& out) const {
    const Index source = checkBuilding(start, out);
    if (source == missing) {
        return;
    }
    std::vector<Visit> visits(buildings.size());
    const auto order = bfs(source, visits);
    for (Index index : order) {
        out << buildings[index].name << " (distance " << visits[index].distance << ")\n";
    }
    out << "Reachable buildings: " << order.size() << " out of " << buildings.size() << '\n';
}

// Trace the recorded parents from the destination back to the start.
void CampusGraph::printShortestPath(const std::string& start, const std::string& destination,
                                    std::ostream& out) const {
    const Index source = checkBuilding(start, out);
    const Index target = checkBuilding(destination, out);
    if (source == missing || target == missing) {
        return;
    }

    std::vector<Visit> visits(buildings.size());
    bfs(source, visits);
    if (visits[target].distance == missing) {
        out << "No path from " << buildings[source].name << " to " << buildings[target].name << ".\n";
        return;
    }

    std::vector<Index> path;
    for (Index index = target; index != missing; index = visits[index].parent) {
        path.push_back(index);
    }
    out << "Shortest path:\n";
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        out << buildings[*it].name << '\n';
    }
    out << "Connections: " << visits[target].distance << '\n';
}

// Reuse one visited array across BFS runs to count each component once.
void CampusGraph::printConnectivity(std::ostream& out) const {
    printTotals(out);
    std::size_t highestDegree = 0;
    std::size_t components = 0;
    std::vector<Visit> visits(buildings.size());
    for (Index i = 0; i < buildings.size(); ++i) {
        highestDegree = std::max(highestDegree, buildings[i].neighbors.size());
        if (visits[i].distance == missing) {
            bfs(i, visits);
            ++components;
        }
    }

    if (buildings.empty()) {
        out << "No buildings are loaded.\nHighest degree: None\n";
    } else {
        out << "Highest degree (" << highestDegree << "):";
        for (const auto& building : buildings) {
            if (building.neighbors.size() == highestDegree) {
                out << ' ' << building.name;
            }
        }
        out << '\n';
    }

    out << "Isolated buildings:";
    bool hasIsolate = false;
    for (const auto& building : buildings) {
        if (building.neighbors.empty()) {
            out << ' ' << building.name;
            hasIsolate = true;
        }
    }
    if (!hasIsolate) {
        out << " None";
    }
    out << "\nConnected components: " << components << '\n';
}

// Use line input so building names can contain spaces.
bool CampusGraph::handleMenuOption(int choice, bool finalMenu, std::istream& in,
                                   std::ostream& out) const {
    const int graphOption = finalMenu ? 5 : 3;
    if (choice == graphOption) {
        printGraph(out);
    } else if (choice == graphOption + 1) {
        printDegrees(out);
    } else if (choice == graphOption + 2) {
        out << "Starting building: ";
        std::string start;
        if (std::getline(in, start)) {
            printReachable(start, out);
        }
    } else if (finalMenu && choice == 8) {
        out << "Starting building: ";
        std::string start;
        if (!std::getline(in, start) || checkBuilding(start, out) == missing) {
            return true;
        }
        out << "Destination building: ";
        std::string destination;
        if (std::getline(in, destination)) {
            printShortestPath(start, destination, out);
        }
    } else if (finalMenu && choice == 17) {
        printConnectivity(out);
    } else {
        return false;
    }
    return true;
}
