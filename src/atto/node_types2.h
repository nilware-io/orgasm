#pragma once
#include "node_types.h" // for NodeTypeID

// New pin model: flattened inputs/outputs, optional, va_args

enum class PortKind2 : uint8_t {
    BangTrigger,  // bang input (rendered as square, top)
    Data,         // data input/output
    Lambda,       // lambda capture (only accepts node refs)
    BangNext,     // bang output (rendered as square)
};

struct PortDesc2 {
    const char* name;
    const char* desc;
    PortKind2 kind = PortKind2::Data;
    const char* type_name = nullptr;
    bool optional = false;
    bool va_args = false;   // last pin only: repeats as name_0, name_1, ...
};

struct NodeType2 {
    NodeTypeID type_id;
    const char* name;
    const char* desc;
    const PortDesc2* input_ports;
    int num_inputs;
    const PortDesc2* output_ports;
    int num_outputs;
    bool is_event = false;
    bool is_declaration = false;
};

// ─── Port descriptor arrays ───

// Common outputs
static const PortDesc2 P2_NEXT[]       = {{"next", "fires after completion", PortKind2::BangNext}};
static const PortDesc2 P2_RESULT[]     = {{"result", "result value"}};
static const PortDesc2 P2_NEXT_RESULT[] = {{"next", "fires after completion", PortKind2::BangNext}, {"result", "result value"}};

// Common inputs
static const PortDesc2 P2_BANG_IN[]    = {{"bang_in", "trigger input", PortKind2::BangTrigger}};
static const PortDesc2 P2_VALUE[]      = {{"value", "input value"}};

// expr! inputs
static const PortDesc2 P2_EXPR_BANG_IN[] = {{"bang_in", "trigger input", PortKind2::BangTrigger}};

// store! inputs: bang, target, value
static const PortDesc2 P2_STORE_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"target", "variable/reference to store into"},
    {"value", "value to store"},
};

// store (no bang) inputs: target, value
static const PortDesc2 P2_STORE_IN[] = {
    {"target", "variable/reference to store into"},
    {"value", "value to store"},
};

// append! inputs: bang, target, value
static const PortDesc2 P2_APPEND_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"target", "collection to append to"},
    {"value", "value to append"},
};

// append (no bang) inputs: target, value
static const PortDesc2 P2_APPEND_IN[] = {
    {"target", "collection to append to"},
    {"value", "value to append"},
};

// erase inputs
static const PortDesc2 P2_ERASE_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"target", "collection to erase from"},
    {"key", "key/value/iterator to erase"},
};
static const PortDesc2 P2_ERASE_IN[] = {
    {"target", "collection to erase from"},
    {"key", "key/value/iterator to erase"},
};

// select inputs: condition, if_true, if_false
static const PortDesc2 P2_SELECT_IN[] = {
    {"condition", "boolean selector"},
    {"if_true", "value when true"},
    {"if_false", "value when false"},
};

// select! inputs: bang, condition
static const PortDesc2 P2_SELECT_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"condition", "boolean condition"},
};
// select! outputs: next, true, false
static const PortDesc2 P2_SELECT_BANG_OUT[] = {
    {"next", "fires after branch completes", PortKind2::BangNext},
    {"true", "fires when true", PortKind2::BangNext},
    {"false", "fires when false", PortKind2::BangNext},
};

// new: va_args fields
static const PortDesc2 P2_NEW_IN[] = {
    {"field", "constructor field", PortKind2::Data, nullptr, true, true},
};

// call: va_args arguments
static const PortDesc2 P2_CALL_IN[] = {
    {"arg", "function argument", PortKind2::Data, nullptr, true, true},
};
// call!: bang + va_args
static const PortDesc2 P2_CALL_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"arg", "function argument", PortKind2::Data, nullptr, true, true},
};

// iterate: collection + fn(lambda)
static const PortDesc2 P2_ITERATE_IN[] = {
    {"collection", "collection to iterate over"},
    {"fn", "it=fn(it); while it!=end", PortKind2::Lambda},
};
static const PortDesc2 P2_ITERATE_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"collection", "collection to iterate over"},
    {"fn", "it=fn(it); while it!=end", PortKind2::Lambda},
};

// lock: mutex + fn(lambda) + optional va_args params
static const PortDesc2 P2_LOCK_IN[] = {
    {"mutex", "mutex to lock"},
    {"fn", "body under lock", PortKind2::Lambda},
    {"param", "lambda parameter", PortKind2::Data, nullptr, true, true},
};
static const PortDesc2 P2_LOCK_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"mutex", "mutex to lock"},
    {"fn", "body under lock", PortKind2::Lambda},
    {"param", "lambda parameter", PortKind2::Data, nullptr, true, true},
};

