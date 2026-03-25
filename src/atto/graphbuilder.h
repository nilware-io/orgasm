#pragma once
#include "model.h"
#include "types.h"
#include "args.h"
#include "node_types.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

struct GraphBuilder {
    FlowGraph graph;
    TypePool pool;

    // Primary add: takes pre-parsed args (must not be null — throws on null)
    FlowNode& add(const std::string& id, NodeTypeID type, std::unique_ptr<ParsedArgs> args,
                  int num_inputs = -1, int num_outputs = -1);

    void link(const std::string& from, const std::string& to);

    FlowNode* find(const std::string& id);
    FlowPin* find_pin(const std::string& pin_id);

    std::vector<std::string> run_inference();
    std::vector<std::string> run_full_pipeline();
};

// Deserializer: string-based node creation with error handling.
// Wraps GraphBuilder, handles split/parse/error-fallback.
struct Deserializer {
    GraphBuilder* builder;

    explicit Deserializer(GraphBuilder* b) : builder(b) {}

    // Add a node from raw strings. On parse failure, creates an Error node.
    FlowNode& add(const std::string& id, const std::string& type, const std::string& args_str,
                  int num_inputs = -1, int num_outputs = -1);
};
