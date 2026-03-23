#pragma once
#include "model.h"
#include <string>

// Load a .nano file into a FlowGraph.
// Format:
//   version = "nanoprog@0"
//   [[node]]
//   guid = "42"
//   type = "osc~"
//   args = ["440"]
//   connections = ["42.out0->7.0"]
//   position = [100, 200]

bool load_nano(const std::string& path, FlowGraph& graph);
void save_nano_stream(std::ostream& f, const FlowGraph& graph);
std::string save_nano_string(const FlowGraph& graph);
bool save_nano(const std::string& path, const FlowGraph& graph);
bool load_nano_string(const std::string& data, FlowGraph& graph);
