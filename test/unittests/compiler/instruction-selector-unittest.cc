// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/compiler/instruction-selector-unittest.h"

#include "src/compiler/graph.h"
#include "src/compiler/schedule.h"
#include "src/flags.h"
#include "test/unittests/compiler/compiler-test-utils.h"

namespace v8 {
namespace internal {
namespace compiler {

namespace {

typedef RawMachineAssembler::Label MLabel;

}  // namespace


InstructionSelectorTest::InstructionSelectorTest() : rng_(FLAG_random_seed) {}


InstructionSelectorTest::~InstructionSelectorTest() {}


InstructionSelectorTest::Stream InstructionSelectorTest::StreamBuilder::Build(
    InstructionSelector::Features features,
    InstructionSelectorTest::StreamBuilderMode mode,
    InstructionSelector::SourcePositionMode source_position_mode) {
  Schedule* schedule = Export();
  if (FLAG_trace_turbo) {
    OFStream out(stdout);
    out << "=== Schedule before instruction selection ===" << std::endl
        << *schedule;
  }
  size_t const node_count = graph()->NodeCount();
  EXPECT_NE(0u, node_count);
  Linkage linkage(call_descriptor());
  InstructionBlocks* instruction_blocks =
      InstructionSequence::InstructionBlocksFor(test_->zone(), schedule);
  InstructionSequence sequence(test_->isolate(), test_->zone(),
                               instruction_blocks);
  SourcePositionTable source_position_table(graph());
  InstructionSelector selector(test_->zone(), node_count, &linkage, &sequence,
                               schedule, &source_position_table,
                               source_position_mode, features);
  selector.SelectInstructions();
  if (FLAG_trace_turbo) {
    OFStream out(stdout);
    PrintableInstructionSequence printable = {
        RegisterConfiguration::ArchDefault(), &sequence};
    out << "=== Code sequence after instruction selection ===" << std::endl
        << printable;
  }
  Stream s;
  s.virtual_registers_ = selector.GetVirtualRegistersForTesting();
  // Map virtual registers.
  for (Instruction* const instr : sequence) {
    if (instr->opcode() < 0) continue;
    if (mode == kTargetInstructions) {
      switch (instr->arch_opcode()) {
#define CASE(Name) \
  case k##Name:    \
    break;
        TARGET_ARCH_OPCODE_LIST(CASE)
#undef CASE
        default:
          continue;
      }
    }
    if (mode == kAllExceptNopInstructions && instr->arch_opcode() == kArchNop) {
      continue;
    }
    for (size_t i = 0; i < instr->OutputCount(); ++i) {
      InstructionOperand* output = instr->OutputAt(i);
      EXPECT_NE(InstructionOperand::IMMEDIATE, output->kind());
      if (output->IsConstant()) {
        int vreg = ConstantOperand::cast(output)->virtual_register();
        s.constants_.insert(std::make_pair(vreg, sequence.GetConstant(vreg)));
      }
    }
    for (size_t i = 0; i < instr->InputCount(); ++i) {
      InstructionOperand* input = instr->InputAt(i);
      EXPECT_NE(InstructionOperand::CONSTANT, input->kind());
      if (input->IsImmediate()) {
        auto imm = ImmediateOperand::cast(input);
        if (imm->type() == ImmediateOperand::INDEXED) {
          int index = imm->indexed_value();
          s.immediates_.insert(
              std::make_pair(index, sequence.GetImmediate(imm)));
        }
      }
    }
    s.instructions_.push_back(instr);
  }
  for (auto i : s.virtual_registers_) {
    int const virtual_register = i.second;
    if (sequence.IsFloat(virtual_register)) {
      EXPECT_FALSE(sequence.IsReference(virtual_register));
      s.doubles_.insert(virtual_register);
    }
    if (sequence.IsReference(virtual_register)) {
      EXPECT_FALSE(sequence.IsFloat(virtual_register));
      s.references_.insert(virtual_register);
    }
  }
  for (int i = 0; i < sequence.GetFrameStateDescriptorCount(); i++) {
    s.deoptimization_entries_.push_back(sequence.GetFrameStateDescriptor(
        InstructionSequence::StateId::FromInt(i)));
  }
  return s;
}


int InstructionSelectorTest::Stream::ToVreg(const Node* node) const {
  VirtualRegisters::const_iterator i = virtual_registers_.find(node->id());
  CHECK(i != virtual_registers_.end());
  return i->second;
}


bool InstructionSelectorTest::Stream::IsFixed(const InstructionOperand* operand,
                                              Register reg) const {
  if (!operand->IsUnallocated()) return false;
  const UnallocatedOperand* unallocated = UnallocatedOperand::cast(operand);
  if (!unallocated->HasFixedRegisterPolicy()) return false;
  return unallocated->fixed_register_index() == reg.code();
}


bool InstructionSelectorTest::Stream::IsSameAsFirst(
    const InstructionOperand* operand) const {
  if (!operand->IsUnallocated()) return false;
  const UnallocatedOperand* unallocated = UnallocatedOperand::cast(operand);
  return unallocated->HasSameAsInputPolicy();
}


bool InstructionSelectorTest::Stream::IsUsedAtStart(
    const InstructionOperand* operand) const {
  if (!operand->IsUnallocated()) return false;
  const UnallocatedOperand* unallocated = UnallocatedOperand::cast(operand);
  return unallocated->IsUsedAtStart();
}


const FrameStateFunctionInfo*
InstructionSelectorTest::StreamBuilder::GetFrameStateFunctionInfo(
    int parameter_count, int local_count) {
  return common()->CreateFrameStateFunctionInfo(
      FrameStateType::kJavaScriptFunction, parameter_count, local_count,
      Handle<SharedFunctionInfo>(), CALL_MAINTAINS_NATIVE_CONTEXT);
}


// -----------------------------------------------------------------------------
// Return.


TARGET_TEST_F(InstructionSelectorTest, ReturnFloat32Constant) {
  const float kValue = 4.2f;
  StreamBuilder m(this, kMachFloat32);
  m.Return(m.Float32Constant(kValue));
  Stream s = m.Build(kAllInstructions);
  ASSERT_EQ(3U, s.size());
  EXPECT_EQ(kArchNop, s[0]->arch_opcode());
  ASSERT_EQ(InstructionOperand::CONSTANT, s[0]->OutputAt(0)->kind());
  EXPECT_FLOAT_EQ(kValue, s.ToFloat32(s[0]->OutputAt(0)));
  EXPECT_EQ(kArchRet, s[1]->arch_opcode());
  EXPECT_EQ(1U, s[1]->InputCount());
}


TARGET_TEST_F(InstructionSelectorTest, ReturnParameter) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  m.Return(m.Parameter(0));
  Stream s = m.Build(kAllInstructions);
  ASSERT_EQ(3U, s.size());
  EXPECT_EQ(kArchNop, s[0]->arch_opcode());
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_EQ(kArchRet, s[1]->arch_opcode());
  EXPECT_EQ(1U, s[1]->InputCount());
}


TARGET_TEST_F(InstructionSelectorTest, ReturnZero) {
  StreamBuilder m(this, kMachInt32);
  m.Return(m.Int32Constant(0));
  Stream s = m.Build(kAllInstructions);
  ASSERT_EQ(3U, s.size());
  EXPECT_EQ(kArchNop, s[0]->arch_opcode());
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_EQ(InstructionOperand::CONSTANT, s[0]->OutputAt(0)->kind());
  EXPECT_EQ(0, s.ToInt32(s[0]->OutputAt(0)));
  EXPECT_EQ(kArchRet, s[1]->arch_opcode());
  EXPECT_EQ(1U, s[1]->InputCount());
}


// -----------------------------------------------------------------------------
// Conversions.


TARGET_TEST_F(InstructionSelectorTest, TruncateFloat64ToInt32WithParameter) {
  StreamBuilder m(this, kMachInt32, kMachFloat64);
  m.Return(
      m.TruncateFloat64ToInt32(TruncationMode::kJavaScript, m.Parameter(0)));
  Stream s = m.Build(kAllInstructions);
  ASSERT_EQ(4U, s.size());
  EXPECT_EQ(kArchNop, s[0]->arch_opcode());
  EXPECT_EQ(kArchTruncateDoubleToI, s[1]->arch_opcode());
  EXPECT_EQ(1U, s[1]->InputCount());
  EXPECT_EQ(1U, s[1]->OutputCount());
  EXPECT_EQ(kArchRet, s[2]->arch_opcode());
}


// -----------------------------------------------------------------------------
// Parameters.


TARGET_TEST_F(InstructionSelectorTest, DoubleParameter) {
  StreamBuilder m(this, kMachFloat64, kMachFloat64);
  Node* param = m.Parameter(0);
  m.Return(param);
  Stream s = m.Build(kAllInstructions);
  EXPECT_TRUE(s.IsDouble(param));
}


TARGET_TEST_F(InstructionSelectorTest, ReferenceParameter) {
  StreamBuilder m(this, kMachAnyTagged, kMachAnyTagged);
  Node* param = m.Parameter(0);
  m.Return(param);
  Stream s = m.Build(kAllInstructions);
  EXPECT_TRUE(s.IsReference(param));
}


// -----------------------------------------------------------------------------
// FinishRegion.


TARGET_TEST_F(InstructionSelectorTest, FinishRegion) {
  StreamBuilder m(this, kMachAnyTagged, kMachAnyTagged);
  Node* param = m.Parameter(0);
  Node* finish =
      m.AddNode(m.common()->FinishRegion(), param, m.graph()->start());
  m.Return(finish);
  Stream s = m.Build(kAllInstructions);
  ASSERT_EQ(4U, s.size());
  EXPECT_EQ(kArchNop, s[0]->arch_opcode());
  ASSERT_EQ(1U, s[0]->OutputCount());
  ASSERT_TRUE(s[0]->Output()->IsUnallocated());
  EXPECT_EQ(s.ToVreg(param), s.ToVreg(s[0]->Output()));
  EXPECT_EQ(kArchNop, s[1]->arch_opcode());
  ASSERT_EQ(1U, s[1]->InputCount());
  ASSERT_TRUE(s[1]->InputAt(0)->IsUnallocated());
  EXPECT_EQ(s.ToVreg(param), s.ToVreg(s[1]->InputAt(0)));
  ASSERT_EQ(1U, s[1]->OutputCount());
  ASSERT_TRUE(s[1]->Output()->IsUnallocated());
  EXPECT_TRUE(UnallocatedOperand::cast(s[1]->Output())->HasSameAsInputPolicy());
  EXPECT_EQ(s.ToVreg(finish), s.ToVreg(s[1]->Output()));
  EXPECT_TRUE(s.IsReference(finish));
}


// -----------------------------------------------------------------------------
// Phi.


typedef InstructionSelectorTestWithParam<MachineType>
    InstructionSelectorPhiTest;


TARGET_TEST_P(InstructionSelectorPhiTest, Doubleness) {
  const MachineType type = GetParam();
  StreamBuilder m(this, type, type, type);
  Node* param0 = m.Parameter(0);
  Node* param1 = m.Parameter(1);
  MLabel a, b, c;
  m.Branch(m.Int32Constant(0), &a, &b);
  m.Bind(&a);
  m.Goto(&c);
  m.Bind(&b);
  m.Goto(&c);
  m.Bind(&c);
  Node* phi = m.Phi(type, param0, param1);
  m.Return(phi);
  Stream s = m.Build(kAllInstructions);
  EXPECT_EQ(s.IsDouble(phi), s.IsDouble(param0));
  EXPECT_EQ(s.IsDouble(phi), s.IsDouble(param1));
}


TARGET_TEST_P(InstructionSelectorPhiTest, Referenceness) {
  const MachineType type = GetParam();
  StreamBuilder m(this, type, type, type);
  Node* param0 = m.Parameter(0);
  Node* param1 = m.Parameter(1);
  MLabel a, b, c;
  m.Branch(m.Int32Constant(1), &a, &b);
  m.Bind(&a);
  m.Goto(&c);
  m.Bind(&b);
  m.Goto(&c);
  m.Bind(&c);
  Node* phi = m.Phi(type, param0, param1);
  m.Return(phi);
  Stream s = m.Build(kAllInstructions);
  EXPECT_EQ(s.IsReference(phi), s.IsReference(param0));
  EXPECT_EQ(s.IsReference(phi), s.IsReference(param1));
}


INSTANTIATE_TEST_CASE_P(InstructionSelectorTest, InstructionSelectorPhiTest,
                        ::testing::Values(kMachFloat64, kMachInt8, kMachUint8,
                                          kMachInt16, kMachUint16, kMachInt32,
                                          kMachUint32, kMachInt64, kMachUint64,
                                          kMachPtr, kMachAnyTagged));


// -----------------------------------------------------------------------------
// ValueEffect.


TARGET_TEST_F(InstructionSelectorTest, ValueEffect) {
  StreamBuilder m1(this, kMachInt32, kMachPtr);
  Node* p1 = m1.Parameter(0);
  m1.Return(m1.Load(kMachInt32, p1, m1.Int32Constant(0)));
  Stream s1 = m1.Build(kAllInstructions);
  StreamBuilder m2(this, kMachInt32, kMachPtr);
  Node* p2 = m2.Parameter(0);
  m2.Return(
      m2.AddNode(m2.machine()->Load(kMachInt32), p2, m2.Int32Constant(0),
                 m2.AddNode(m2.common()->BeginRegion(), m2.graph()->start())));
  Stream s2 = m2.Build(kAllInstructions);
  EXPECT_LE(3U, s1.size());
  ASSERT_EQ(s1.size(), s2.size());
  TRACED_FORRANGE(size_t, i, 0, s1.size() - 1) {
    const Instruction* i1 = s1[i];
    const Instruction* i2 = s2[i];
    EXPECT_EQ(i1->arch_opcode(), i2->arch_opcode());
    EXPECT_EQ(i1->InputCount(), i2->InputCount());
    EXPECT_EQ(i1->OutputCount(), i2->OutputCount());
  }
}


// -----------------------------------------------------------------------------
// Calls with deoptimization.


TARGET_TEST_F(InstructionSelectorTest, CallJSFunctionWithDeopt) {
  StreamBuilder m(this, kMachAnyTagged, kMachAnyTagged, kMachAnyTagged,
                  kMachAnyTagged);

  BailoutId bailout_id(42);

  Node* function_node = m.Parameter(0);
  Node* receiver = m.Parameter(1);
  Node* context = m.Parameter(2);

  ZoneVector<MachineType> int32_type(1, kMachInt32, zone());
  ZoneVector<MachineType> empty_types(zone());

  CallDescriptor* descriptor = Linkage::GetJSCallDescriptor(
      zone(), false, 1, CallDescriptor::kNeedsFrameState);

  Node* parameters =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(1));
  Node* locals = m.AddNode(m.common()->TypedStateValues(&empty_types));
  Node* stack = m.AddNode(m.common()->TypedStateValues(&empty_types));
  Node* context_dummy = m.Int32Constant(0);

