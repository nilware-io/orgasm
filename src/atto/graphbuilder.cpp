#include "graphbuilder.h"
#include "args.h"
#include "expr.h"
#include "node_types.h"
#include "shadow.h"
#include "inference.h"
#include "type_utils.h"
#include <cstdio>

// ─── GraphBuilder ───

FlowNode& GraphBuilder::add(const std::string& id, NodeTypeID type, std::unique_ptr<ParsedArgs> parsed_args,
                             int num_inputs, int num_outputs) {
    if (!parsed_args)
        throw std::invalid_argument("GraphBuilder::add: parsed_args must not be null");

    auto* nt = find_node_type(type);
    bool is_expr = is_any_of(type, NodeTypeID::Expr, NodeTypeID::ExprBang);
    int di = nt ? nt->inputs : 0;
    int nbi = nt ? nt->num_triggers : 0;
    int nbo = nt ? nt->num_nexts : 0;
    int no = (num_outputs >= 0) ? num_outputs : (nt ? nt->outputs : 1);

    std::string args_str = reconstruct_args_str(*parsed_args);

    FlowNode node;
    node.id = graph.next_node_id();
    node.node_id = id;
    node.guid = (id.size() > 1 && id[0] == '$') ? id.substr(1) : id;
    node.type_id = type;
    node.args = args_str;
    node.position = {0, 0};

    for (int i = 0; i < nbi; i++)
        node.triggers.push_back(make_pin("", "bang_in" + std::to_string(i), "", nullptr, FlowPin::BangTrigger));

    if (is_expr) {
        int ni = (num_inputs >= 0) ? num_inputs : parsed_args->total_pin_count(di);
        for (int i = 0; i < ni; i++) {
            bool il = parsed_args->is_lambda_slot(i);
            std::string pn = il ? ("@" + std::to_string(i)) : std::to_string(i);
            node.inputs.push_back(make_pin("", pn, "", nullptr, il ? FlowPin::Lambda : FlowPin::Input));
        }
        if (!args_str.empty() && num_outputs < 0) {
            auto tokens = tokenize_args(args_str, false);
            no = std::max(1, (int)tokens.size());
        }
    } else if (is_any_of(type, NodeTypeID::Cast, NodeTypeID::New)) {
        int ni = (num_inputs >= 0) ? num_inputs : di;
        for (int i = 0; i < ni; i++) {
            std::string pn; std::string pt; bool il = false;
            if (nt && nt->input_ports && i < nt->inputs) {
                pn = nt->input_ports[i].name;
                il = (nt->input_ports[i].kind == PortKind::Lambda);
                if (nt->input_ports[i].type_name) pt = nt->input_ports[i].type_name;
            } else pn = std::to_string(i);
            node.inputs.push_back(make_pin("", pn, pt, nullptr, il ? FlowPin::Lambda : FlowPin::Input));
        }
    } else {
        int ref_pins = (parsed_args->max_slot >= 0) ? (parsed_args->max_slot + 1) : 0;
        if (num_inputs >= 0) ref_pins = num_inputs;
        for (int i = 0; i < ref_pins; i++) {
            bool il = parsed_args->is_lambda_slot(i);
            std::string pn = il ? ("@" + std::to_string(i)) : std::to_string(i);
            node.inputs.push_back(make_pin("", pn, "", nullptr, il ? FlowPin::Lambda : FlowPin::Input));
        }
        int num_inline = (int)parsed_args->args.size();
        for (int i = num_inline; i < di; i++) {
            std::string pn; std::string pt; bool il = false;
            if (nt && nt->input_ports && i < nt->inputs) {
                pn = nt->input_ports[i].name;
                il = (nt->input_ports[i].kind == PortKind::Lambda);
                if (nt->input_ports[i].type_name) pt = nt->input_ports[i].type_name;
            } else pn = std::to_string(i);
            node.inputs.push_back(make_pin("", pn, pt, nullptr, il ? FlowPin::Lambda : FlowPin::Input));
        }
    }

    for (int i = 0; i < no; i++)
        node.outputs.push_back(make_pin("", "out" + std::to_string(i), "", nullptr, FlowPin::Output));
    for (int i = 0; i < nbo; i++) {
        std::string bname = (nt && nt->next_ports && i < nt->num_nexts) ? nt->next_ports[i].name : ("bang" + std::to_string(i));
        node.nexts.push_back(make_pin("", bname, "", nullptr, FlowPin::BangNext));
    }

    node.rebuild_pin_ids();
    node.parse_args();
    graph.nodes.push_back(std::move(node));
    return graph.nodes.back();
}

void GraphBuilder::link(const std::string& from, const std::string& to) {
    graph.add_link(from, to);
}

FlowNode* GraphBuilder::find(const std::string& id) {
    for (auto& n : graph.nodes) if (n.guid == id || n.node_id == id) return &n;
    return nullptr;
}

FlowPin* GraphBuilder::find_pin(const std::string& pin_id) {
    return graph.find_pin(pin_id);
}

std::vector<std::string> GraphBuilder::run_inference() {
    resolve_type_based_pins(graph);
    generate_shadow_nodes(graph);
    GraphInference inference(pool);
    return inference.run(graph);
}

std::vector<std::string> GraphBuilder::run_full_pipeline() {
    resolve_type_based_pins(graph);
    generate_shadow_nodes(graph);
    GraphInference inference(pool);
    return inference.run(graph);
}

// ─── Deserializer ───

static FlowNode& make_error_node(FlowGraph& graph, const std::string& id,
                                  const std::string& type, const std::string& args_str,
                                  const std::string& error_msg) {
    FlowNode node;
    node.id = graph.next_node_id();
    node.node_id = id;
    node.guid = (id.size() > 1 && id[0] == '$') ? id.substr(1) : id;
    node.type_id = NodeTypeID::Error;
    node.args = type + " " + args_str;
    node.error = error_msg;
    node.position = {0, 0};
    node.rebuild_pin_ids();
    graph.nodes.push_back(std::move(node));
    return graph.nodes.back();
}

FlowNode& Deserializer::add(const std::string& id, const std::string& type, const std::string& args_str,
                              int num_inputs, int num_outputs) {
    NodeTypeID type_id = node_type_id_from_string(type.c_str());

    if (type_id == NodeTypeID::Unknown) {
        return make_error_node(builder->graph, id, type, args_str, "Unknown node type: " + type);
    }

    // Labels and errors don't need parsing
    if (is_any_of(type_id, NodeTypeID::Label, NodeTypeID::Error)) {
        auto parsed = std::make_unique<ParsedArgs>();
        if (!args_str.empty()) {
            parsed->args.push_back(ArgString{args_str});
            parsed->has_any_args = true;
        }
        return builder->add(id, type_id, std::move(parsed), num_inputs, num_outputs);
    }

    // Split args
    auto split_result = split_args(args_str);
    if (auto* err = std::get_if<std::string>(&split_result)) {
        return make_error_node(builder->graph, id, type, args_str, *err);
    }

    auto& exprs = std::get<std::vector<std::string>>(split_result);
    bool is_expr = is_any_of(type_id, NodeTypeID::Expr, NodeTypeID::ExprBang);

    // Parse args
    auto parse_result = parse_args_v2(exprs, is_expr);
    if (auto* err = std::get_if<std::string>(&parse_result)) {
        return make_error_node(builder->graph, id, type, args_str, *err);
    }

    auto parsed = std::get<std::unique_ptr<ParsedArgs>>(std::move(parse_result));
    return builder->add(id, type_id, std::move(parsed), num_inputs, num_outputs);
}
