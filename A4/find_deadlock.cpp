//Ali Kirmani 30115539
//CPSC 457
//A4 Q1 Deadlock

#include "find_deadlock.h"
#include <unordered_map>
#include <vector>

class NodeFind {
    int counter = 0;
    std::unordered_map<int, std::string> id_to_string;
    std::unordered_map<std::string, int> string_to_id;

public:
    int getId(const std::string& str) {
        auto it = string_to_id.find(str);

        if (it == string_to_id.end()) {
            id_to_string[counter] = str;
            string_to_id[str] = counter;
            return counter++;
        } else {
            return it->second;
        }
    }

    std::string getString(int id) const {
        auto it = id_to_string.find(id);
        if (it == id_to_string.end()) {
            return "";
        }
        return it->second;
    }

    size_t size() const {
        return counter;
    }
} dictionary;

class Graph {
public:
    std::vector<std::vector<int>> adjacency;
    std::vector<int> out_degrees;
};

namespace EdgeSigns {
    const std::string PULL = "->";
    const std::string SET = "<-";
}

Result find_deadlock(const std::vector<std::string>& edges) {
    Result result;

    Graph graph;

    for (size_t i = 0; i < edges.size(); ++i) {
        std::string edge = edges[i];
        std::vector<std::string> tokens = split_line(edge);

        if (tokens.size() != 3) {
            continue; // Skip invalid edge
        }

        std::string sign = tokens[1];
        if (sign != EdgeSigns::PULL && sign != EdgeSigns::SET) {
            continue; // Skip invalid sign
        }

        int from = dictionary.getId("[P]" + tokens[0]);
        int to = dictionary.getId("[R]" + tokens[2]);

        // Resize graph if necessary
        size_t graphSize = dictionary.size();
        if (graphSize > graph.out_degrees.size()) {
            graph.out_degrees.resize(graphSize);
            graph.adjacency.resize(graphSize);
        }

        // Update graph
        graph.adjacency[to].push_back(from);
        graph.out_degrees[from]++;

        // Detect cycle
        std::vector<int> out_degrees = graph.out_degrees;
        std::vector<int> zeros;
        for (size_t i = 0; i < out_degrees.size(); ++i) {
            if (out_degrees[i] == 0) {
                zeros.push_back(i);
            }
        }

        // Detect cycle and update out degrees
        bool cycle_detected = false;
        while (!zeros.empty() && !cycle_detected) {
            int node = zeros.back();
            zeros.pop_back();
            for (int adj : graph.adjacency[node]) {
                out_degrees[adj]--;

                if (out_degrees[adj] == 0) {
                    zeros.push_back(adj);
                }
                // Check for cycle
                if (out_degrees[adj] < 0) {
                    cycle_detected = true;
                    break;
                }
            }
        }

        // Find processes involved in deadlock
        for (size_t i = 0; i < out_degrees.size(); ++i) {
            if (out_degrees[i] != 0) {
                std::string node = dictionary.getString(i);
                if (!node.empty() && node.find("[P]") != std::string::npos) {
                    node.replace(0, 3, ""); // Remove prefix
                    result.procs.push_back(node);
                }
            }
        }

        // Set index if deadlock detected and exit loop
        if (!result.procs.empty()) {
            result.index = i;
            bool deadlock_found = true;
            break;
        }
    }

    // Set index to -1 if no deadlock
    if (result.procs.empty()) {
        result.index = -1;
    }

    return result;
}