  Node* state_node = m.AddNode(
      m.common()->FrameState(bailout_id, OutputFrameStateCombine::Push(),
                             m.GetFrameStateFunctionInfo(1, 0)),
      parameters, locals, stack, context_dummy, function_node,
      m.UndefinedConstant());
  Node* args[] = {receiver, context};
  Node* call =
      m.CallNWithFrameState(descriptor, function_node, args, state_node);
  m.Return(call);

  Stream s = m.Build(kAllExceptNopInstructions);

  // Skip until kArchCallJSFunction.
  size_t index = 0;
  for (; index < s.size() && s[index]->arch_opcode() != kArchCallJSFunction;
       index++) {
  }
  // Now we should have two instructions: call and return.
  ASSERT_EQ(index + 2, s.size());

  EXPECT_EQ(kArchCallJSFunction, s[index++]->arch_opcode());
  EXPECT_EQ(kArchRet, s[index++]->arch_opcode());

  // TODO(jarin) Check deoptimization table.
}


TARGET_TEST_F(InstructionSelectorTest, CallFunctionStubWithDeopt) {
  StreamBuilder m(this, kMachAnyTagged, kMachAnyTagged, kMachAnyTagged,
                  kMachAnyTagged);

  BailoutId bailout_id_before(42);

  // Some arguments for the call node.
  Node* function_node = m.Parameter(0);
  Node* receiver = m.Parameter(1);
  Node* context = m.Int32Constant(1);  // Context is ignored.

  ZoneVector<MachineType> int32_type(1, kMachInt32, zone());
  ZoneVector<MachineType> float64_type(1, kMachFloat64, zone());
  ZoneVector<MachineType> tagged_type(1, kMachAnyTagged, zone());

  // Build frame state for the state before the call.
  Node* parameters =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(43));
  Node* locals = m.AddNode(m.common()->TypedStateValues(&float64_type),
                           m.Float64Constant(0.5));
  Node* stack = m.AddNode(m.common()->TypedStateValues(&tagged_type),
                          m.UndefinedConstant());