// decl inputs
static const PortDesc2 P2_DECL_TYPE_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"name", "type name (symbol)"},
    {"type", "type definition"},
};
static const PortDesc2 P2_DECL_TYPE_OUT[] = {
    {"next", "fires after declaration", PortKind2::BangNext},
    {"type", "the declared type"},
};
static const PortDesc2 P2_DECL_VAR_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"name", "variable name (symbol)"},
    {"type", "variable type"},
};
static const PortDesc2 P2_DECL_VAR_OUT[] = {
    {"next", "fires after declaration", PortKind2::BangNext},
    {"ref", "reference to variable"},
};
static const PortDesc2 P2_DECL_OUT[] = {{"next", "fires to start declarations", PortKind2::BangNext}};
static const PortDesc2 P2_DECL_EVENT_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"name", "event name (symbol)"},
    {"type", "event function type"},
};
static const PortDesc2 P2_DECL_IMPORT_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"path", "module path", PortKind2::Data, "literal<string,?>"},
};
static const PortDesc2 P2_FFI_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"name", "function name (symbol)"},
    {"type", "function type"},
};

// discard
static const PortDesc2 P2_DISCARD_BANG_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"value", "value to discard"},
};

// output_mix!
static const PortDesc2 P2_OUTPUT_MIX_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"value", "audio sample to mix"},
};

// resize!
static const PortDesc2 P2_RESIZE_IN[] = {
    {"bang_in", "trigger", PortKind2::BangTrigger},
    {"target", "vector to resize"},
    {"size", "new size", PortKind2::Data, "s32"},
};

// on_key outputs: next + data
static const PortDesc2 P2_KEY_OUT[] = {
    {"next", "fires on key event", PortKind2::BangNext},
    {"midi_key", "MIDI note number", PortKind2::Data, "u8"},
    {"freq", "frequency in Hz", PortKind2::Data, "f32"},
};
// on_key_up outputs: next + midi_key only
static const PortDesc2 P2_KEY_UP_OUT[] = {
    {"next", "fires on key release", PortKind2::BangNext},
    {"midi_key", "MIDI note number", PortKind2::Data, "u8"},
    {"freq", "frequency in Hz", PortKind2::Data, "f32"},
};

// event! outputs
static const PortDesc2 P2_EVENT_OUT[] = {{"next", "fires on event", PortKind2::BangNext}};

// ─── Node type table ───

