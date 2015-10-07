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
    case IrOpcode::kTruncateFloat64ToInt32:
      return ReduceTruncateFloat64ToInt32(node);
    default:
      break;
  }
  return NoChange();
}


Reduction BinaryOperatorReducer::ReduceFloat52Mul(Node* node) {
  if (!machine()->Is64() || node->opcode() != IrOpcode::kFloat64Mul) {
    return NoChange();
  }

  // TODO(indutny): Observe ranges and cast things to int
  if (node->InputAt(0)->opcode() != IrOpcode::kChangeInt32ToFloat64 ||
      node->InputAt(1)->opcode() != IrOpcode::kChangeInt32ToFloat64) {
    return NoChange();
  }

  Type::RangeType* range = NodeProperties::GetType(node)->GetRange();

  // JavaScript has 52 bit precision in multiplication
  if (range == nullptr || range->Min() < 0.0 ||
      range->Max() > 0xFFFFFFFFFFFFFULL) {
    return NoChange();
  }

  Node* out = graph()->NewNode(machine()->Int64Mul(),
      node->InputAt(0)->InputAt(0), node->InputAt(1)->InputAt(0));
  Revisit(out);
  return Replace(out);
}


Reduction BinaryOperatorReducer::ReduceFloat52Div(Node* node) {
  if (!machine()->Is64() || node->opcode() != IrOpcode::kFloat64Div) {
    return NoChange();
  }

  Float64BinopMatcher m(node);

  // Right value should be positive...
  if (!m.right().HasValue() || m.right().Value() <= 0) return NoChange();

  // ...integer...
  int64_t value = static_cast<int64_t>(m.right().Value());
  if (value != static_cast<int64_t>(m.right().Value())) return NoChange();

  // ...and should be a power of two.
  if (!base::bits::IsPowerOfTwo64(value)) return NoChange();

  Reduction mul = ReduceFloat52Mul(node->InputAt(0));
  if (!mul.Changed()) return NoChange();

  Type::RangeType* range = NodeProperties::GetType(node->InputAt(0))
      ->GetRange();

  // The result should fit into 32bit word
  if ((static_cast<int64_t>(range->Max()) / value) > 0xFFFFFFFULL) {
    return NoChange();
  }

  int64_t shift = WhichPowerOf2_64(static_cast<int64_t>(m.right().Value()));

  // Replace division with 64bit right shift
  Node* out = graph()->NewNode(machine()->Word64Shr(),
      mul.replacement(), graph()->NewNode(common()->Int64Constant(shift)));
  Revisit(out);
  return Replace(out);
}


Reduction BinaryOperatorReducer::ReduceTruncateFloat64ToInt32(Node* node) {
  Reduction subst = ReduceFloat52Mul(node->InputAt(0));

  // truncate(26bit x 26bit)
  if (!subst.Changed() && machine()->Is64()) {
    subst = ReduceFloat52Div(node->InputAt(0));
  }

  if (!subst.Changed()) return NoChange();

  return Change(node, machine()->TruncateInt64ToInt32(), subst.replacement());
}


Reduction BinaryOperatorReducer::Change(Node* node, Operator const* op,
                                        Node* a) {
  node->set_op(op);
  node->ReplaceInput(0, a);
  node->TrimInputCount(1);
  return Changed(node);
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