  Node* context_sentinel = m.Int32Constant(0);
  Node* frame_state_before = m.AddNode(
      m.common()->FrameState(bailout_id_before, OutputFrameStateCombine::Push(),
                             m.GetFrameStateFunctionInfo(1, 1)),
      parameters, locals, stack, context_sentinel, function_node,
      m.UndefinedConstant());

  // Build the call.
  Node* call = m.CallFunctionStub0(function_node, receiver, context,
                                   frame_state_before, CALL_AS_METHOD);

  m.Return(call);

  Stream s = m.Build(kAllExceptNopInstructions);

  // Skip until kArchCallJSFunction.
  size_t index = 0;
  for (; index < s.size() && s[index]->arch_opcode() != kArchCallCodeObject;
       index++) {
  }
  // Now we should have two instructions: call, return.
  ASSERT_EQ(index + 2, s.size());

  // Check the call instruction
  const Instruction* call_instr = s[index++];
  EXPECT_EQ(kArchCallCodeObject, call_instr->arch_opcode());
  size_t num_operands =
      1 +  // Code object.
      1 +
      5 +  // Frame state deopt id + one input for each value in frame state.
      1 +  // Function.
      1;   // Context.
  ASSERT_EQ(num_operands, call_instr->InputCount());

  // Code object.
  EXPECT_TRUE(call_instr->InputAt(0)->IsImmediate());