static const NodeType2 NODE_TYPES2[] = {
    // expr: no fixed inputs, outputs = args count
    {NodeTypeID::Expr,          "expr",       "Evaluate expression",
     nullptr, 0, P2_RESULT, 1, false, false},

    // select: 3 fixed inputs, 1 output
    {NodeTypeID::Select,        "select",     "Select value by condition",
     P2_SELECT_IN, 3, P2_RESULT, 1, false, false},

    // new: va_args fields, 1 output
    {NodeTypeID::New,           "new",        "Instantiate a type",
     P2_NEW_IN, 1, P2_RESULT, 1, false, false},

    // dup: 1 input, 1 output
    {NodeTypeID::Dup,           "dup",        "Duplicate input to output",
     P2_VALUE, 1, P2_RESULT, 1, false, false},

    // str: 1 input, 1 output
    {NodeTypeID::Str,           "str",        "Convert to string",
     P2_VALUE, 1, P2_RESULT, 1, false, false},

    // void: no inputs, 1 output
    {NodeTypeID::Void,          "void",       "Void result",
     nullptr, 0, P2_RESULT, 1, false, false},

    // discard!: bang + value, next output
    {NodeTypeID::DiscardBang,   "discard!",   "Discard value, pass bang",
     P2_DISCARD_BANG_IN, 2, P2_NEXT, 1, false, false},

    // discard: 1 input, no outputs
    {NodeTypeID::Discard,       "discard",    "Discard input values",
     P2_VALUE, 1, nullptr, 0, false, false},

    // decl_type
    {NodeTypeID::DeclType,      "decl_type",  "Declare a type",
     P2_DECL_TYPE_IN, 3, P2_DECL_TYPE_OUT, 2, false, true},

    // decl_var
    {NodeTypeID::DeclVar,       "decl_var",   "Declare a variable",
     P2_DECL_VAR_IN, 3, P2_DECL_VAR_OUT, 2, false, true},

    // decl
    {NodeTypeID::Decl,          "decl",       "Compile-time entry point",
     nullptr, 0, P2_DECL_OUT, 1, false, true},

    // decl_event
    {NodeTypeID::DeclEvent,     "decl_event", "Declare event",
     P2_DECL_EVENT_IN, 3, P2_NEXT, 1, false, true},

    // decl_import
    {NodeTypeID::DeclImport,    "decl_import","Import module",
     P2_DECL_IMPORT_IN, 2, P2_NEXT, 1, false, true},

    // ffi
    {NodeTypeID::Ffi,           "ffi",        "Declare external function",
     P2_FFI_IN, 3, P2_NEXT, 1, false, true},

    // call: va_args, 1 output
    {NodeTypeID::Call,          "call",       "Call function",
     P2_CALL_IN, 1, P2_RESULT, 1, false, false},

    // call!: bang + va_args, next + result
    {NodeTypeID::CallBang,      "call!",      "Call function (bang)",
     P2_CALL_BANG_IN, 2, P2_NEXT_RESULT, 2, false, false},

    // erase: 2 inputs, 1 output
    {NodeTypeID::Erase,         "erase",      "Erase from collection",
     P2_ERASE_IN, 2, P2_RESULT, 1, false, false},

    // output_mix!
    {NodeTypeID::OutputMixBang, "output_mix!","Mix into audio output",
     P2_OUTPUT_MIX_IN, 2, nullptr, 0, false, false},

    // append: 2 inputs, 1 output
    {NodeTypeID::Append,        "append",     "Append to collection",
     P2_APPEND_IN, 2, P2_RESULT, 1, false, false},

    // append!: bang + 2 inputs, next + result
    {NodeTypeID::AppendBang,    "append!",    "Append to collection (bang)",
     P2_APPEND_BANG_IN, 3, P2_NEXT_RESULT, 2, false, false},

    // store: 2 inputs, no outputs
    {NodeTypeID::Store,         "store",      "Store value",
     P2_STORE_IN, 2, nullptr, 0, false, false},

    // store!: bang + 2 inputs, next
    {NodeTypeID::StoreBang,     "store!",     "Store value (bang)",
     P2_STORE_BANG_IN, 3, P2_NEXT, 1, false, false},

    // event!: no inputs, next output
    {NodeTypeID::EventBang,     "event!",     "Event source",
     nullptr, 0, P2_EVENT_OUT, 1, true, false},

    // on_key_down!: no inputs, next + 2 data outputs
    {NodeTypeID::OnKeyDownBang, "on_key_down!","Key press event",
     nullptr, 0, P2_KEY_OUT, 3, true, false},

    // on_key_up!: no inputs, next + 2 data outputs
    {NodeTypeID::OnKeyUpBang,   "on_key_up!", "Key release event",
     nullptr, 0, P2_KEY_UP_OUT, 3, true, false},

    // select!: bang + condition, 3 bang outputs
    {NodeTypeID::SelectBang,    "select!",    "Branch on condition",
     P2_SELECT_BANG_IN, 2, P2_SELECT_BANG_OUT, 3, false, false},

    // expr!: bang input, next + outputs (dynamic)
    {NodeTypeID::ExprBang,      "expr!",      "Evaluate expression on bang",
     P2_EXPR_BANG_IN, 1, P2_NEXT, 1, false, false},

    // erase!: bang + 2 inputs, next + result
    {NodeTypeID::EraseBang,     "erase!",     "Erase from collection (bang)",
     P2_ERASE_BANG_IN, 3, P2_NEXT_RESULT, 2, false, false},

    // iterate: collection + fn, no outputs
    {NodeTypeID::Iterate,       "iterate",    "Iterate collection",
     P2_ITERATE_IN, 2, nullptr, 0, false, false},

    // iterate!: bang + collection + fn, next
    {NodeTypeID::IterateBang,   "iterate!",   "Iterate collection (bang)",
     P2_ITERATE_BANG_IN, 3, P2_NEXT, 1, false, false},

    // next: 1 input, 1 output
    {NodeTypeID::Next,          "next",       "Advance iterator",
     P2_VALUE, 1, P2_RESULT, 1, false, false},

    // lock: mutex + fn + va_args, no outputs
    {NodeTypeID::Lock,          "lock",       "Execute under mutex lock",
     P2_LOCK_IN, 3, nullptr, 0, false, false},

    // lock!: bang + mutex + fn + va_args, next
    {NodeTypeID::LockBang,      "lock!",      "Execute under mutex lock (bang)",
     P2_LOCK_BANG_IN, 4, P2_NEXT, 1, false, false},

    // resize!: bang + target + size, next
    {NodeTypeID::ResizeBang,    "resize!",    "Resize vector",
     P2_RESIZE_IN, 3, P2_NEXT, 1, false, false},

    // cast: 1 input, 1 output
    {NodeTypeID::Cast,          "cast",       "Cast value to type",
     P2_VALUE, 1, P2_RESULT, 1, false, false},

    // label: no pins
    {NodeTypeID::Label,         "label",      "Text label",
     nullptr, 0, nullptr, 0, false, false},

    // deref: 1 input, 1 output
    {NodeTypeID::Deref,         "deref",      "Dereference iterator (internal)",
     P2_VALUE, 1, P2_RESULT, 1, false, false},

    // error: no pins
    {NodeTypeID::Error,         "error",      "Error: invalid node",
     nullptr, 0, nullptr, 0, false, false},
};

static constexpr int NUM_NODE_TYPES2 = sizeof(NODE_TYPES2) / sizeof(NODE_TYPES2[0]);

static const NodeType2* find_node_type2(NodeTypeID id) {
    auto idx = static_cast<uint8_t>(id);
    if (idx < NUM_NODE_TYPES2) return &NODE_TYPES2[idx];
    return nullptr;
}

static const NodeType2* find_node_type2(const char* name) {
    for (int i = 0; i < NUM_NODE_TYPES2; i++)
        if (strcmp(NODE_TYPES2[i].name, name) == 0) return &NODE_TYPES2[i];
    return nullptr;
}
