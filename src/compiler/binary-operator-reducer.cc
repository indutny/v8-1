// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/binary-operator-reducer.h"

#include <algorithm>

#include "src/compiler/common-operator.h"
#include "src/compiler/graph.h"
#include "src/compiler/machine-operator.h"
#include "src/compiler/node.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/node-properties.h"

namespace v8 {
namespace internal {
namespace compiler {

BinaryOperatorReducer::BinaryOperatorReducer(Editor* editor, Graph* graph,
                                             CommonOperatorBuilder* common,
                                             MachineOperatorBuilder* machine)
    : AdvancedReducer(editor),
      graph_(graph),
      common_(common),
      machine_(machine),
      dead_(graph->NewNode(common->Dead())) {}


Reduction BinaryOperatorReducer::Reduce(Node* node) {
  switch (node->opcode()) {
    case IrOpcode::kFloat64Mul:
      return ReduceFloat64Mul(node);
    default:
      break;
  }
  return NoChange();
}


Reduction BinaryOperatorReducer::ReduceFloat64Mul(Node* node) {
  if (node->InputAt(0)->opcode() != IrOpcode::kChangeInt32ToFloat64) {
    return NoChange();
  }
  if (node->InputAt(1)->opcode() != IrOpcode::kChangeInt32ToFloat64) {
    return NoChange();
  }

  Type::RangeType* range = NodeProperties::GetType(node)->GetRange();

  // JavaScript has 52 bit precision in multiplication
  if (range == nullptr || range->Min() < 0.0 ||
      range->Max() > 0xFFFFFFFFFFFFFULL) {
    return NoChange();
  }

  // Verify that the uses cast result to Int32
  for (Node* use : node->uses()) {
    // XXX: How to handle this properly?
    if (use->opcode() == IrOpcode::kStateValues) continue;

    if (use->opcode() == IrOpcode::kTruncateFloat64ToInt32) continue;
    if (use->opcode() != IrOpcode::kFloat64Div) return NoChange();

    // Verify division
    //
    // The RHS value should be positive integer
    Float64BinopMatcher m(use);
    if (!m.right().HasValue() || m.right().Value() <= 0) return NoChange();

    int64_t value = static_cast<int64_t>(m.right().Value());
    if (value != static_cast<int64_t>(m.right().Value())) return NoChange();
    if (!base::bits::IsPowerOfTwo64(value)) return NoChange();

    // The result should fit into 32bit word
    if ((static_cast<int64_t>(range->Max()) / value) > 0xFFFFFFFULL) {
      return NoChange();
    }

    // Check that uses of division are cast to Int32
    for (Node* subuse : use->uses()) {
      // XXX: How to handle this properly?
      if (subuse->opcode() == IrOpcode::kStateValues) continue;

      if (subuse->opcode() != IrOpcode::kTruncateFloat64ToInt32) {
        return NoChange();
      }
    }
  }

  // The mul+div can be optimized
  for (Node* use : node->uses()) {
    // XXX: How to handle this properly?
    if (use->opcode() == IrOpcode::kStateValues) continue;

    if (use->opcode() == IrOpcode::kTruncateFloat64ToInt32) {
      use->set_op(machine()->TruncateInt64ToInt32());
      Revisit(use);
      continue;
    }

    Float64BinopMatcher m(use);
    int64_t shift = WhichPowerOf2_64(static_cast<int64_t>(m.right().Value()));

    use->set_op(machine()->Word64Shr());
    use->ReplaceInput(1, graph()->NewNode(common()->Int64Constant(shift)));
    Revisit(use);

    for (Node* subuse : use->uses()) {
      // XXX: How to handle this properly?
      if (subuse->opcode() == IrOpcode::kStateValues) continue;

      subuse->set_op(machine()->TruncateInt64ToInt32());
      Revisit(subuse);
    }
  }

  return Change(node, machine()->Int64Mul(), node->InputAt(0)->InputAt(0),
                node->InputAt(1)->InputAt(0));
}


Reduction BinaryOperatorReducer::Change(Node* node, Operator const* op,
                                        Node* a) {
  node->set_op(op);
  node->ReplaceInput(0, a);
  node->TrimInputCount(1);
  return Changed(node);
}


Reduction BinaryOperatorReducer::Change(Node* node, Operator const* op, Node* a,
                                        Node* b) {
  node->set_op(op);
  node->ReplaceInput(0, a);
  node->ReplaceInput(1, b);
  node->TrimInputCount(2);
  return Changed(node);
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