  // Deoptimization id.
  int32_t deopt_id_before = s.ToInt32(call_instr->InputAt(1));
  FrameStateDescriptor* desc_before =
      s.GetFrameStateDescriptor(deopt_id_before);
  EXPECT_EQ(bailout_id_before, desc_before->bailout_id());
  EXPECT_EQ(OutputFrameStateCombine::kPushOutput,
            desc_before->state_combine().kind());
  EXPECT_EQ(1u, desc_before->parameters_count());
  EXPECT_EQ(1u, desc_before->locals_count());
  EXPECT_EQ(1u, desc_before->stack_count());
  EXPECT_EQ(43, s.ToInt32(call_instr->InputAt(3)));
  EXPECT_EQ(0, s.ToInt32(call_instr->InputAt(4)));  // This should be a context.
                                                    // We inserted 0 here.
  EXPECT_EQ(0.5, s.ToFloat64(call_instr->InputAt(5)));
  EXPECT_TRUE(s.ToHeapObject(call_instr->InputAt(6))->IsUndefined());
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(0));  // function is always
                                                       // tagged/any.
  EXPECT_EQ(kMachInt32, desc_before->GetType(1));
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(2));  // context is always
                                                       // tagged/any.
  EXPECT_EQ(kMachFloat64, desc_before->GetType(3));
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(4));

  // Function.
  EXPECT_EQ(s.ToVreg(function_node), s.ToVreg(call_instr->InputAt(7)));
  // Context.
  EXPECT_EQ(s.ToVreg(context), s.ToVreg(call_instr->InputAt(8)));

  EXPECT_EQ(kArchRet, s[index++]->arch_opcode());

  EXPECT_EQ(index, s.size());
}


