#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// Enumeration representing the type of each wrestler node.
// The None value is added here to use as a  sentinel when initializing wrestler
// nodes.
enum class WrestlerType {
    BabyFace,
    Heel,
    None,
};

// Denotes whether a node has been visited, not visited, or processed.
// Used in the BFS algorithm.
enum class NodeStatus {
    NotDiscovered,
    // A node has been visited but not processed yet.
    Discovered,
    Processed,
};

// A vertex in the graph, representing one wrestler.
struct WrestlerNode {
    // Name of the wrestler
    std::string name;
    // The type of the wrestler (babyface or heel)
    WrestlerType type;
    // Whether the node has been visited and processed yet
    NodeStatus status;
    // List of vertices adjacent to this node
    std::vector<WrestlerNode*> adjacent_nodes;
};

// Performs a Breadth-First Search starting the provided argument (start).
// Implemented using C++'s implementation of a FIFO queue.
// Sets the type of nodes in the graph to the opposite of their parent's type;
// so if a vertex's parent is a BabyFace, that vertex will become a Heel.
//
// Returns true if it's possible to designate some nodes as babyfaces and others
// as heels and have the edges connect a babyface and a heel, with the given
// start node. Otherwise, returns false.
bool bfs(WrestlerNode* start) {
    std::queue<WrestlerNode*> q;
    q.push(start);
    // Arbitrary choice to set first wrestler to BabyFace
    start->type = WrestlerType::BabyFace;

    while (!q.empty()) {
        // Get the first element, then remove it from the queue
        WrestlerNode* u = q.front();
        q.pop();

        // Iterate through all the nodes adjacent to u
        for (const auto& v : u->adjacent_nodes) {
            if (v->status == NodeStatus::NotDiscovered) {
                v->status = NodeStatus::Discovered;

                // Set wrestler type to opposite of what parent node is
                if (u->type == WrestlerType::BabyFace) {
                    v->type = WrestlerType::Heel;
                } else {
                    v->type = WrestlerType::BabyFace;
                }

                // Add v to the back of the queue
                q.push(v);
            // v has already been visited and has its type set, so check to make sure
            // its type is different from its parent's (u's) type. If they're the same,
            // we can stop, since we know it's not possible to find the requested
            // designation.
            } else if (u->type == v->type) {
                return false;
            }

            // u is completely processed now
            u->status = NodeStatus::Processed;
        }
    }

    return true;
}

// Uses Breadth-First Search to determine if it is possible to designate
// some nodes (wrestlers) as babyfaces and the remaining as heels such
// that every edge (rivalry) in the graph connects a babyface and a heel.
//
// Returns true if it's possible; otherwise, returns false
bool rivalries(std::vector<WrestlerNode*> nodes) {
    // In case the graph is disconnected, loop and do BFS starting at all
    // unvisited nodes
    for (const auto& u : nodes) {
        if (u->status == NodeStatus::NotDiscovered) {
            if (!bfs(u)) {
                return false;
            }
        }
    }

    return true;
}

// Helper template function to read a line and parse a value from the given input stream
// Reads an entire line from the istream, turns the line into a stringstream, and attempts to parse
// the line's contents into the given T reference; if the parsing fails, an exception is thrown
// with the err_msg argument
template <class T>
void stream_get_t(std::istream &is, T &t, const char* err_msg) {
    std::string line;
    std::getline(is, line);
    std::istringstream iss(line);
    iss >> t;
    if (!iss) {
        throw std::runtime_error(err_msg);
    }
}

// Reads the input file and parses its contents into an adjacency list that will
// be used in the Breadth-First Search algorithm
std::vector<WrestlerNode*> parse_file(const std::string& filename) {
    // Try to open the file for reading
    std::ifstream in;
    in.open(filename);
    if (!in) {
        throw std::runtime_error("Unable to read input file!");
    }

    // Stores the adjacency list
    std::vector<WrestlerNode*> nodes;
    // Used to quickly look up nodes when building adjacency list
    std::unordered_map<std::string, WrestlerNode*> wrestler_map;
    // Buffer to hold names being read in from the input file
    std::string name;

    // Get the first line and parse the integer as the number of wrestlers
    int wrestler_count;
    stream_get_t(in, wrestler_count, "Unable to read the number of wrestlers!");

    // Read and parse all the wrestlers based off the parsed wrestler count
    for (int i = 0; i < wrestler_count; ++i) {
        // Read the entire line, since it should only contain the wrestler name
        std::getline(in, name);

        // Resolves a weird error where lines end with a carriage return (\r) character
        if (name[name.size() - 1] == '\r') {
            name.pop_back();
        }

        // Initialize a new WrestlerNode with default values
        auto wrestler = new WrestlerNode;
        wrestler->name = name;
        wrestler->status = NodeStatus::NotDiscovered;
        wrestler->type = WrestlerType::None;

        nodes.push_back(wrestler);
        // Add the WrestlerNode to the map for quick lookup below
        wrestler_map.insert({name, wrestler});
    }

    // Try to parse the next line as the number of rivalries
    int rivalry_count;
    stream_get_t(in, rivalry_count, "Unable to read the number of rivalries!");

    // Buffers to hold the names of the two people in the rivalry
    std::string w1_name, w2_name;
    // Holds the WrestlerNode objects of the two people in the rivalry
    WrestlerNode *w1, *w2;

    // Add the adjacent nodes for each wrestler
    for (int i = 0; i < rivalry_count; ++i) {
        in >> w1_name;
        in >> w2_name;

        // Quickly look up the wrestlers based on the names from the hash map
        w1 = wrestler_map[w1_name];
        w2 = wrestler_map[w2_name];

        // w1 and w2 are both adjacent to each other, so add each other to their
        // adjacency lists
        w1->adjacent_nodes.push_back(w2);
        w2->adjacent_nodes.push_back(w1);
    }

    return nodes;
}

int main(int argc, char** argv) {
    // Should only have one argument to the program (the name of the input file)
    if (argc != 2) {
        std::cerr << "Expected one argument to program: input filename" << std::endl;
        return 5;
    }

    // Parse the input then run the algorithm to check if the designations are possible
    std::vector<WrestlerNode*> nodes = parse_file(argv[1]);
    bool is_possible = rivalries(nodes);

    // If they're possible, print Yes and the list of babyfaces and heels
    if (is_possible) {
        std::cout << "Yes\n";

        std::vector<std::string> babyfaces, heels;

        for (const auto& u : nodes) {
            if (u->type == WrestlerType::BabyFace) {
                babyfaces.push_back(u->name);
            } else {
                heels.push_back(u->name);
            }
        }

        std::cout << "Babyfaces:";
        for (const auto& name : babyfaces) std::cout << ' ' << name;
        std::cout << '\n';

        std::cout << "Heels:";
        for (const auto& name : heels) std::cout << ' ' << name;
        std::cout << std::endl;
    // Otherwise, just print No
    } else {
        std::cout << "No" << std::endl;
    }

    // Free all the dynamically allocated WrestlerNodes
    for (const auto& u : nodes) {
        delete u;
    }

    return 0;
}