TARGET_TEST_F(InstructionSelectorTest,
              CallFunctionStubDeoptRecursiveFrameState) {
  StreamBuilder m(this, kMachAnyTagged, kMachAnyTagged, kMachAnyTagged,
                  kMachAnyTagged);

  BailoutId bailout_id_before(42);
  BailoutId bailout_id_parent(62);

  // Some arguments for the call node.
  Node* function_node = m.Parameter(0);
  Node* receiver = m.Parameter(1);
  Node* context = m.Int32Constant(66);

  ZoneVector<MachineType> int32_type(1, kMachInt32, zone());
  ZoneVector<MachineType> int32x2_type(2, kMachInt32, zone());
  ZoneVector<MachineType> float64_type(1, kMachFloat64, zone());

  // Build frame state for the state before the call.
  Node* parameters =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(63));
  Node* locals =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(64));
  Node* stack =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(65));
  Node* frame_state_parent = m.AddNode(
      m.common()->FrameState(bailout_id_parent,
                             OutputFrameStateCombine::Ignore(),
                             m.GetFrameStateFunctionInfo(1, 1)),
      parameters, locals, stack, context, function_node, m.UndefinedConstant());

  Node* context2 = m.Int32Constant(46);
  Node* parameters2 =
      m.AddNode(m.common()->TypedStateValues(&int32_type), m.Int32Constant(43));
  Node* locals2 = m.AddNode(m.common()->TypedStateValues(&float64_type),
                            m.Float64Constant(0.25));
  Node* stack2 = m.AddNode(m.common()->TypedStateValues(&int32x2_type),
                           m.Int32Constant(44), m.Int32Constant(45));
  Node* frame_state_before = m.AddNode(
      m.common()->FrameState(bailout_id_before, OutputFrameStateCombine::Push(),
                             m.GetFrameStateFunctionInfo(1, 1)),
      parameters2, locals2, stack2, context2, function_node,
      frame_state_parent);

  // Build the call.
  Node* call = m.CallFunctionStub0(function_node, receiver, context2,
                                   frame_state_before, CALL_AS_METHOD);

  m.Return(call);

  Stream s = m.Build(kAllExceptNopInstructions);

  // Skip until kArchCallJSFunction.
  size_t index = 0;
  for (; index < s.size() && s[index]->arch_opcode() != kArchCallCodeObject;
       index++) {
  }
  // Now we should have three instructions: call, return.
  EXPECT_EQ(index + 2, s.size());

  // Check the call instruction
  const Instruction* call_instr = s[index++];
  EXPECT_EQ(kArchCallCodeObject, call_instr->arch_opcode());
  size_t num_operands =
      1 +  // Code object.
      1 +  // Frame state deopt id
      6 +  // One input for each value in frame state + context.
      5 +  // One input for each value in the parent frame state + context.
      1 +  // Function.
      1;   // Context.
  EXPECT_EQ(num_operands, call_instr->InputCount());
  // Code object.
  EXPECT_TRUE(call_instr->InputAt(0)->IsImmediate());

  // Deoptimization id.
  int32_t deopt_id_before = s.ToInt32(call_instr->InputAt(1));
  FrameStateDescriptor* desc_before =
      s.GetFrameStateDescriptor(deopt_id_before);
  FrameStateDescriptor* desc_before_outer = desc_before->outer_state();
  EXPECT_EQ(bailout_id_before, desc_before->bailout_id());
  EXPECT_EQ(1u, desc_before_outer->parameters_count());
  EXPECT_EQ(1u, desc_before_outer->locals_count());
  EXPECT_EQ(1u, desc_before_outer->stack_count());
  // Values from parent environment.
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(0));
  EXPECT_EQ(63, s.ToInt32(call_instr->InputAt(3)));
  EXPECT_EQ(kMachInt32, desc_before_outer->GetType(1));
  // Context:
  EXPECT_EQ(66, s.ToInt32(call_instr->InputAt(4)));
  EXPECT_EQ(kMachAnyTagged, desc_before_outer->GetType(2));
  EXPECT_EQ(64, s.ToInt32(call_instr->InputAt(5)));
  EXPECT_EQ(kMachInt32, desc_before_outer->GetType(3));
  EXPECT_EQ(65, s.ToInt32(call_instr->InputAt(6)));
  EXPECT_EQ(kMachInt32, desc_before_outer->GetType(4));
  // Values from the nested frame.
  EXPECT_EQ(1u, desc_before->parameters_count());
  EXPECT_EQ(1u, desc_before->locals_count());
  EXPECT_EQ(2u, desc_before->stack_count());
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(0));
  EXPECT_EQ(43, s.ToInt32(call_instr->InputAt(8)));
  EXPECT_EQ(kMachInt32, desc_before->GetType(1));
  EXPECT_EQ(46, s.ToInt32(call_instr->InputAt(9)));
  EXPECT_EQ(kMachAnyTagged, desc_before->GetType(2));
  EXPECT_EQ(0.25, s.ToFloat64(call_instr->InputAt(10)));
  EXPECT_EQ(kMachFloat64, desc_before->GetType(3));
  EXPECT_EQ(44, s.ToInt32(call_instr->InputAt(11)));
  EXPECT_EQ(kMachInt32, desc_before->GetType(4));
  EXPECT_EQ(45, s.ToInt32(call_instr->InputAt(12)));
  EXPECT_EQ(kMachInt32, desc_before->GetType(5));

  // Function.
  EXPECT_EQ(s.ToVreg(function_node), s.ToVreg(call_instr->InputAt(13)));
  // Context.
  EXPECT_EQ(s.ToVreg(context2), s.ToVreg(call_instr->InputAt(14)));
  // Continuation.

  EXPECT_EQ(kArchRet, s[index++]->arch_opcode());
  EXPECT_EQ(index, s.size());
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
