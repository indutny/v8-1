// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <functional>
#include <limits>

#include "src/base/bits.h"
#include "src/base/utils/random-number-generator.h"
#include "src/codegen.h"
#include "test/cctest/cctest.h"
#include "test/cctest/compiler/codegen-tester.h"
#include "test/cctest/compiler/value-helper.h"

using namespace v8::base;
using namespace v8::internal;
using namespace v8::internal::compiler;

typedef RawMachineAssembler::Label MLabel;

TEST(RunInt32Add) {
  RawMachineAssemblerTester<int32_t> m;
  Node* add = m.Int32Add(m.Int32Constant(0), m.Int32Constant(1));
  m.Return(add);
  CHECK_EQ(1, m.Call());
}


void TestWord32Ctz(int32_t value, int32_t expected) {
  RawMachineAssemblerTester<int32_t> m;
  if (m.machine()->Word32Ctz().IsSupported()) {
    Node* ctz =
        m.AddNode(m.machine()->Word32Ctz().op(), m.Int32Constant(value));
    m.Return(ctz);
    CHECK_EQ(expected, m.Call());
  }
}


TEST(RunInt32Ctz) {
  TestWord32Ctz(0x00000000, 32);
  TestWord32Ctz(0x80000000, 31);
  TestWord32Ctz(0x40000000, 30);
  TestWord32Ctz(0x20000000, 29);
  TestWord32Ctz(0x10000000, 28);
  TestWord32Ctz(0xa8000000, 27);
  TestWord32Ctz(0xf4000000, 26);
  TestWord32Ctz(0x62000000, 25);
  TestWord32Ctz(0x91000000, 24);
  TestWord32Ctz(0xcd800000, 23);
  TestWord32Ctz(0x09400000, 22);
  TestWord32Ctz(0xaf200000, 21);
  TestWord32Ctz(0xac100000, 20);
  TestWord32Ctz(0xe0b80000, 19);
  TestWord32Ctz(0x9ce40000, 18);
  TestWord32Ctz(0xc7920000, 17);
  TestWord32Ctz(0xb8f10000, 16);
  TestWord32Ctz(0x3b9f8000, 15);
  TestWord32Ctz(0xdb4c4000, 14);
  TestWord32Ctz(0xe9a32000, 13);
  TestWord32Ctz(0xfca61000, 12);
  TestWord32Ctz(0x6c8a7800, 11);
  TestWord32Ctz(0x8ce5a400, 10);
  TestWord32Ctz(0xcb7d0200, 9);
  TestWord32Ctz(0xcb4dc100, 8);
  TestWord32Ctz(0xdfbec580, 7);
  TestWord32Ctz(0x27a9db40, 6);
  TestWord32Ctz(0xde3bcb20, 5);
  TestWord32Ctz(0xd7e8a610, 4);
  TestWord32Ctz(0x9afdbc88, 3);
  TestWord32Ctz(0x9afdbc84, 2);
  TestWord32Ctz(0x9afdbc82, 1);
  TestWord32Ctz(0x9afdbc81, 0);
}


void TestWord32Clz(int32_t value, int32_t expected) {
  RawMachineAssemblerTester<int32_t> m;
  Node* clz = m.Word32Clz(m.Int32Constant(value));
  m.Return(clz);
  CHECK_EQ(expected, m.Call());
}


TEST(RunInt32Clz) {
  TestWord32Clz(0x80001000, 0);
  TestWord32Clz(0x40000500, 1);
  TestWord32Clz(0x20000300, 2);
  TestWord32Clz(0x10000003, 3);
  TestWord32Clz(0x08050000, 4);
  TestWord32Clz(0x04006000, 5);
  TestWord32Clz(0x02000000, 6);
  TestWord32Clz(0x010000a0, 7);
  TestWord32Clz(0x00800c00, 8);
  TestWord32Clz(0x00400000, 9);
  TestWord32Clz(0x0020000d, 10);
  TestWord32Clz(0x00100f00, 11);
  TestWord32Clz(0x00080000, 12);
  TestWord32Clz(0x00041000, 13);
  TestWord32Clz(0x00020020, 14);
  TestWord32Clz(0x00010300, 15);
  TestWord32Clz(0x00008040, 16);
  TestWord32Clz(0x00004005, 17);
  TestWord32Clz(0x00002050, 18);
  TestWord32Clz(0x00001700, 19);
  TestWord32Clz(0x00000870, 20);
  TestWord32Clz(0x00000405, 21);
  TestWord32Clz(0x00000203, 22);
  TestWord32Clz(0x00000101, 23);
  TestWord32Clz(0x00000089, 24);
  TestWord32Clz(0x00000041, 25);
  TestWord32Clz(0x00000022, 26);
  TestWord32Clz(0x00000013, 27);
  TestWord32Clz(0x00000008, 28);
  TestWord32Clz(0x00000004, 29);
  TestWord32Clz(0x00000002, 30);
  TestWord32Clz(0x00000001, 31);
  TestWord32Clz(0x00000000, 32);
}


void TestWord32Popcnt(int32_t value, int32_t expected) {
  RawMachineAssemblerTester<int32_t> m;
  compiler::OptionalOperator op = m.machine()->Word32Popcnt();
  if (op.IsSupported()) {
    Node* popcnt = m.AddNode(op.op(), m.Int32Constant(value));
    m.Return(popcnt);
    CHECK_EQ(expected, m.Call());
  }
}


TEST(RunWord32Popcnt) {
  TestWord32Popcnt(0x00000000, 0);
  TestWord32Popcnt(0x00000001, 1);
  TestWord32Popcnt(0x80000000, 1);
  TestWord32Popcnt(0xffffffff, 32);
  TestWord32Popcnt(0x000dc100, 6);
  TestWord32Popcnt(0xe00dc100, 9);
  TestWord32Popcnt(0xe00dc103, 11);
  TestWord32Popcnt(0x000dc107, 9);
}


static Node* Int32Input(RawMachineAssemblerTester<int32_t>* m, int index) {
  switch (index) {
    case 0:
      return m->Parameter(0);
    case 1:
      return m->Parameter(1);
    case 2:
      return m->Int32Constant(0);
    case 3:
      return m->Int32Constant(1);
    case 4:
      return m->Int32Constant(-1);
    case 5:
      return m->Int32Constant(0xff);
    case 6:
      return m->Int32Constant(0x01234567);
    case 7:
      return m->Load(kMachInt32, m->PointerConstant(NULL));
    default:
      return NULL;
  }
}


TEST(CodeGenInt32Binop) {
  RawMachineAssemblerTester<void> m;

  const Operator* kOps[] = {
      m.machine()->Word32And(),      m.machine()->Word32Or(),
      m.machine()->Word32Xor(),      m.machine()->Word32Shl(),
      m.machine()->Word32Shr(),      m.machine()->Word32Sar(),
      m.machine()->Word32Equal(),    m.machine()->Int32Add(),
      m.machine()->Int32Sub(),       m.machine()->Int32Mul(),
      m.machine()->Int32MulHigh(),   m.machine()->Int32Div(),
      m.machine()->Uint32Div(),      m.machine()->Int32Mod(),
      m.machine()->Uint32Mod(),      m.machine()->Uint32MulHigh(),
      m.machine()->Int32LessThan(),  m.machine()->Int32LessThanOrEqual(),
      m.machine()->Uint32LessThan(), m.machine()->Uint32LessThanOrEqual()};

  for (size_t i = 0; i < arraysize(kOps); ++i) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
        Node* a = Int32Input(&m, j);
        Node* b = Int32Input(&m, k);
        m.Return(m.AddNode(kOps[i], a, b));
        m.GenerateCode();
      }
    }
  }
}


TEST(CodeGenNop) {
  RawMachineAssemblerTester<void> m;
  m.Return(m.Int32Constant(0));
  m.GenerateCode();
}


#if V8_TARGET_ARCH_64_BIT
static Node* Int64Input(RawMachineAssemblerTester<int64_t>* m, int index) {
  switch (index) {
    case 0:
      return m->Parameter(0);
    case 1:
      return m->Parameter(1);
    case 2:
      return m->Int64Constant(0);
    case 3:
      return m->Int64Constant(1);
    case 4:
      return m->Int64Constant(-1);
    case 5:
      return m->Int64Constant(0xff);
    case 6:
      return m->Int64Constant(0x0123456789abcdefLL);
    case 7:
      return m->Load(kMachInt64, m->PointerConstant(NULL));
    default:
      return NULL;
  }
}


TEST(CodeGenInt64Binop) {
  RawMachineAssemblerTester<void> m;

  const Operator* kOps[] = {
      m.machine()->Word64And(), m.machine()->Word64Or(),
      m.machine()->Word64Xor(), m.machine()->Word64Shl(),
      m.machine()->Word64Shr(), m.machine()->Word64Sar(),
      m.machine()->Word64Equal(), m.machine()->Int64Add(),
      m.machine()->Int64Sub(), m.machine()->Int64Mul(), m.machine()->Int64Div(),
      m.machine()->Uint64Div(), m.machine()->Int64Mod(),
      m.machine()->Uint64Mod(), m.machine()->Int64LessThan(),
      m.machine()->Int64LessThanOrEqual(), m.machine()->Uint64LessThan(),
      m.machine()->Uint64LessThanOrEqual()};

  for (size_t i = 0; i < arraysize(kOps); ++i) {
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 8; k++) {
        RawMachineAssemblerTester<int64_t> m(kMachInt64, kMachInt64);
        Node* a = Int64Input(&m, j);
        Node* b = Int64Input(&m, k);
        m.Return(m.AddNode(kOps[i], a, b));
        m.GenerateCode();
      }
    }
  }
}


// TODO(titzer): add tests that run 64-bit integer operations.
#endif  // V8_TARGET_ARCH_64_BIT


TEST(RunGoto) {
  RawMachineAssemblerTester<int32_t> m;
  int constant = 99999;

  MLabel next;
  m.Goto(&next);
  m.Bind(&next);
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


TEST(RunGotoMultiple) {
  RawMachineAssemblerTester<int32_t> m;
  int constant = 9999977;

  MLabel labels[10];
  for (size_t i = 0; i < arraysize(labels); i++) {
    m.Goto(&labels[i]);
    m.Bind(&labels[i]);
  }
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


TEST(RunBranch) {
  RawMachineAssemblerTester<int32_t> m;
  int constant = 999777;

  MLabel blocka, blockb;
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Return(m.Int32Constant(0 - constant));
  m.Bind(&blockb);
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


TEST(RunDiamond2) {
  RawMachineAssemblerTester<int32_t> m;

  int constant = 995666;

  MLabel blocka, blockb, end;
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);
  m.Bind(&end);
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


TEST(RunLoop) {
  RawMachineAssemblerTester<int32_t> m;
  int constant = 999555;

  MLabel header, body, exit;
  m.Goto(&header);
  m.Bind(&header);
  m.Branch(m.Int32Constant(0), &body, &exit);
  m.Bind(&body);
  m.Goto(&header);
  m.Bind(&exit);
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


template <typename R>
static void BuildDiamondPhi(RawMachineAssemblerTester<R>* m, Node* cond_node,
                            MachineType type, Node* true_node,
                            Node* false_node) {
  MLabel blocka, blockb, end;
  m->Branch(cond_node, &blocka, &blockb);
  m->Bind(&blocka);
  m->Goto(&end);
  m->Bind(&blockb);
  m->Goto(&end);

  m->Bind(&end);
  Node* phi = m->Phi(type, true_node, false_node);
  m->Return(phi);
}


TEST(RunDiamondPhiConst) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  int false_val = 0xFF666;
  int true_val = 0x00DDD;
  Node* true_node = m.Int32Constant(true_val);
  Node* false_node = m.Int32Constant(false_val);
  BuildDiamondPhi(&m, m.Parameter(0), kMachInt32, true_node, false_node);
  CHECK_EQ(false_val, m.Call(0));
  CHECK_EQ(true_val, m.Call(1));
}


TEST(RunDiamondPhiNumber) {
  RawMachineAssemblerTester<Object*> m(kMachInt32);
  double false_val = -11.1;
  double true_val = 200.1;
  Node* true_node = m.NumberConstant(true_val);
  Node* false_node = m.NumberConstant(false_val);
  BuildDiamondPhi(&m, m.Parameter(0), kMachAnyTagged, true_node, false_node);
  m.CheckNumber(false_val, m.Call(0));
  m.CheckNumber(true_val, m.Call(1));
}


TEST(RunDiamondPhiString) {
  RawMachineAssemblerTester<Object*> m(kMachInt32);
  const char* false_val = "false";
  const char* true_val = "true";
  Node* true_node = m.StringConstant(true_val);
  Node* false_node = m.StringConstant(false_val);
  BuildDiamondPhi(&m, m.Parameter(0), kMachAnyTagged, true_node, false_node);
  m.CheckString(false_val, m.Call(0));
  m.CheckString(true_val, m.Call(1));
}


TEST(RunDiamondPhiParam) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
  BuildDiamondPhi(&m, m.Parameter(0), kMachInt32, m.Parameter(1),
                  m.Parameter(2));
  int32_t c1 = 0x260cb75a;
  int32_t c2 = 0xcd3e9c8b;
  int result = m.Call(0, c1, c2);
  CHECK_EQ(c2, result);
  result = m.Call(1, c1, c2);
  CHECK_EQ(c1, result);
}


TEST(RunLoopPhiConst) {
  RawMachineAssemblerTester<int32_t> m;
  int true_val = 0x44000;
  int false_val = 0x00888;

  Node* cond_node = m.Int32Constant(0);
  Node* true_node = m.Int32Constant(true_val);
  Node* false_node = m.Int32Constant(false_val);

  // x = false_val; while(false) { x = true_val; } return x;
  MLabel body, header, end;

  m.Goto(&header);
  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, false_node, true_node);
  m.Branch(cond_node, &body, &end);
  m.Bind(&body);
  m.Goto(&header);
  m.Bind(&end);
  m.Return(phi);

  CHECK_EQ(false_val, m.Call());
}


TEST(RunLoopPhiParam) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);

  MLabel blocka, blockb, end;

  m.Goto(&blocka);

  m.Bind(&blocka);
  Node* phi = m.Phi(kMachInt32, m.Parameter(1), m.Parameter(2));
  Node* cond = m.Phi(kMachInt32, m.Parameter(0), m.Int32Constant(0));
  m.Branch(cond, &blockb, &end);

  m.Bind(&blockb);
  m.Goto(&blocka);

  m.Bind(&end);
  m.Return(phi);

  int32_t c1 = 0xa81903b4;
  int32_t c2 = 0x5a1207da;
  int result = m.Call(0, c1, c2);
  CHECK_EQ(c1, result);
  result = m.Call(1, c1, c2);
  CHECK_EQ(c2, result);
}


TEST(RunLoopPhiInduction) {
  RawMachineAssemblerTester<int32_t> m;

  int false_val = 0x10777;

  // x = false_val; while(false) { x++; } return x;
  MLabel header, body, end;
  Node* false_node = m.Int32Constant(false_val);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, false_node, false_node);
  m.Branch(m.Int32Constant(0), &body, &end);

  m.Bind(&body);
  Node* add = m.Int32Add(phi, m.Int32Constant(1));
  phi->ReplaceInput(1, add);
  m.Goto(&header);

  m.Bind(&end);
  m.Return(phi);

  CHECK_EQ(false_val, m.Call());
}


TEST(RunLoopIncrement) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);

  // x = 0; while(x ^ param) { x++; } return x;
  MLabel header, body, end;
  Node* zero = m.Int32Constant(0);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, zero, zero);
  m.Branch(m.WordXor(phi, bt.param0), &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Int32Add(phi, m.Int32Constant(1)));
  m.Goto(&header);

  m.Bind(&end);
  bt.AddReturn(phi);

  CHECK_EQ(11, bt.call(11, 0));
  CHECK_EQ(110, bt.call(110, 0));
  CHECK_EQ(176, bt.call(176, 0));
}


TEST(RunLoopIncrement2) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);

  // x = 0; while(x < param) { x++; } return x;
  MLabel header, body, end;
  Node* zero = m.Int32Constant(0);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, zero, zero);
  m.Branch(m.Int32LessThan(phi, bt.param0), &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Int32Add(phi, m.Int32Constant(1)));
  m.Goto(&header);

  m.Bind(&end);
  bt.AddReturn(phi);

  CHECK_EQ(11, bt.call(11, 0));
  CHECK_EQ(110, bt.call(110, 0));
  CHECK_EQ(176, bt.call(176, 0));
  CHECK_EQ(0, bt.call(-200, 0));
}


TEST(RunLoopIncrement3) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);

  // x = 0; while(x < param) { x++; } return x;
  MLabel header, body, end;
  Node* zero = m.Int32Constant(0);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, zero, zero);
  m.Branch(m.Uint32LessThan(phi, bt.param0), &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Int32Add(phi, m.Int32Constant(1)));
  m.Goto(&header);

  m.Bind(&end);
  bt.AddReturn(phi);

  CHECK_EQ(11, bt.call(11, 0));
  CHECK_EQ(110, bt.call(110, 0));
  CHECK_EQ(176, bt.call(176, 0));
  CHECK_EQ(200, bt.call(200, 0));
}


TEST(RunLoopDecrement) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);

  // x = param; while(x) { x--; } return x;
  MLabel header, body, end;

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, bt.param0, m.Int32Constant(0));
  m.Branch(phi, &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Int32Sub(phi, m.Int32Constant(1)));
  m.Goto(&header);

  m.Bind(&end);
  bt.AddReturn(phi);

  CHECK_EQ(0, bt.call(11, 0));
  CHECK_EQ(0, bt.call(110, 0));
  CHECK_EQ(0, bt.call(197, 0));
}


TEST(RunLoopIncrementFloat32) {
  RawMachineAssemblerTester<int32_t> m;

  // x = -3.0f; while(x < 10f) { x = x + 0.5f; } return (int) (double) x;
  MLabel header, body, end;
  Node* minus_3 = m.Float32Constant(-3.0f);
  Node* ten = m.Float32Constant(10.0f);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachFloat32, minus_3, ten);
  m.Branch(m.Float32LessThan(phi, ten), &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Float32Add(phi, m.Float32Constant(0.5f)));
  m.Goto(&header);

  m.Bind(&end);
  m.Return(m.ChangeFloat64ToInt32(m.ChangeFloat32ToFloat64(phi)));

  CHECK_EQ(10, m.Call());
}


TEST(RunLoopIncrementFloat64) {
  RawMachineAssemblerTester<int32_t> m;

  // x = -3.0; while(x < 10) { x = x + 0.5; } return (int) x;
  MLabel header, body, end;
  Node* minus_3 = m.Float64Constant(-3.0);
  Node* ten = m.Float64Constant(10.0);

  m.Goto(&header);

  m.Bind(&header);
  Node* phi = m.Phi(kMachFloat64, minus_3, ten);
  m.Branch(m.Float64LessThan(phi, ten), &body, &end);

  m.Bind(&body);
  phi->ReplaceInput(1, m.Float64Add(phi, m.Float64Constant(0.5)));
  m.Goto(&header);

  m.Bind(&end);
  m.Return(m.ChangeFloat64ToInt32(phi));

  CHECK_EQ(10, m.Call());
}


TEST(RunSwitch1) {
  RawMachineAssemblerTester<int32_t> m;

  int constant = 11223344;

  MLabel block0, block1, def, end;
  MLabel* case_labels[] = {&block0, &block1};
  int32_t case_values[] = {0, 1};
  m.Switch(m.Int32Constant(0), &def, case_values, case_labels,
           arraysize(case_labels));
  m.Bind(&block0);
  m.Goto(&end);
  m.Bind(&block1);
  m.Goto(&end);
  m.Bind(&def);
  m.Goto(&end);
  m.Bind(&end);
  m.Return(m.Int32Constant(constant));

  CHECK_EQ(constant, m.Call());
}


TEST(RunSwitch2) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);

  MLabel blocka, blockb, blockc;
  MLabel* case_labels[] = {&blocka, &blockb};
  int32_t case_values[] = {std::numeric_limits<int32_t>::min(),
                           std::numeric_limits<int32_t>::max()};
  m.Switch(m.Parameter(0), &blockc, case_values, case_labels,
           arraysize(case_labels));
  m.Bind(&blocka);
  m.Return(m.Int32Constant(-1));
  m.Bind(&blockb);
  m.Return(m.Int32Constant(1));
  m.Bind(&blockc);
  m.Return(m.Int32Constant(0));

  CHECK_EQ(1, m.Call(std::numeric_limits<int32_t>::max()));
  CHECK_EQ(-1, m.Call(std::numeric_limits<int32_t>::min()));
  for (int i = -100; i < 100; i += 25) {
    CHECK_EQ(0, m.Call(i));
  }
}


TEST(RunSwitch3) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);

  MLabel blocka, blockb, blockc;
  MLabel* case_labels[] = {&blocka, &blockb};
  int32_t case_values[] = {std::numeric_limits<int32_t>::min() + 0,
                           std::numeric_limits<int32_t>::min() + 1};
  m.Switch(m.Parameter(0), &blockc, case_values, case_labels,
           arraysize(case_labels));
  m.Bind(&blocka);
  m.Return(m.Int32Constant(0));
  m.Bind(&blockb);
  m.Return(m.Int32Constant(1));
  m.Bind(&blockc);
  m.Return(m.Int32Constant(2));

  CHECK_EQ(0, m.Call(std::numeric_limits<int32_t>::min() + 0));
  CHECK_EQ(1, m.Call(std::numeric_limits<int32_t>::min() + 1));
  for (int i = -100; i < 100; i += 25) {
    CHECK_EQ(2, m.Call(i));
  }
}


TEST(RunSwitch4) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);

  const size_t kNumCases = 512;
  const size_t kNumValues = kNumCases + 1;
  int32_t values[kNumValues];
  m.main_isolate()->random_number_generator()->NextBytes(values,
                                                         sizeof(values));
  MLabel end, def;
  int32_t case_values[kNumCases];
  MLabel* case_labels[kNumCases];
  Node* results[kNumValues];
  for (size_t i = 0; i < kNumCases; ++i) {
    case_values[i] = static_cast<int32_t>(i);
    case_labels[i] = new (m.main_zone()->New(sizeof(MLabel))) MLabel;
  }
  m.Switch(m.Parameter(0), &def, case_values, case_labels,
           arraysize(case_labels));
  for (size_t i = 0; i < kNumCases; ++i) {
    m.Bind(case_labels[i]);
    results[i] = m.Int32Constant(values[i]);
    m.Goto(&end);
  }
  m.Bind(&def);
  results[kNumCases] = m.Int32Constant(values[kNumCases]);
  m.Goto(&end);
  m.Bind(&end);
  const int num_results = static_cast<int>(arraysize(results));
  Node* phi =
      m.AddNode(m.common()->Phi(kMachInt32, num_results), num_results, results);
  m.Return(phi);

  for (size_t i = 0; i < kNumValues; ++i) {
    CHECK_EQ(values[i], m.Call(static_cast<int>(i)));
  }
}


TEST(RunLoadInt32) {
  RawMachineAssemblerTester<int32_t> m;

  int32_t p1 = 0;  // loads directly from this location.
  m.Return(m.LoadFromPointer(&p1, kMachInt32));

  FOR_INT32_INPUTS(i) {
    p1 = *i;
    CHECK_EQ(p1, m.Call());
  }
}


TEST(RunLoadInt32Offset) {
  int32_t p1 = 0;  // loads directly from this location.

  int32_t offsets[] = {-2000000, -100, -101, 1,          3,
                       7,        120,  2000, 2000000000, 0xff};

  for (size_t i = 0; i < arraysize(offsets); i++) {
    RawMachineAssemblerTester<int32_t> m;
    int32_t offset = offsets[i];
    byte* pointer = reinterpret_cast<byte*>(&p1) - offset;
    // generate load [#base + #index]
    m.Return(m.LoadFromPointer(pointer, kMachInt32, offset));

    FOR_INT32_INPUTS(j) {
      p1 = *j;
      CHECK_EQ(p1, m.Call());
    }
  }
}


TEST(RunLoadStoreFloat32Offset) {
  float p1 = 0.0f;  // loads directly from this location.
  float p2 = 0.0f;  // and stores directly into this location.

  FOR_INT32_INPUTS(i) {
    int32_t magic = 0x2342aabb + *i * 3;
    RawMachineAssemblerTester<int32_t> m;
    int32_t offset = *i;
    byte* from = reinterpret_cast<byte*>(&p1) - offset;
    byte* to = reinterpret_cast<byte*>(&p2) - offset;
    // generate load [#base + #index]
    Node* load =
        m.Load(kMachFloat32, m.PointerConstant(from), m.IntPtrConstant(offset));
    m.Store(kMachFloat32, m.PointerConstant(to), m.IntPtrConstant(offset),
            load);
    m.Return(m.Int32Constant(magic));

    FOR_FLOAT32_INPUTS(j) {
      p1 = *j;
      p2 = *j - 5;
      CHECK_EQ(magic, m.Call());
      CheckDoubleEq(p1, p2);
    }
  }
}


TEST(RunLoadStoreFloat64Offset) {
  double p1 = 0;  // loads directly from this location.
  double p2 = 0;  // and stores directly into this location.

  FOR_INT32_INPUTS(i) {
    int32_t magic = 0x2342aabb + *i * 3;
    RawMachineAssemblerTester<int32_t> m;
    int32_t offset = *i;
    byte* from = reinterpret_cast<byte*>(&p1) - offset;
    byte* to = reinterpret_cast<byte*>(&p2) - offset;
    // generate load [#base + #index]
    Node* load =
        m.Load(kMachFloat64, m.PointerConstant(from), m.IntPtrConstant(offset));
    m.Store(kMachFloat64, m.PointerConstant(to), m.IntPtrConstant(offset),
            load);
    m.Return(m.Int32Constant(magic));

    FOR_FLOAT64_INPUTS(j) {
      p1 = *j;
      p2 = *j - 5;
      CHECK_EQ(magic, m.Call());
      CheckDoubleEq(p1, p2);
    }
  }
}


TEST(RunInt32AddP) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);

  bt.AddReturn(m.Int32Add(bt.param0, bt.param1));

  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      // Use uint32_t because signed overflow is UB in C.
      int expected = static_cast<int32_t>(*i + *j);
      CHECK_EQ(expected, bt.call(*i, *j));
    }
  }
}


TEST(RunInt32AddAndWord32EqualP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(m.Int32Add(m.Parameter(0),
                        m.Word32Equal(m.Parameter(1), m.Parameter(2))));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>(bit_cast<uint32_t>(*i) + (*j == *k));
          CHECK_EQ(expected, m.Call(*i, *j, *k));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(m.Int32Add(m.Word32Equal(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>((*i == *j) + bit_cast<uint32_t>(*k));
          CHECK_EQ(expected, m.Call(*i, *j, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32EqualImm) {
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
      m.Return(m.Int32Add(m.Int32Constant(*i),
                          m.Word32Equal(m.Parameter(0), m.Parameter(1))));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>(bit_cast<uint32_t>(*i) + (*j == *k));
          CHECK_EQ(expected, m.Call(*j, *k));
        }
      }
    }
  }
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
      m.Return(m.Int32Add(m.Word32Equal(m.Int32Constant(*i), m.Parameter(0)),
                          m.Parameter(1)));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>((*i == *j) + bit_cast<uint32_t>(*k));
          CHECK_EQ(expected, m.Call(*j, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32NotEqualP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(m.Int32Add(m.Parameter(0),
                        m.Word32NotEqual(m.Parameter(1), m.Parameter(2))));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>(bit_cast<uint32_t>(*i) + (*j != *k));
          CHECK_EQ(expected, m.Call(*i, *j, *k));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(m.Int32Add(m.Word32NotEqual(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>((*i != *j) + bit_cast<uint32_t>(*k));
          CHECK_EQ(expected, m.Call(*i, *j, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32NotEqualImm) {
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
      m.Return(m.Int32Add(m.Int32Constant(*i),
                          m.Word32NotEqual(m.Parameter(0), m.Parameter(1))));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>(bit_cast<uint32_t>(*i) + (*j != *k));
          CHECK_EQ(expected, m.Call(*j, *k));
        }
      }
    }
  }
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
      m.Return(m.Int32Add(m.Word32NotEqual(m.Int32Constant(*i), m.Parameter(0)),
                          m.Parameter(1)));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t const expected =
              bit_cast<int32_t>((*i != *j) + bit_cast<uint32_t>(*k));
          CHECK_EQ(expected, m.Call(*j, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32SarP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32, kMachUint32);
    m.Return(m.Int32Add(m.Parameter(0),
                        m.Word32Sar(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = *i + (*j >> shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachUint32, kMachUint32);
    m.Return(m.Int32Add(m.Word32Sar(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = (*i >> shift) + *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32ShlP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32, kMachUint32);
    m.Return(m.Int32Add(m.Parameter(0),
                        m.Word32Shl(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = *i + (*j << shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachUint32, kMachUint32);
    m.Return(m.Int32Add(m.Word32Shl(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = (*i << shift) + *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddAndWord32ShrP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Int32Add(m.Parameter(0),
                        m.Word32Shr(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = *i + (*j >> shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Int32Add(m.Word32Shr(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = (*i >> shift) + *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32AddInBranch) {
  static const int32_t constant = 987654321;
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32Equal(m.Int32Add(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i + *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32NotEqual(m.Int32Add(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i + *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Int32Add(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i + *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32NotEqual(m.Int32Add(m.Int32Constant(*i), m.Parameter(0)),
                                m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i + *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Int32Add(m.Parameter(0),
                                        m.AddNode(shops[n], m.Parameter(1),
                                                  m.Parameter(2))),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = ((*i + right) == 0) ? constant : 0 - constant;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunInt32AddInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Add(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i + *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Int32Add(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i + *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Int32Add(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i + *j) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Int32Add(m.Parameter(0), m.Int32Constant(*i)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*j + *i) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      m.Return(m.Word32Equal(
          m.Int32Add(m.Parameter(0),
                     m.AddNode(shops[n], m.Parameter(1), m.Parameter(2))),
          m.Int32Constant(0)));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = (*i + right) == 0;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunInt32SubP) {
  RawMachineAssemblerTester<int32_t> m;
  Uint32BinopTester bt(&m);

  m.Return(m.Int32Sub(bt.param0, bt.param1));

  FOR_UINT32_INPUTS(i) {
    FOR_UINT32_INPUTS(j) {
      uint32_t expected = static_cast<int32_t>(*i - *j);
      CHECK_EQ(expected, bt.call(*i, *j));
    }
  }
}


TEST(RunInt32SubImm) {
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Int32Sub(m.Int32Constant(*i), m.Parameter(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i - *j;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Int32Sub(m.Parameter(0), m.Int32Constant(*i)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *j - *i;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunInt32SubAndWord32SarP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32, kMachUint32);
    m.Return(m.Int32Sub(m.Parameter(0),
                        m.Word32Sar(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          int32_t expected = *i - (*j >> shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachUint32, kMachUint32);
    m.Return(m.Int32Sub(m.Word32Sar(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          int32_t expected = (*i >> shift) - *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32SubAndWord32ShlP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32, kMachUint32);
    m.Return(m.Int32Sub(m.Parameter(0),
                        m.Word32Shl(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          int32_t expected = *i - (*j << shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachUint32, kMachUint32);
    m.Return(m.Int32Sub(m.Word32Shl(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          int32_t expected = (*i << shift) - *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32SubAndWord32ShrP) {
  {
    RawMachineAssemblerTester<uint32_t> m(kMachUint32, kMachUint32,
                                          kMachUint32);
    m.Return(m.Int32Sub(m.Parameter(0),
                        m.Word32Shr(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          // Use uint32_t because signed overflow is UB in C.
          uint32_t expected = *i - (*j >> shift);
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<uint32_t> m(kMachUint32, kMachUint32,
                                          kMachUint32);
    m.Return(m.Int32Sub(m.Word32Shr(m.Parameter(0), m.Parameter(1)),
                        m.Parameter(2)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          // Use uint32_t because signed overflow is UB in C.
          uint32_t expected = (*i >> shift) - *k;
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunInt32SubInBranch) {
  static const int constant = 987654321;
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32Equal(m.Int32Sub(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i - *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32NotEqual(m.Int32Sub(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i - *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Int32Sub(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i - *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32NotEqual(m.Int32Sub(m.Int32Constant(*i), m.Parameter(0)),
                                m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i - *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Int32Sub(m.Parameter(0),
                                        m.AddNode(shops[n], m.Parameter(1),
                                                  m.Parameter(2))),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = ((*i - right) == 0) ? constant : 0 - constant;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunInt32SubInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Sub(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i - *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Int32Sub(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i - *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Int32Sub(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i - *j) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Int32Sub(m.Parameter(0), m.Int32Constant(*i)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*j - *i) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      m.Return(m.Word32Equal(
          m.Int32Sub(m.Parameter(0),
                     m.AddNode(shops[n], m.Parameter(1), m.Parameter(2))),
          m.Int32Constant(0)));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = (*i - right) == 0;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunInt32MulP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Mul(bt.param0, bt.param1));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int expected = static_cast<int32_t>(*i * *j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Int32Mul(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i * *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunInt32MulHighP) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  bt.AddReturn(m.Int32MulHigh(bt.param0, bt.param1));
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected = static_cast<int32_t>(
          (static_cast<int64_t>(*i) * static_cast<int64_t>(*j)) >> 32);
      CHECK_EQ(expected, bt.call(*i, *j));
    }
  }
}


TEST(RunInt32MulImm) {
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Int32Mul(m.Int32Constant(*i), m.Parameter(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i * *j;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Int32Mul(m.Parameter(0), m.Int32Constant(*i)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *j * *i;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunInt32MulAndInt32AddP) {
  {
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        RawMachineAssemblerTester<int32_t> m(kMachInt32);
        int32_t p0 = *i;
        int32_t p1 = *j;
        m.Return(m.Int32Add(m.Int32Constant(p0),
                            m.Int32Mul(m.Parameter(0), m.Int32Constant(p1))));
        FOR_INT32_INPUTS(k) {
          int32_t p2 = *k;
          int expected = p0 + static_cast<int32_t>(p1 * p2);
          CHECK_EQ(expected, m.Call(p2));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(
        m.Int32Add(m.Parameter(0), m.Int32Mul(m.Parameter(1), m.Parameter(2))));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          int32_t p0 = *i;
          int32_t p1 = *j;
          int32_t p2 = *k;
          int expected = p0 + static_cast<int32_t>(p1 * p2);
          CHECK_EQ(expected, m.Call(p0, p1, p2));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachInt32);
    m.Return(
        m.Int32Add(m.Int32Mul(m.Parameter(0), m.Parameter(1)), m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          int32_t p0 = *i;
          int32_t p1 = *j;
          int32_t p2 = *k;
          int expected = static_cast<int32_t>(p0 * p1) + p2;
          CHECK_EQ(expected, m.Call(p0, p1, p2));
        }
      }
    }
  }
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m;
      Int32BinopTester bt(&m);
      bt.AddReturn(
          m.Int32Add(m.Int32Constant(*i), m.Int32Mul(bt.param0, bt.param1)));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          int32_t p0 = *j;
          int32_t p1 = *k;
          int expected = *i + static_cast<int32_t>(p0 * p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunInt32MulAndInt32SubP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32, kMachInt32);
    m.Return(
        m.Int32Sub(m.Parameter(0), m.Int32Mul(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          uint32_t p0 = *i;
          int32_t p1 = *j;
          int32_t p2 = *k;
          // Use uint32_t because signed overflow is UB in C.
          int expected = p0 - static_cast<uint32_t>(p1 * p2);
          CHECK_EQ(expected, m.Call(p0, p1, p2));
        }
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m;
      Int32BinopTester bt(&m);
      bt.AddReturn(
          m.Int32Sub(m.Int32Constant(*i), m.Int32Mul(bt.param0, bt.param1)));
      FOR_INT32_INPUTS(j) {
        FOR_INT32_INPUTS(k) {
          int32_t p0 = *j;
          int32_t p1 = *k;
          // Use uint32_t because signed overflow is UB in C.
          int expected = *i - static_cast<uint32_t>(p0 * p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunUint32MulHighP) {
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  bt.AddReturn(m.Uint32MulHigh(bt.param0, bt.param1));
  FOR_UINT32_INPUTS(i) {
    FOR_UINT32_INPUTS(j) {
      int32_t expected = bit_cast<int32_t>(static_cast<uint32_t>(
          (static_cast<uint64_t>(*i) * static_cast<uint64_t>(*j)) >> 32));
      CHECK_EQ(expected, bt.call(bit_cast<int32_t>(*i), bit_cast<int32_t>(*j)));
    }
  }
}


TEST(RunInt32DivP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Div(bt.param0, bt.param1));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int p0 = *i;
        int p1 = *j;
        if (p1 != 0 && (static_cast<uint32_t>(p0) != 0x80000000 || p1 != -1)) {
          int expected = static_cast<int32_t>(p0 / p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Add(bt.param0, m.Int32Div(bt.param0, bt.param1)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int p0 = *i;
        int p1 = *j;
        if (p1 != 0 && (static_cast<uint32_t>(p0) != 0x80000000 || p1 != -1)) {
          int expected = static_cast<int32_t>(p0 + (p0 / p1));
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunUint32DivP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Uint32Div(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t p0 = *i;
        uint32_t p1 = *j;
        if (p1 != 0) {
          int32_t expected = bit_cast<int32_t>(p0 / p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Add(bt.param0, m.Uint32Div(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t p0 = *i;
        uint32_t p1 = *j;
        if (p1 != 0) {
          int32_t expected = bit_cast<int32_t>(p0 + (p0 / p1));
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunInt32ModP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Mod(bt.param0, bt.param1));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int p0 = *i;
        int p1 = *j;
        if (p1 != 0 && (static_cast<uint32_t>(p0) != 0x80000000 || p1 != -1)) {
          int expected = static_cast<int32_t>(p0 % p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Int32Add(bt.param0, m.Int32Mod(bt.param0, bt.param1)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int p0 = *i;
        int p1 = *j;
        if (p1 != 0 && (static_cast<uint32_t>(p0) != 0x80000000 || p1 != -1)) {
          int expected = static_cast<int32_t>(p0 + (p0 % p1));
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunUint32ModP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Uint32Mod(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t p0 = *i;
        uint32_t p1 = *j;
        if (p1 != 0) {
          uint32_t expected = static_cast<uint32_t>(p0 % p1);
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Int32Add(bt.param0, m.Uint32Mod(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t p0 = *i;
        uint32_t p1 = *j;
        if (p1 != 0) {
          uint32_t expected = static_cast<uint32_t>(p0 + (p0 % p1));
          CHECK_EQ(expected, bt.call(p0, p1));
        }
      }
    }
  }
}


TEST(RunWord32AndP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32And(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = *i & *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32And(bt.param0, m.Word32Not(bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = *i & ~(*j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32And(m.Word32Not(bt.param0), bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = ~(*i) & *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunWord32AndAndWord32ShlP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Shl(bt.param0, m.Word32And(bt.param1, m.Int32Constant(0x1f))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i << (*j & 0x1f);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Shl(bt.param0, m.Word32And(m.Int32Constant(0x1f), bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i << (0x1f & *j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunWord32AndAndWord32ShrP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Shr(bt.param0, m.Word32And(bt.param1, m.Int32Constant(0x1f))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i >> (*j & 0x1f);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Shr(bt.param0, m.Word32And(m.Int32Constant(0x1f), bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i >> (0x1f & *j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunWord32AndAndWord32SarP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Sar(bt.param0, m.Word32And(bt.param1, m.Int32Constant(0x1f))));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = *i >> (*j & 0x1f);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Sar(bt.param0, m.Word32And(m.Int32Constant(0x1f), bt.param1)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = *i >> (0x1f & *j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunWord32AndImm) {
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32And(m.Int32Constant(*i), m.Parameter(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i & *j;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32And(m.Int32Constant(*i), m.Word32Not(m.Parameter(0))));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i & ~(*j);
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunWord32AndInBranch) {
  static const int constant = 987654321;
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32Equal(m.Word32And(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i & *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32NotEqual(m.Word32And(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i & *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32And(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i & *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(
          m.Word32NotEqual(m.Word32And(m.Int32Constant(*i), m.Parameter(0)),
                           m.Int32Constant(0)),
          &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i & *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32And(m.Parameter(0),
                                         m.AddNode(shops[n], m.Parameter(1),
                                                   m.Parameter(2))),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = ((*i & right) == 0) ? constant : 0 - constant;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunWord32AndInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32And(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i & *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32And(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i & *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Word32And(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i & *j) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Word32And(m.Parameter(0), m.Int32Constant(*i)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*j & *i) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunWord32OrP) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Or(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i | *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Or(bt.param0, m.Word32Not(bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i | ~(*j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Or(m.Word32Not(bt.param0), bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = ~(*i) | *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
}


TEST(RunWord32OrImm) {
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Or(m.Int32Constant(*i), m.Parameter(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i | *j;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Or(m.Int32Constant(*i), m.Word32Not(m.Parameter(0))));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i | ~(*j);
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunWord32OrInBranch) {
  static const int constant = 987654321;
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32Equal(m.Word32Or(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = (*i | *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32NotEqual(m.Word32Or(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = (*i | *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32Or(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_INT32_INPUTS(j) {
        int32_t expected = (*i | *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_INT32_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      MLabel blocka, blockb;
      m.Branch(m.Word32NotEqual(m.Word32Or(m.Int32Constant(*i), m.Parameter(0)),
                                m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_INT32_INPUTS(j) {
        int32_t expected = (*i | *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32Or(m.Parameter(0),
                                        m.AddNode(shops[n], m.Parameter(1),
                                                  m.Parameter(2))),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = ((*i | right) == 0) ? constant : 0 - constant;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunWord32OrInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32Or(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i | *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32Or(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        int32_t expected = (*i | *j) == 0;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Word32Or(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i | *j) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Equal(m.Word32Or(m.Parameter(0), m.Int32Constant(*i)),
                             m.Int32Constant(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*j | *i) == 0;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunWord32XorP) {
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Xor(m.Int32Constant(*i), m.Parameter(0)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i ^ *j;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Xor(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i ^ *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32Xor(bt.param0, m.Word32Not(bt.param1)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = *i ^ ~(*j);
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32Xor(m.Word32Not(bt.param0), bt.param1));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        int32_t expected = ~(*i) ^ *j;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Xor(m.Int32Constant(*i), m.Word32Not(m.Parameter(0))));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *i ^ ~(*j);
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
}


TEST(RunWord32XorInBranch) {
  static const uint32_t constant = 987654321;
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32Equal(m.Word32Xor(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i ^ *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    MLabel blocka, blockb;
    m.Branch(
        m.Word32NotEqual(m.Word32Xor(bt.param0, bt.param1), m.Int32Constant(0)),
        &blocka, &blockb);
    m.Bind(&blocka);
    bt.AddReturn(m.Int32Constant(constant));
    m.Bind(&blockb);
    bt.AddReturn(m.Int32Constant(0 - constant));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i ^ *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, bt.call(*i, *j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32Xor(m.Int32Constant(*i), m.Parameter(0)),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i ^ *j) == 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    FOR_UINT32_INPUTS(i) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      MLabel blocka, blockb;
      m.Branch(
          m.Word32NotEqual(m.Word32Xor(m.Int32Constant(*i), m.Parameter(0)),
                           m.Int32Constant(0)),
          &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = (*i ^ *j) != 0 ? constant : 0 - constant;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<void> m;
    const Operator* shops[] = {m.machine()->Word32Sar(),
                               m.machine()->Word32Shl(),
                               m.machine()->Word32Shr()};
    for (size_t n = 0; n < arraysize(shops); n++) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachInt32,
                                           kMachUint32);
      MLabel blocka, blockb;
      m.Branch(m.Word32Equal(m.Word32Xor(m.Parameter(0),
                                         m.AddNode(shops[n], m.Parameter(1),
                                                   m.Parameter(2))),
                             m.Int32Constant(0)),
               &blocka, &blockb);
      m.Bind(&blocka);
      m.Return(m.Int32Constant(constant));
      m.Bind(&blockb);
      m.Return(m.Int32Constant(0 - constant));
      FOR_UINT32_INPUTS(i) {
        FOR_INT32_INPUTS(j) {
          FOR_UINT32_SHIFTS(shift) {
            int32_t right;
            switch (shops[n]->opcode()) {
              default:
                UNREACHABLE();
              case IrOpcode::kWord32Sar:
                right = *j >> shift;
                break;
              case IrOpcode::kWord32Shl:
                right = *j << shift;
                break;
              case IrOpcode::kWord32Shr:
                right = static_cast<uint32_t>(*j) >> shift;
                break;
            }
            int32_t expected = ((*i ^ right) == 0) ? constant : 0 - constant;
            CHECK_EQ(expected, m.Call(*i, *j, shift));
          }
        }
      }
    }
  }
}


TEST(RunWord32ShlP) {
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Shl(m.Parameter(0), m.Int32Constant(shift)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *j << shift;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Shl(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = *i << shift;
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
}


TEST(RunWord32ShlInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32Shl(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == (*i << shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32Shl(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == (*i << shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Int32Constant(0),
                        m.Word32Shl(m.Parameter(0), m.Int32Constant(shift))));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == (*i << shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Word32Shl(m.Parameter(0), m.Int32Constant(shift)),
                        m.Int32Constant(0)));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == (*i << shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
}


TEST(RunWord32ShrP) {
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(m.Word32Shr(m.Parameter(0), m.Int32Constant(shift)));
      FOR_UINT32_INPUTS(j) {
        uint32_t expected = *j >> shift;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Shr(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = *i >> shift;
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
    CHECK_EQ(0x00010000u, bt.call(0x80000000, 15));
  }
}


TEST(RunWord32ShrInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32Shr(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32Shr(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Int32Constant(0),
                        m.Word32Shr(m.Parameter(0), m.Int32Constant(shift))));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Word32Shr(m.Parameter(0), m.Int32Constant(shift)),
                        m.Int32Constant(0)));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
}


TEST(RunWord32SarP) {
  {
    FOR_INT32_SHIFTS(shift) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      m.Return(m.Word32Sar(m.Parameter(0), m.Int32Constant(shift)));
      FOR_INT32_INPUTS(j) {
        int32_t expected = *j >> shift;
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(m.Word32Sar(bt.param0, bt.param1));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_SHIFTS(shift) {
        int32_t expected = *i >> shift;
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
    CHECK_EQ(bit_cast<int32_t>(0xFFFF0000), bt.call(0x80000000, 15));
  }
}


TEST(RunWord32SarInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32Sar(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_SHIFTS(shift) {
        int32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Int32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32Sar(bt.param0, bt.param1)));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_SHIFTS(shift) {
        int32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    FOR_INT32_SHIFTS(shift) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      m.Return(
          m.Word32Equal(m.Int32Constant(0),
                        m.Word32Sar(m.Parameter(0), m.Int32Constant(shift))));
      FOR_INT32_INPUTS(i) {
        int32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
  {
    FOR_INT32_SHIFTS(shift) {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      m.Return(
          m.Word32Equal(m.Word32Sar(m.Parameter(0), m.Int32Constant(shift)),
                        m.Int32Constant(0)));
      FOR_INT32_INPUTS(i) {
        int32_t expected = 0 == (*i >> shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
}


TEST(RunWord32RorP) {
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<int32_t> m(kMachUint32);
      m.Return(m.Word32Ror(m.Parameter(0), m.Int32Constant(shift)));
      FOR_UINT32_INPUTS(j) {
        int32_t expected = bits::RotateRight32(*j, shift);
        CHECK_EQ(expected, m.Call(*j));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(m.Word32Ror(bt.param0, bt.param1));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = bits::RotateRight32(*i, shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
}


TEST(RunWord32RorInComparison) {
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Word32Ror(bt.param0, bt.param1), m.Int32Constant(0)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == bits::RotateRight32(*i, shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Uint32BinopTester bt(&m);
    bt.AddReturn(
        m.Word32Equal(m.Int32Constant(0), m.Word32Ror(bt.param0, bt.param1)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        uint32_t expected = 0 == bits::RotateRight32(*i, shift);
        CHECK_EQ(expected, bt.call(*i, shift));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Int32Constant(0),
                        m.Word32Ror(m.Parameter(0), m.Int32Constant(shift))));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == bits::RotateRight32(*i, shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
  {
    FOR_UINT32_SHIFTS(shift) {
      RawMachineAssemblerTester<uint32_t> m(kMachUint32);
      m.Return(
          m.Word32Equal(m.Word32Ror(m.Parameter(0), m.Int32Constant(shift)),
                        m.Int32Constant(0)));
      FOR_UINT32_INPUTS(i) {
        uint32_t expected = 0 == bits::RotateRight32(*i, shift);
        CHECK_EQ(expected, m.Call(*i));
      }
    }
  }
}


TEST(RunWord32NotP) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  m.Return(m.Word32Not(m.Parameter(0)));
  FOR_INT32_INPUTS(i) {
    int expected = ~(*i);
    CHECK_EQ(expected, m.Call(*i));
  }
}


TEST(RunInt32NegP) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  m.Return(m.Int32Neg(m.Parameter(0)));
  FOR_INT32_INPUTS(i) {
    int expected = -*i;
    CHECK_EQ(expected, m.Call(*i));
  }
}


TEST(RunWord32EqualAndWord32SarP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32, kMachUint32);
    m.Return(m.Word32Equal(m.Parameter(0),
                           m.Word32Sar(m.Parameter(1), m.Parameter(2))));
    FOR_INT32_INPUTS(i) {
      FOR_INT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          int32_t expected = (*i == (*j >> shift));
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachUint32, kMachInt32);
    m.Return(m.Word32Equal(m.Word32Sar(m.Parameter(0), m.Parameter(1)),
                           m.Parameter(2)));
    FOR_INT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_INT32_INPUTS(k) {
          int32_t expected = ((*i >> shift) == *k);
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunWord32EqualAndWord32ShlP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Word32Equal(m.Parameter(0),
                           m.Word32Shl(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          int32_t expected = (*i == (*j << shift));
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Word32Equal(m.Word32Shl(m.Parameter(0), m.Parameter(1)),
                           m.Parameter(2)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          int32_t expected = ((*i << shift) == *k);
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunWord32EqualAndWord32ShrP) {
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Word32Equal(m.Parameter(0),
                           m.Word32Shr(m.Parameter(1), m.Parameter(2))));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_INPUTS(j) {
        FOR_UINT32_SHIFTS(shift) {
          int32_t expected = (*i == (*j >> shift));
          CHECK_EQ(expected, m.Call(*i, *j, shift));
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m(kMachUint32, kMachUint32, kMachUint32);
    m.Return(m.Word32Equal(m.Word32Shr(m.Parameter(0), m.Parameter(1)),
                           m.Parameter(2)));
    FOR_UINT32_INPUTS(i) {
      FOR_UINT32_SHIFTS(shift) {
        FOR_UINT32_INPUTS(k) {
          int32_t expected = ((*i >> shift) == *k);
          CHECK_EQ(expected, m.Call(*i, shift, *k));
        }
      }
    }
  }
}


TEST(RunDeadNodes) {
  for (int i = 0; true; i++) {
    RawMachineAssemblerTester<int32_t> m(i == 5 ? kMachInt32 : kMachNone);
    int constant = 0x55 + i;
    switch (i) {
      case 0:
        m.Int32Constant(44);
        break;
      case 1:
        m.StringConstant("unused");
        break;
      case 2:
        m.NumberConstant(11.1);
        break;
      case 3:
        m.PointerConstant(&constant);
        break;
      case 4:
        m.LoadFromPointer(&constant, kMachInt32);
        break;
      case 5:
        m.Parameter(0);
        break;
      default:
        return;
    }
    m.Return(m.Int32Constant(constant));
    if (i != 5) {
      CHECK_EQ(constant, m.Call());
    } else {
      CHECK_EQ(constant, m.Call(0));
    }
  }
}


TEST(RunDeadInt32Binops) {
  RawMachineAssemblerTester<int32_t> m;

  const Operator* kOps[] = {
      m.machine()->Word32And(),            m.machine()->Word32Or(),
      m.machine()->Word32Xor(),            m.machine()->Word32Shl(),
      m.machine()->Word32Shr(),            m.machine()->Word32Sar(),
      m.machine()->Word32Ror(),            m.machine()->Word32Equal(),
      m.machine()->Int32Add(),             m.machine()->Int32Sub(),
      m.machine()->Int32Mul(),             m.machine()->Int32MulHigh(),
      m.machine()->Int32Div(),             m.machine()->Uint32Div(),
      m.machine()->Int32Mod(),             m.machine()->Uint32Mod(),
      m.machine()->Uint32MulHigh(),        m.machine()->Int32LessThan(),
      m.machine()->Int32LessThanOrEqual(), m.machine()->Uint32LessThan(),
      m.machine()->Uint32LessThanOrEqual()};

  for (size_t i = 0; i < arraysize(kOps); ++i) {
    RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
    int32_t constant = static_cast<int32_t>(0x55555 + i);
    m.AddNode(kOps[i], m.Parameter(0), m.Parameter(1));
    m.Return(m.Int32Constant(constant));

    CHECK_EQ(constant, m.Call(1, 1));
  }
}


template <typename Type>
static void RunLoadImmIndex(MachineType rep) {
  const int kNumElems = 3;
  Type buffer[kNumElems];

  // initialize the buffer with raw data.
  byte* raw = reinterpret_cast<byte*>(buffer);
  for (size_t i = 0; i < sizeof(buffer); i++) {
    raw[i] = static_cast<byte>((i + sizeof(buffer)) ^ 0xAA);
  }

  // Test with various large and small offsets.
  for (int offset = -1; offset <= 200000; offset *= -5) {
    for (int i = 0; i < kNumElems; i++) {
      RawMachineAssemblerTester<Type> m;
      Node* base = m.PointerConstant(buffer - offset);
      Node* index = m.Int32Constant((offset + i) * sizeof(buffer[0]));
      m.Return(m.Load(rep, base, index));

      Type expected = buffer[i];
      Type actual = m.Call();
      CHECK(expected == actual);
    }
  }
}


TEST(RunLoadImmIndex) {
  RunLoadImmIndex<int8_t>(kMachInt8);
  RunLoadImmIndex<uint8_t>(kMachUint8);
  RunLoadImmIndex<int16_t>(kMachInt16);
  RunLoadImmIndex<uint16_t>(kMachUint16);
  RunLoadImmIndex<int32_t>(kMachInt32);
  RunLoadImmIndex<uint32_t>(kMachUint32);
  RunLoadImmIndex<int32_t*>(kMachAnyTagged);

  // TODO(titzer): test kRepBit loads
  // TODO(titzer): test kMachFloat64 loads
  // TODO(titzer): test various indexing modes.
}


template <typename CType>
static void RunLoadStore(MachineType rep) {
  const int kNumElems = 4;
  CType buffer[kNumElems];

  for (int32_t x = 0; x < kNumElems; x++) {
    int32_t y = kNumElems - x - 1;
    // initialize the buffer with raw data.
    byte* raw = reinterpret_cast<byte*>(buffer);
    for (size_t i = 0; i < sizeof(buffer); i++) {
      raw[i] = static_cast<byte>((i + sizeof(buffer)) ^ 0xAA);
    }

    RawMachineAssemblerTester<int32_t> m;
    int32_t OK = 0x29000 + x;
    Node* base = m.PointerConstant(buffer);
    Node* index0 = m.IntPtrConstant(x * sizeof(buffer[0]));
    Node* load = m.Load(rep, base, index0);
    Node* index1 = m.IntPtrConstant(y * sizeof(buffer[0]));
    m.Store(rep, base, index1, load);
    m.Return(m.Int32Constant(OK));

    CHECK(buffer[x] != buffer[y]);
    CHECK_EQ(OK, m.Call());
    CHECK(buffer[x] == buffer[y]);
  }
}


TEST(RunLoadStore) {
  RunLoadStore<int8_t>(kMachInt8);
  RunLoadStore<uint8_t>(kMachUint8);
  RunLoadStore<int16_t>(kMachInt16);
  RunLoadStore<uint16_t>(kMachUint16);
  RunLoadStore<int32_t>(kMachInt32);
  RunLoadStore<uint32_t>(kMachUint32);
  RunLoadStore<void*>(kMachAnyTagged);
  RunLoadStore<float>(kMachFloat32);
  RunLoadStore<double>(kMachFloat64);
}


TEST(RunFloat32Binop) {
  RawMachineAssemblerTester<int32_t> m;
  float result;

  const Operator* ops[] = {m.machine()->Float32Add(), m.machine()->Float32Sub(),
                           m.machine()->Float32Mul(), m.machine()->Float32Div(),
                           NULL};

  float inf = std::numeric_limits<float>::infinity();
  const Operator* inputs[] = {
      m.common()->Float32Constant(0.0f),   m.common()->Float32Constant(1.0f),
      m.common()->Float32Constant(1.0f),   m.common()->Float32Constant(0.0f),
      m.common()->Float32Constant(0.0f),   m.common()->Float32Constant(-1.0f),
      m.common()->Float32Constant(-1.0f),  m.common()->Float32Constant(0.0f),
      m.common()->Float32Constant(0.22f),  m.common()->Float32Constant(-1.22f),
      m.common()->Float32Constant(-1.22f), m.common()->Float32Constant(0.22f),
      m.common()->Float32Constant(inf),    m.common()->Float32Constant(0.22f),
      m.common()->Float32Constant(inf),    m.common()->Float32Constant(-inf),
      NULL};

  for (int i = 0; ops[i] != NULL; i++) {
    for (int j = 0; inputs[j] != NULL; j += 2) {
      RawMachineAssemblerTester<int32_t> m;
      Node* a = m.AddNode(inputs[j]);
      Node* b = m.AddNode(inputs[j + 1]);
      Node* binop = m.AddNode(ops[i], a, b);
      Node* base = m.PointerConstant(&result);
      Node* zero = m.IntPtrConstant(0);
      m.Store(kMachFloat32, base, zero, binop);
      m.Return(m.Int32Constant(i + j));
      CHECK_EQ(i + j, m.Call());
    }
  }
}


TEST(RunFloat64Binop) {
  RawMachineAssemblerTester<int32_t> m;
  double result;

  const Operator* ops[] = {m.machine()->Float64Add(), m.machine()->Float64Sub(),
                           m.machine()->Float64Mul(), m.machine()->Float64Div(),
                           m.machine()->Float64Mod(), NULL};

  double inf = V8_INFINITY;
  const Operator* inputs[] = {
      m.common()->Float64Constant(0),     m.common()->Float64Constant(1),
      m.common()->Float64Constant(1),     m.common()->Float64Constant(0),
      m.common()->Float64Constant(0),     m.common()->Float64Constant(-1),
      m.common()->Float64Constant(-1),    m.common()->Float64Constant(0),
      m.common()->Float64Constant(0.22),  m.common()->Float64Constant(-1.22),
      m.common()->Float64Constant(-1.22), m.common()->Float64Constant(0.22),
      m.common()->Float64Constant(inf),   m.common()->Float64Constant(0.22),
      m.common()->Float64Constant(inf),   m.common()->Float64Constant(-inf),
      NULL};

  for (int i = 0; ops[i] != NULL; i++) {
    for (int j = 0; inputs[j] != NULL; j += 2) {
      RawMachineAssemblerTester<int32_t> m;
      Node* a = m.AddNode(inputs[j]);
      Node* b = m.AddNode(inputs[j + 1]);
      Node* binop = m.AddNode(ops[i], a, b);
      Node* base = m.PointerConstant(&result);
      Node* zero = m.Int32Constant(0);
      m.Store(kMachFloat64, base, zero, binop);
      m.Return(m.Int32Constant(i + j));
      CHECK_EQ(i + j, m.Call());
    }
  }
}


TEST(RunDeadFloat32Binops) {
  RawMachineAssemblerTester<int32_t> m;

  const Operator* ops[] = {m.machine()->Float32Add(), m.machine()->Float32Sub(),
                           m.machine()->Float32Mul(), m.machine()->Float32Div(),
                           NULL};

  for (int i = 0; ops[i] != NULL; i++) {
    RawMachineAssemblerTester<int32_t> m;
    int constant = 0x53355 + i;
    m.AddNode(ops[i], m.Float32Constant(0.1f), m.Float32Constant(1.11f));
    m.Return(m.Int32Constant(constant));
    CHECK_EQ(constant, m.Call());
  }
}


TEST(RunDeadFloat64Binops) {
  RawMachineAssemblerTester<int32_t> m;

  const Operator* ops[] = {m.machine()->Float64Add(), m.machine()->Float64Sub(),
                           m.machine()->Float64Mul(), m.machine()->Float64Div(),
                           m.machine()->Float64Mod(), NULL};

  for (int i = 0; ops[i] != NULL; i++) {
    RawMachineAssemblerTester<int32_t> m;
    int constant = 0x53355 + i;
    m.AddNode(ops[i], m.Float64Constant(0.1), m.Float64Constant(1.11));
    m.Return(m.Int32Constant(constant));
    CHECK_EQ(constant, m.Call());
  }
}


TEST(RunFloat32AddP) {
  RawMachineAssemblerTester<int32_t> m;
  Float32BinopTester bt(&m);

  bt.AddReturn(m.Float32Add(bt.param0, bt.param1));

  FOR_FLOAT32_INPUTS(pl) {
    FOR_FLOAT32_INPUTS(pr) {
      float expected = *pl + *pr;
      CheckFloatEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64AddP) {
  RawMachineAssemblerTester<int32_t> m;
  Float64BinopTester bt(&m);

  bt.AddReturn(m.Float64Add(bt.param0, bt.param1));

  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      double expected = *pl + *pr;
      CheckDoubleEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat32SubP) {
  RawMachineAssemblerTester<int32_t> m;
  Float32BinopTester bt(&m);

  bt.AddReturn(m.Float32Sub(bt.param0, bt.param1));

  FOR_FLOAT32_INPUTS(pl) {
    FOR_FLOAT32_INPUTS(pr) {
      float expected = *pl - *pr;
      CheckFloatEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat32SubImm1) {
  float input = 0.0f;
  float output = 0.0f;

  FOR_FLOAT32_INPUTS(i) {
    RawMachineAssemblerTester<int32_t> m;
    Node* t0 = m.LoadFromPointer(&input, kMachFloat32);
    Node* t1 = m.Float32Sub(m.Float32Constant(*i), t0);
    m.StoreToPointer(&output, kMachFloat32, t1);
    m.Return(m.Int32Constant(0));
    FOR_FLOAT32_INPUTS(j) {
      input = *j;
      float expected = *i - input;
      CHECK_EQ(0, m.Call());
      CheckFloatEq(expected, output);
    }
  }
}


TEST(RunFloat32SubImm2) {
  float input = 0.0f;
  float output = 0.0f;

  FOR_FLOAT32_INPUTS(i) {
    RawMachineAssemblerTester<int32_t> m;
    Node* t0 = m.LoadFromPointer(&input, kMachFloat32);
    Node* t1 = m.Float32Sub(t0, m.Float32Constant(*i));
    m.StoreToPointer(&output, kMachFloat32, t1);
    m.Return(m.Int32Constant(0));
    FOR_FLOAT32_INPUTS(j) {
      input = *j;
      float expected = input - *i;
      CHECK_EQ(0, m.Call());
      CheckFloatEq(expected, output);
    }
  }
}


TEST(RunFloat64SubP) {
  RawMachineAssemblerTester<int32_t> m;
  Float64BinopTester bt(&m);

  bt.AddReturn(m.Float64Sub(bt.param0, bt.param1));

  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      double expected = *pl - *pr;
      CheckDoubleEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64SubImm1) {
  double input = 0.0;
  double output = 0.0;

  FOR_FLOAT64_INPUTS(i) {
    RawMachineAssemblerTester<int32_t> m;
    Node* t0 = m.LoadFromPointer(&input, kMachFloat64);
    Node* t1 = m.Float64Sub(m.Float64Constant(*i), t0);
    m.StoreToPointer(&output, kMachFloat64, t1);
    m.Return(m.Int32Constant(0));
    FOR_FLOAT64_INPUTS(j) {
      input = *j;
      double expected = *i - input;
      CHECK_EQ(0, m.Call());
      CheckDoubleEq(expected, output);
    }
  }
}


TEST(RunFloat64SubImm2) {
  double input = 0.0;
  double output = 0.0;

  FOR_FLOAT64_INPUTS(i) {
    RawMachineAssemblerTester<int32_t> m;
    Node* t0 = m.LoadFromPointer(&input, kMachFloat64);
    Node* t1 = m.Float64Sub(t0, m.Float64Constant(*i));
    m.StoreToPointer(&output, kMachFloat64, t1);
    m.Return(m.Int32Constant(0));
    FOR_FLOAT64_INPUTS(j) {
      input = *j;
      double expected = input - *i;
      CHECK_EQ(0, m.Call());
      CheckDoubleEq(expected, output);
    }
  }
}


TEST(RunFloat32MulP) {
  RawMachineAssemblerTester<int32_t> m;
  Float32BinopTester bt(&m);

  bt.AddReturn(m.Float32Mul(bt.param0, bt.param1));

  FOR_FLOAT32_INPUTS(pl) {
    FOR_FLOAT32_INPUTS(pr) {
      float expected = *pl * *pr;
      CheckFloatEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64MulP) {
  RawMachineAssemblerTester<int32_t> m;
  Float64BinopTester bt(&m);

  bt.AddReturn(m.Float64Mul(bt.param0, bt.param1));

  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      double expected = *pl * *pr;
      CheckDoubleEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64MulAndFloat64AddP) {
  double input_a = 0.0;
  double input_b = 0.0;
  double input_c = 0.0;
  double output = 0.0;

  {
    RawMachineAssemblerTester<int32_t> m;
    Node* a = m.LoadFromPointer(&input_a, kMachFloat64);
    Node* b = m.LoadFromPointer(&input_b, kMachFloat64);
    Node* c = m.LoadFromPointer(&input_c, kMachFloat64);
    m.StoreToPointer(&output, kMachFloat64,
                     m.Float64Add(m.Float64Mul(a, b), c));
    m.Return(m.Int32Constant(0));
    FOR_FLOAT64_INPUTS(i) {
      FOR_FLOAT64_INPUTS(j) {
        FOR_FLOAT64_INPUTS(k) {
          input_a = *i;
          input_b = *j;
          input_c = *k;
          volatile double temp = input_a * input_b;
          volatile double expected = temp + input_c;
          CHECK_EQ(0, m.Call());
          CheckDoubleEq(expected, output);
        }
      }
    }
  }
  {
    RawMachineAssemblerTester<int32_t> m;
    Node* a = m.LoadFromPointer(&input_a, kMachFloat64);
    Node* b = m.LoadFromPointer(&input_b, kMachFloat64);
    Node* c = m.LoadFromPointer(&input_c, kMachFloat64);
    m.StoreToPointer(&output, kMachFloat64,
                     m.Float64Add(a, m.Float64Mul(b, c)));
    m.Return(m.Int32Constant(0));
    FOR_FLOAT64_INPUTS(i) {
      FOR_FLOAT64_INPUTS(j) {
        FOR_FLOAT64_INPUTS(k) {
          input_a = *i;
          input_b = *j;
          input_c = *k;
          volatile double temp = input_b * input_c;
          volatile double expected = input_a + temp;
          CHECK_EQ(0, m.Call());
          CheckDoubleEq(expected, output);
        }
      }
    }
  }
}


TEST(RunFloat64MulAndFloat64SubP) {
  double input_a = 0.0;
  double input_b = 0.0;
  double input_c = 0.0;
  double output = 0.0;

  RawMachineAssemblerTester<int32_t> m;
  Node* a = m.LoadFromPointer(&input_a, kMachFloat64);
  Node* b = m.LoadFromPointer(&input_b, kMachFloat64);
  Node* c = m.LoadFromPointer(&input_c, kMachFloat64);
  m.StoreToPointer(&output, kMachFloat64, m.Float64Sub(a, m.Float64Mul(b, c)));
  m.Return(m.Int32Constant(0));

  FOR_FLOAT64_INPUTS(i) {
    FOR_FLOAT64_INPUTS(j) {
      FOR_FLOAT64_INPUTS(k) {
        input_a = *i;
        input_b = *j;
        input_c = *k;
        volatile double temp = input_b * input_c;
        volatile double expected = input_a - temp;
        CHECK_EQ(0, m.Call());
        CheckDoubleEq(expected, output);
      }
    }
  }
}


TEST(RunFloat64MulImm) {
  double input = 0.0;
  double output = 0.0;

  {
    FOR_FLOAT64_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m;
      Node* t0 = m.LoadFromPointer(&input, kMachFloat64);
      Node* t1 = m.Float64Mul(m.Float64Constant(*i), t0);
      m.StoreToPointer(&output, kMachFloat64, t1);
      m.Return(m.Int32Constant(0));
      FOR_FLOAT64_INPUTS(j) {
        input = *j;
        double expected = *i * input;
        CHECK_EQ(0, m.Call());
        CheckDoubleEq(expected, output);
      }
    }
  }
  {
    FOR_FLOAT64_INPUTS(i) {
      RawMachineAssemblerTester<int32_t> m;
      Node* t0 = m.LoadFromPointer(&input, kMachFloat64);
      Node* t1 = m.Float64Mul(t0, m.Float64Constant(*i));
      m.StoreToPointer(&output, kMachFloat64, t1);
      m.Return(m.Int32Constant(0));
      FOR_FLOAT64_INPUTS(j) {
        input = *j;
        double expected = input * *i;
        CHECK_EQ(0, m.Call());
        CheckDoubleEq(expected, output);
      }
    }
  }
}


TEST(RunFloat32DivP) {
  RawMachineAssemblerTester<int32_t> m;
  Float32BinopTester bt(&m);

  bt.AddReturn(m.Float32Div(bt.param0, bt.param1));

  FOR_FLOAT32_INPUTS(pl) {
    FOR_FLOAT32_INPUTS(pr) {
      float expected = *pl / *pr;
      CheckFloatEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64DivP) {
  RawMachineAssemblerTester<int32_t> m;
  Float64BinopTester bt(&m);

  bt.AddReturn(m.Float64Div(bt.param0, bt.param1));

  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      double expected = *pl / *pr;
      CheckDoubleEq(expected, bt.call(*pl, *pr));
    }
  }
}


TEST(RunFloat64ModP) {
  RawMachineAssemblerTester<int32_t> m;
  Float64BinopTester bt(&m);

  bt.AddReturn(m.Float64Mod(bt.param0, bt.param1));

  FOR_FLOAT64_INPUTS(i) {
    FOR_FLOAT64_INPUTS(j) {
      double expected = modulo(*i, *j);
      double found = bt.call(*i, *j);
      CheckDoubleEq(expected, found);
    }
  }
}


TEST(RunChangeInt32ToFloat64_A) {
  RawMachineAssemblerTester<int32_t> m;
  int32_t magic = 0x986234;
  double result = 0;

  Node* convert = m.ChangeInt32ToFloat64(m.Int32Constant(magic));
  m.Store(kMachFloat64, m.PointerConstant(&result), m.Int32Constant(0),
          convert);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK_EQ(static_cast<double>(magic), result);
}


TEST(RunChangeInt32ToFloat64_B) {
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  double output = 0;

  Node* convert = m.ChangeInt32ToFloat64(m.Parameter(0));
  m.Store(kMachFloat64, m.PointerConstant(&output), m.Int32Constant(0),
          convert);
  m.Return(m.Parameter(0));

  FOR_INT32_INPUTS(i) {
    int32_t expect = *i;
    CHECK_EQ(expect, m.Call(expect));
    CHECK_EQ(static_cast<double>(expect), output);
  }
}


TEST(RunChangeUint32ToFloat64_B) {
  RawMachineAssemblerTester<uint32_t> m(kMachUint32);
  double output = 0;

  Node* convert = m.ChangeUint32ToFloat64(m.Parameter(0));
  m.Store(kMachFloat64, m.PointerConstant(&output), m.Int32Constant(0),
          convert);
  m.Return(m.Parameter(0));

  FOR_UINT32_INPUTS(i) {
    uint32_t expect = *i;
    CHECK_EQ(expect, m.Call(expect));
    CHECK_EQ(static_cast<double>(expect), output);
  }
}


TEST(RunChangeUint32ToFloat64_spilled) {
  RawMachineAssemblerTester<int32_t> m;
  const int kNumInputs = 32;
  int32_t magic = 0x786234;
  uint32_t input[kNumInputs];
  double result[kNumInputs];
  Node* input_node[kNumInputs];

  for (int i = 0; i < kNumInputs; i++) {
    input_node[i] =
        m.Load(kMachUint32, m.PointerConstant(&input), m.Int32Constant(i * 4));
  }

  for (int i = 0; i < kNumInputs; i++) {
    m.Store(kMachFloat64, m.PointerConstant(&result), m.Int32Constant(i * 8),
            m.ChangeUint32ToFloat64(input_node[i]));
  }

  m.Return(m.Int32Constant(magic));

  for (int i = 0; i < kNumInputs; i++) {
    input[i] = 100 + i;
  }

  CHECK_EQ(magic, m.Call());

  for (int i = 0; i < kNumInputs; i++) {
    CHECK_EQ(result[i], static_cast<double>(100 + i));
  }
}


TEST(RunChangeFloat64ToInt32_A) {
  RawMachineAssemblerTester<int32_t> m;
  int32_t magic = 0x786234;
  double input = 11.1;
  int32_t result = 0;

  m.Store(kMachInt32, m.PointerConstant(&result), m.Int32Constant(0),
          m.ChangeFloat64ToInt32(m.Float64Constant(input)));
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK_EQ(static_cast<int32_t>(input), result);
}


TEST(RunChangeFloat64ToInt32_B) {
  RawMachineAssemblerTester<int32_t> m;
  double input = 0;
  int32_t output = 0;

  Node* load =
      m.Load(kMachFloat64, m.PointerConstant(&input), m.Int32Constant(0));
  Node* convert = m.ChangeFloat64ToInt32(load);
  m.Store(kMachInt32, m.PointerConstant(&output), m.Int32Constant(0), convert);
  m.Return(convert);

  {
    FOR_INT32_INPUTS(i) {
      input = *i;
      int32_t expect = *i;
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }
  }

  // Check various powers of 2.
  for (int32_t n = 1; n < 31; ++n) {
    {
      input = 1 << n;
      int32_t expect = static_cast<int32_t>(input);
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }

    {
      input = 3 << n;
      int32_t expect = static_cast<int32_t>(input);
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }
  }
  // Note we don't check fractional inputs, because these Convert operators
  // really should be Change operators.
}


TEST(RunChangeFloat64ToUint32_B) {
  RawMachineAssemblerTester<int32_t> m;
  double input = 0;
  int32_t output = 0;

  Node* load =
      m.Load(kMachFloat64, m.PointerConstant(&input), m.Int32Constant(0));
  Node* convert = m.ChangeFloat64ToUint32(load);
  m.Store(kMachInt32, m.PointerConstant(&output), m.Int32Constant(0), convert);
  m.Return(convert);

  {
    FOR_UINT32_INPUTS(i) {
      input = *i;
      // TODO(titzer): add a CheckEqualsHelper overload for uint32_t.
      int32_t expect = static_cast<int32_t>(*i);
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }
  }

  // Check various powers of 2.
  for (int32_t n = 1; n < 31; ++n) {
    {
      input = 1u << n;
      int32_t expect = static_cast<int32_t>(static_cast<uint32_t>(input));
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }

    {
      input = 3u << n;
      int32_t expect = static_cast<int32_t>(static_cast<uint32_t>(input));
      CHECK_EQ(expect, m.Call());
      CHECK_EQ(expect, output);
    }
  }
  // Note we don't check fractional inputs, because these Convert operators
  // really should be Change operators.
}


TEST(RunChangeFloat64ToInt32_spilled) {
  RawMachineAssemblerTester<int32_t> m;
  const int kNumInputs = 32;
  int32_t magic = 0x786234;
  double input[kNumInputs];
  int32_t result[kNumInputs];
  Node* input_node[kNumInputs];

  for (int i = 0; i < kNumInputs; i++) {
    input_node[i] =
        m.Load(kMachFloat64, m.PointerConstant(&input), m.Int32Constant(i * 8));
  }

  for (int i = 0; i < kNumInputs; i++) {
    m.Store(kMachInt32, m.PointerConstant(&result), m.Int32Constant(i * 4),
            m.ChangeFloat64ToInt32(input_node[i]));
  }

  m.Return(m.Int32Constant(magic));

  for (int i = 0; i < kNumInputs; i++) {
    input[i] = 100.9 + i;
  }

  CHECK_EQ(magic, m.Call());

  for (int i = 0; i < kNumInputs; i++) {
    CHECK_EQ(result[i], 100 + i);
  }
}


TEST(RunChangeFloat64ToUint32_spilled) {
  RawMachineAssemblerTester<uint32_t> m;
  const int kNumInputs = 32;
  uint32_t magic = 0x786234;
  double input[kNumInputs];
  uint32_t result[kNumInputs];
  Node* input_node[kNumInputs];

  for (int i = 0; i < kNumInputs; i++) {
    input_node[i] =
        m.Load(kMachFloat64, m.PointerConstant(&input), m.Int32Constant(i * 8));
  }

  for (int i = 0; i < kNumInputs; i++) {
    m.Store(kMachUint32, m.PointerConstant(&result), m.Int32Constant(i * 4),
            m.ChangeFloat64ToUint32(input_node[i]));
  }

  m.Return(m.Int32Constant(magic));

  for (int i = 0; i < kNumInputs; i++) {
    if (i % 2) {
      input[i] = 100 + i + 2147483648u;
    } else {
      input[i] = 100 + i;
    }
  }

  CHECK_EQ(magic, m.Call());

  for (int i = 0; i < kNumInputs; i++) {
    if (i % 2) {
      CHECK_EQ(result[i], static_cast<uint32_t>(100 + i + 2147483648u));
    } else {
      CHECK_EQ(result[i], static_cast<uint32_t>(100 + i));
    }
  }
}


TEST(RunTruncateFloat64ToFloat32_spilled) {
  RawMachineAssemblerTester<uint32_t> m;
  const int kNumInputs = 32;
  uint32_t magic = 0x786234;
  double input[kNumInputs];
  float result[kNumInputs];
  Node* input_node[kNumInputs];

  for (int i = 0; i < kNumInputs; i++) {
    input_node[i] =
        m.Load(kMachFloat64, m.PointerConstant(&input), m.Int32Constant(i * 8));
  }

  for (int i = 0; i < kNumInputs; i++) {
    m.Store(kMachFloat32, m.PointerConstant(&result), m.Int32Constant(i * 4),
            m.TruncateFloat64ToFloat32(input_node[i]));
  }

  m.Return(m.Int32Constant(magic));

  for (int i = 0; i < kNumInputs; i++) {
    input[i] = 0.1 + i;
  }

  CHECK_EQ(magic, m.Call());

  for (int i = 0; i < kNumInputs; i++) {
    CHECK_EQ(result[i], DoubleToFloat32(input[i]));
  }
}


TEST(RunDeadChangeFloat64ToInt32) {
  RawMachineAssemblerTester<int32_t> m;
  const int magic = 0x88abcda4;
  m.ChangeFloat64ToInt32(m.Float64Constant(999.78));
  m.Return(m.Int32Constant(magic));
  CHECK_EQ(magic, m.Call());
}


TEST(RunDeadChangeInt32ToFloat64) {
  RawMachineAssemblerTester<int32_t> m;
  const int magic = 0x8834abcd;
  m.ChangeInt32ToFloat64(m.Int32Constant(magic - 6888));
  m.Return(m.Int32Constant(magic));
  CHECK_EQ(magic, m.Call());
}


TEST(RunLoopPhiInduction2) {
  RawMachineAssemblerTester<int32_t> m;

  int false_val = 0x10777;

  // x = false_val; while(false) { x++; } return x;
  MLabel header, body, end;
  Node* false_node = m.Int32Constant(false_val);
  m.Goto(&header);
  m.Bind(&header);
  Node* phi = m.Phi(kMachInt32, false_node, false_node);
  m.Branch(m.Int32Constant(0), &body, &end);
  m.Bind(&body);
  Node* add = m.Int32Add(phi, m.Int32Constant(1));
  phi->ReplaceInput(1, add);
  m.Goto(&header);
  m.Bind(&end);
  m.Return(phi);

  CHECK_EQ(false_val, m.Call());
}


TEST(RunFloatDiamond) {
  RawMachineAssemblerTester<int32_t> m;

  const int magic = 99645;
  float buffer = 0.1f;
  float constant = 99.99f;

  MLabel blocka, blockb, end;
  Node* k1 = m.Float32Constant(constant);
  Node* k2 = m.Float32Constant(0 - constant);
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);
  m.Bind(&end);
  Node* phi = m.Phi(kMachFloat32, k2, k1);
  m.Store(kMachFloat32, m.PointerConstant(&buffer), m.IntPtrConstant(0), phi);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK(constant == buffer);
}


TEST(RunDoubleDiamond) {
  RawMachineAssemblerTester<int32_t> m;

  const int magic = 99645;
  double buffer = 0.1;
  double constant = 99.99;

  MLabel blocka, blockb, end;
  Node* k1 = m.Float64Constant(constant);
  Node* k2 = m.Float64Constant(0 - constant);
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);
  m.Bind(&end);
  Node* phi = m.Phi(kMachFloat64, k2, k1);
  m.Store(kMachFloat64, m.PointerConstant(&buffer), m.Int32Constant(0), phi);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK_EQ(constant, buffer);
}


TEST(RunRefDiamond) {
  RawMachineAssemblerTester<int32_t> m;

  const int magic = 99644;
  Handle<String> rexpected =
      CcTest::i_isolate()->factory()->InternalizeUtf8String("A");
  String* buffer;

  MLabel blocka, blockb, end;
  Node* k1 = m.StringConstant("A");
  Node* k2 = m.StringConstant("B");
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);
  m.Bind(&end);
  Node* phi = m.Phi(kMachAnyTagged, k2, k1);
  m.Store(kMachAnyTagged, m.PointerConstant(&buffer), m.Int32Constant(0), phi);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK(rexpected->SameValue(buffer));
}


TEST(RunDoubleRefDiamond) {
  RawMachineAssemblerTester<int32_t> m;

  const int magic = 99648;
  double dbuffer = 0.1;
  double dconstant = 99.99;
  Handle<String> rexpected =
      CcTest::i_isolate()->factory()->InternalizeUtf8String("AX");
  String* rbuffer;

  MLabel blocka, blockb, end;
  Node* d1 = m.Float64Constant(dconstant);
  Node* d2 = m.Float64Constant(0 - dconstant);
  Node* r1 = m.StringConstant("AX");
  Node* r2 = m.StringConstant("BX");
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);
  m.Bind(&end);
  Node* dphi = m.Phi(kMachFloat64, d2, d1);
  Node* rphi = m.Phi(kMachAnyTagged, r2, r1);
  m.Store(kMachFloat64, m.PointerConstant(&dbuffer), m.Int32Constant(0), dphi);
  m.Store(kMachAnyTagged, m.PointerConstant(&rbuffer), m.Int32Constant(0),
          rphi);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK_EQ(dconstant, dbuffer);
  CHECK(rexpected->SameValue(rbuffer));
}


TEST(RunDoubleRefDoubleDiamond) {
  RawMachineAssemblerTester<int32_t> m;

  const int magic = 99649;
  double dbuffer = 0.1;
  double dconstant = 99.997;
  Handle<String> rexpected =
      CcTest::i_isolate()->factory()->InternalizeUtf8String("AD");
  String* rbuffer;

  MLabel blocka, blockb, mid, blockd, blocke, end;
  Node* d1 = m.Float64Constant(dconstant);
  Node* d2 = m.Float64Constant(0 - dconstant);
  Node* r1 = m.StringConstant("AD");
  Node* r2 = m.StringConstant("BD");
  m.Branch(m.Int32Constant(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&mid);
  m.Bind(&blockb);
  m.Goto(&mid);
  m.Bind(&mid);
  Node* dphi1 = m.Phi(kMachFloat64, d2, d1);
  Node* rphi1 = m.Phi(kMachAnyTagged, r2, r1);
  m.Branch(m.Int32Constant(0), &blockd, &blocke);

  m.Bind(&blockd);
  m.Goto(&end);
  m.Bind(&blocke);
  m.Goto(&end);
  m.Bind(&end);
  Node* dphi2 = m.Phi(kMachFloat64, d1, dphi1);
  Node* rphi2 = m.Phi(kMachAnyTagged, r1, rphi1);

  m.Store(kMachFloat64, m.PointerConstant(&dbuffer), m.Int32Constant(0), dphi2);
  m.Store(kMachAnyTagged, m.PointerConstant(&rbuffer), m.Int32Constant(0),
          rphi2);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
  CHECK_EQ(dconstant, dbuffer);
  CHECK(rexpected->SameValue(rbuffer));
}


TEST(RunDoubleLoopPhi) {
  RawMachineAssemblerTester<int32_t> m;
  MLabel header, body, end;

  int magic = 99773;
  double buffer = 0.99;
  double dconstant = 777.1;

  Node* zero = m.Int32Constant(0);
  Node* dk = m.Float64Constant(dconstant);

  m.Goto(&header);
  m.Bind(&header);
  Node* phi = m.Phi(kMachFloat64, dk, dk);
  phi->ReplaceInput(1, phi);
  m.Branch(zero, &body, &end);
  m.Bind(&body);
  m.Goto(&header);
  m.Bind(&end);
  m.Store(kMachFloat64, m.PointerConstant(&buffer), m.Int32Constant(0), phi);
  m.Return(m.Int32Constant(magic));

  CHECK_EQ(magic, m.Call());
}


TEST(RunCountToTenAccRaw) {
  RawMachineAssemblerTester<int32_t> m;

  Node* zero = m.Int32Constant(0);
  Node* ten = m.Int32Constant(10);
  Node* one = m.Int32Constant(1);

  MLabel header, body, body_cont, end;

  m.Goto(&header);

  m.Bind(&header);
  Node* i = m.Phi(kMachInt32, zero, zero);
  Node* j = m.Phi(kMachInt32, zero, zero);
  m.Goto(&body);

  m.Bind(&body);
  Node* next_i = m.Int32Add(i, one);
  Node* next_j = m.Int32Add(j, one);
  m.Branch(m.Word32Equal(next_i, ten), &end, &body_cont);

  m.Bind(&body_cont);
  i->ReplaceInput(1, next_i);
  j->ReplaceInput(1, next_j);
  m.Goto(&header);

  m.Bind(&end);
  m.Return(ten);

  CHECK_EQ(10, m.Call());
}


TEST(RunCountToTenAccRaw2) {
  RawMachineAssemblerTester<int32_t> m;

  Node* zero = m.Int32Constant(0);
  Node* ten = m.Int32Constant(10);
  Node* one = m.Int32Constant(1);

  MLabel header, body, body_cont, end;

  m.Goto(&header);

  m.Bind(&header);
  Node* i = m.Phi(kMachInt32, zero, zero);
  Node* j = m.Phi(kMachInt32, zero, zero);
  Node* k = m.Phi(kMachInt32, zero, zero);
  m.Goto(&body);

  m.Bind(&body);
  Node* next_i = m.Int32Add(i, one);
  Node* next_j = m.Int32Add(j, one);
  Node* next_k = m.Int32Add(j, one);
  m.Branch(m.Word32Equal(next_i, ten), &end, &body_cont);

  m.Bind(&body_cont);
  i->ReplaceInput(1, next_i);
  j->ReplaceInput(1, next_j);
  k->ReplaceInput(1, next_k);
  m.Goto(&header);

  m.Bind(&end);
  m.Return(ten);

  CHECK_EQ(10, m.Call());
}


TEST(RunAddTree) {
  RawMachineAssemblerTester<int32_t> m;
  int32_t inputs[] = {11, 12, 13, 14, 15, 16, 17, 18};

  Node* base = m.PointerConstant(inputs);
  Node* n0 = m.Load(kMachInt32, base, m.Int32Constant(0 * sizeof(int32_t)));
  Node* n1 = m.Load(kMachInt32, base, m.Int32Constant(1 * sizeof(int32_t)));
  Node* n2 = m.Load(kMachInt32, base, m.Int32Constant(2 * sizeof(int32_t)));
  Node* n3 = m.Load(kMachInt32, base, m.Int32Constant(3 * sizeof(int32_t)));
  Node* n4 = m.Load(kMachInt32, base, m.Int32Constant(4 * sizeof(int32_t)));
  Node* n5 = m.Load(kMachInt32, base, m.Int32Constant(5 * sizeof(int32_t)));
  Node* n6 = m.Load(kMachInt32, base, m.Int32Constant(6 * sizeof(int32_t)));
  Node* n7 = m.Load(kMachInt32, base, m.Int32Constant(7 * sizeof(int32_t)));

  Node* i1 = m.Int32Add(n0, n1);
  Node* i2 = m.Int32Add(n2, n3);
  Node* i3 = m.Int32Add(n4, n5);
  Node* i4 = m.Int32Add(n6, n7);

  Node* i5 = m.Int32Add(i1, i2);
  Node* i6 = m.Int32Add(i3, i4);

  Node* i7 = m.Int32Add(i5, i6);

  m.Return(i7);

  CHECK_EQ(116, m.Call());
}


static const int kFloat64CompareHelperTestCases = 15;
static const int kFloat64CompareHelperNodeType = 4;

static int Float64CompareHelper(RawMachineAssemblerTester<int32_t>* m,
                                int test_case, int node_type, double x,
                                double y) {
  static double buffer[2];
  buffer[0] = x;
  buffer[1] = y;
  CHECK(0 <= test_case && test_case < kFloat64CompareHelperTestCases);
  CHECK(0 <= node_type && node_type < kFloat64CompareHelperNodeType);
  CHECK(x < y);
  bool load_a = node_type / 2 == 1;
  bool load_b = node_type % 2 == 1;
  Node* a = load_a ? m->Load(kMachFloat64, m->PointerConstant(&buffer[0]))
                   : m->Float64Constant(x);
  Node* b = load_b ? m->Load(kMachFloat64, m->PointerConstant(&buffer[1]))
                   : m->Float64Constant(y);
  Node* cmp = NULL;
  bool expected = false;
  switch (test_case) {
    // Equal tests.
    case 0:
      cmp = m->Float64Equal(a, b);
      expected = false;
      break;
    case 1:
      cmp = m->Float64Equal(a, a);
      expected = true;
      break;
    // LessThan tests.
    case 2:
      cmp = m->Float64LessThan(a, b);
      expected = true;
      break;
    case 3:
      cmp = m->Float64LessThan(b, a);
      expected = false;
      break;
    case 4:
      cmp = m->Float64LessThan(a, a);
      expected = false;
      break;
    // LessThanOrEqual tests.
    case 5:
      cmp = m->Float64LessThanOrEqual(a, b);
      expected = true;
      break;
    case 6:
      cmp = m->Float64LessThanOrEqual(b, a);
      expected = false;
      break;
    case 7:
      cmp = m->Float64LessThanOrEqual(a, a);
      expected = true;
      break;
    // NotEqual tests.
    case 8:
      cmp = m->Float64NotEqual(a, b);
      expected = true;
      break;
    case 9:
      cmp = m->Float64NotEqual(b, a);
      expected = true;
      break;
    case 10:
      cmp = m->Float64NotEqual(a, a);
      expected = false;
      break;
    // GreaterThan tests.
    case 11:
      cmp = m->Float64GreaterThan(a, a);
      expected = false;
      break;
    case 12:
      cmp = m->Float64GreaterThan(a, b);
      expected = false;
      break;
    // GreaterThanOrEqual tests.
    case 13:
      cmp = m->Float64GreaterThanOrEqual(a, a);
      expected = true;
      break;
    case 14:
      cmp = m->Float64GreaterThanOrEqual(b, a);
      expected = true;
      break;
    default:
      UNREACHABLE();
  }
  m->Return(cmp);
  return expected;
}


TEST(RunFloat64Compare) {
  double inf = V8_INFINITY;
  // All pairs (a1, a2) are of the form a1 < a2.
  double inputs[] = {0.0,  1.0,  -1.0, 0.22, -1.22, 0.22,
                     -inf, 0.22, 0.22, inf,  -inf,  inf};

  for (int test = 0; test < kFloat64CompareHelperTestCases; test++) {
    for (int node_type = 0; node_type < kFloat64CompareHelperNodeType;
         node_type++) {
      for (size_t input = 0; input < arraysize(inputs); input += 2) {
        RawMachineAssemblerTester<int32_t> m;
        int expected = Float64CompareHelper(&m, test, node_type, inputs[input],
                                            inputs[input + 1]);
        CHECK_EQ(expected, m.Call());
      }
    }
  }
}


TEST(RunFloat64UnorderedCompare) {
  RawMachineAssemblerTester<int32_t> m;

  const Operator* operators[] = {m.machine()->Float64Equal(),
                                 m.machine()->Float64LessThan(),
                                 m.machine()->Float64LessThanOrEqual()};

  double nan = std::numeric_limits<double>::quiet_NaN();

  FOR_FLOAT64_INPUTS(i) {
    for (size_t o = 0; o < arraysize(operators); ++o) {
      for (int j = 0; j < 2; j++) {
        RawMachineAssemblerTester<int32_t> m;
        Node* a = m.Float64Constant(*i);
        Node* b = m.Float64Constant(nan);
        if (j == 1) std::swap(a, b);
        m.Return(m.AddNode(operators[o], a, b));
        CHECK_EQ(0, m.Call());
      }
    }
  }
}


TEST(RunFloat64Equal) {
  double input_a = 0.0;
  double input_b = 0.0;

  RawMachineAssemblerTester<int32_t> m;
  Node* a = m.LoadFromPointer(&input_a, kMachFloat64);
  Node* b = m.LoadFromPointer(&input_b, kMachFloat64);
  m.Return(m.Float64Equal(a, b));

  CompareWrapper cmp(IrOpcode::kFloat64Equal);
  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      input_a = *pl;
      input_b = *pr;
      int32_t expected = cmp.Float64Compare(input_a, input_b) ? 1 : 0;
      CHECK_EQ(expected, m.Call());
    }
  }
}


TEST(RunFloat64LessThan) {
  double input_a = 0.0;
  double input_b = 0.0;

  RawMachineAssemblerTester<int32_t> m;
  Node* a = m.LoadFromPointer(&input_a, kMachFloat64);
  Node* b = m.LoadFromPointer(&input_b, kMachFloat64);
  m.Return(m.Float64LessThan(a, b));

  CompareWrapper cmp(IrOpcode::kFloat64LessThan);
  FOR_FLOAT64_INPUTS(pl) {
    FOR_FLOAT64_INPUTS(pr) {
      input_a = *pl;
      input_b = *pr;
      int32_t expected = cmp.Float64Compare(input_a, input_b) ? 1 : 0;
      CHECK_EQ(expected, m.Call());
    }
  }
}


template <typename IntType, MachineType kRepresentation>
static void LoadStoreTruncation() {
  IntType input;

  RawMachineAssemblerTester<int32_t> m;
  Node* a = m.LoadFromPointer(&input, kRepresentation);
  Node* ap1 = m.Int32Add(a, m.Int32Constant(1));
  m.StoreToPointer(&input, kRepresentation, ap1);
  m.Return(ap1);

  const IntType max = std::numeric_limits<IntType>::max();
  const IntType min = std::numeric_limits<IntType>::min();

  // Test upper bound.
  input = max;
  CHECK_EQ(max + 1, m.Call());
  CHECK_EQ(min, input);

  // Test lower bound.
  input = min;
  CHECK_EQ(static_cast<IntType>(max + 2), m.Call());
  CHECK_EQ(min + 1, input);

  // Test all one byte values that are not one byte bounds.
  for (int i = -127; i < 127; i++) {
    input = i;
    int expected = i >= 0 ? i + 1 : max + (i - min) + 2;
    CHECK_EQ(static_cast<IntType>(expected), m.Call());
    CHECK_EQ(static_cast<IntType>(i + 1), input);
  }
}


TEST(RunLoadStoreTruncation) {
  LoadStoreTruncation<int8_t, kMachInt8>();
  LoadStoreTruncation<int16_t, kMachInt16>();
}


static void IntPtrCompare(intptr_t left, intptr_t right) {
  for (int test = 0; test < 7; test++) {
    RawMachineAssemblerTester<bool> m(kMachPtr, kMachPtr);
    Node* p0 = m.Parameter(0);
    Node* p1 = m.Parameter(1);
    Node* res = NULL;
    bool expected = false;
    switch (test) {
      case 0:
        res = m.IntPtrLessThan(p0, p1);
        expected = true;
        break;
      case 1:
        res = m.IntPtrLessThanOrEqual(p0, p1);
        expected = true;
        break;
      case 2:
        res = m.IntPtrEqual(p0, p1);
        expected = false;
        break;
      case 3:
        res = m.IntPtrGreaterThanOrEqual(p0, p1);
        expected = false;
        break;
      case 4:
        res = m.IntPtrGreaterThan(p0, p1);
        expected = false;
        break;
      case 5:
        res = m.IntPtrEqual(p0, p0);
        expected = true;
        break;
      case 6:
        res = m.IntPtrNotEqual(p0, p1);
        expected = true;
        break;
      default:
        UNREACHABLE();
        break;
    }
    m.Return(res);
    CHECK_EQ(expected, m.Call(reinterpret_cast<int32_t*>(left),
                              reinterpret_cast<int32_t*>(right)));
  }
}


TEST(RunIntPtrCompare) {
  intptr_t min = std::numeric_limits<intptr_t>::min();
  intptr_t max = std::numeric_limits<intptr_t>::max();
  // An ascending chain of intptr_t
  intptr_t inputs[] = {min, min / 2, -1, 0, 1, max / 2, max};
  for (size_t i = 0; i < arraysize(inputs) - 1; i++) {
    IntPtrCompare(inputs[i], inputs[i + 1]);
  }
}


TEST(RunTestIntPtrArithmetic) {
  static const int kInputSize = 10;
  int32_t inputs[kInputSize];
  int32_t outputs[kInputSize];
  for (int i = 0; i < kInputSize; i++) {
    inputs[i] = i;
    outputs[i] = -1;
  }
  RawMachineAssemblerTester<int32_t*> m;
  Node* input = m.PointerConstant(&inputs[0]);
  Node* output = m.PointerConstant(&outputs[kInputSize - 1]);
  Node* elem_size = m.IntPtrConstant(sizeof(inputs[0]));
  for (int i = 0; i < kInputSize; i++) {
    m.Store(kMachInt32, output, m.Load(kMachInt32, input));
    input = m.IntPtrAdd(input, elem_size);
    output = m.IntPtrSub(output, elem_size);
  }
  m.Return(input);
  CHECK_EQ(&inputs[kInputSize], m.Call());
  for (int i = 0; i < kInputSize; i++) {
    CHECK_EQ(i, inputs[i]);
    CHECK_EQ(kInputSize - i - 1, outputs[i]);
  }
}


TEST(RunSpillLotsOfThings) {
  static const int kInputSize = 1000;
  RawMachineAssemblerTester<int32_t> m;
  Node* accs[kInputSize];
  int32_t outputs[kInputSize];
  Node* one = m.Int32Constant(1);
  Node* acc = one;
  for (int i = 0; i < kInputSize; i++) {
    acc = m.Int32Add(acc, one);
    accs[i] = acc;
  }
  for (int i = 0; i < kInputSize; i++) {
    m.StoreToPointer(&outputs[i], kMachInt32, accs[i]);
  }
  m.Return(one);
  m.Call();
  for (int i = 0; i < kInputSize; i++) {
    CHECK_EQ(outputs[i], i + 2);
  }
}


TEST(RunSpillConstantsAndParameters) {
  static const int kInputSize = 1000;
  static const int32_t kBase = 987;
  RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
  int32_t outputs[kInputSize];
  Node* csts[kInputSize];
  Node* accs[kInputSize];
  Node* acc = m.Int32Constant(0);
  for (int i = 0; i < kInputSize; i++) {
    csts[i] = m.Int32Constant(static_cast<int32_t>(kBase + i));
  }
  for (int i = 0; i < kInputSize; i++) {
    acc = m.Int32Add(acc, csts[i]);
    accs[i] = acc;
  }
  for (int i = 0; i < kInputSize; i++) {
    m.StoreToPointer(&outputs[i], kMachInt32, accs[i]);
  }
  m.Return(m.Int32Add(acc, m.Int32Add(m.Parameter(0), m.Parameter(1))));
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected = *i + *j;
      for (int k = 0; k < kInputSize; k++) {
        expected += kBase + k;
      }
      CHECK_EQ(expected, m.Call(*i, *j));
      expected = 0;
      for (int k = 0; k < kInputSize; k++) {
        expected += kBase + k;
        CHECK_EQ(expected, outputs[k]);
      }
    }
  }
}


TEST(RunNewSpaceConstantsInPhi) {
  RawMachineAssemblerTester<Object*> m(kMachInt32);

  Isolate* isolate = CcTest::i_isolate();
  Handle<HeapNumber> true_val = isolate->factory()->NewHeapNumber(11.2);
  Handle<HeapNumber> false_val = isolate->factory()->NewHeapNumber(11.3);
  Node* true_node = m.HeapConstant(true_val);
  Node* false_node = m.HeapConstant(false_val);

  MLabel blocka, blockb, end;
  m.Branch(m.Parameter(0), &blocka, &blockb);
  m.Bind(&blocka);
  m.Goto(&end);
  m.Bind(&blockb);
  m.Goto(&end);

  m.Bind(&end);
  Node* phi = m.Phi(kMachAnyTagged, true_node, false_node);
  m.Return(phi);

  CHECK_EQ(*false_val, m.Call(0));
  CHECK_EQ(*true_val, m.Call(1));
}


TEST(RunInt32AddWithOverflowP) {
  int32_t actual_val = -1;
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  Node* add = m.Int32AddWithOverflow(bt.param0, bt.param1);
  Node* val = m.Projection(0, add);
  Node* ovf = m.Projection(1, add);
  m.StoreToPointer(&actual_val, kMachInt32, val);
  bt.AddReturn(ovf);
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected_val;
      int expected_ovf = bits::SignedAddOverflow32(*i, *j, &expected_val);
      CHECK_EQ(expected_ovf, bt.call(*i, *j));
      CHECK_EQ(expected_val, actual_val);
    }
  }
}


TEST(RunInt32AddWithOverflowImm) {
  int32_t actual_val = -1, expected_val = 0;
  FOR_INT32_INPUTS(i) {
    {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      Node* add = m.Int32AddWithOverflow(m.Int32Constant(*i), m.Parameter(0));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      FOR_INT32_INPUTS(j) {
        int expected_ovf = bits::SignedAddOverflow32(*i, *j, &expected_val);
        CHECK_EQ(expected_ovf, m.Call(*j));
        CHECK_EQ(expected_val, actual_val);
      }
    }
    {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      Node* add = m.Int32AddWithOverflow(m.Parameter(0), m.Int32Constant(*i));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      FOR_INT32_INPUTS(j) {
        int expected_ovf = bits::SignedAddOverflow32(*i, *j, &expected_val);
        CHECK_EQ(expected_ovf, m.Call(*j));
        CHECK_EQ(expected_val, actual_val);
      }
    }
    FOR_INT32_INPUTS(j) {
      RawMachineAssemblerTester<int32_t> m;
      Node* add =
          m.Int32AddWithOverflow(m.Int32Constant(*i), m.Int32Constant(*j));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      int expected_ovf = bits::SignedAddOverflow32(*i, *j, &expected_val);
      CHECK_EQ(expected_ovf, m.Call());
      CHECK_EQ(expected_val, actual_val);
    }
  }
}


TEST(RunInt32AddWithOverflowInBranchP) {
  int constant = 911777;
  MLabel blocka, blockb;
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  Node* add = m.Int32AddWithOverflow(bt.param0, bt.param1);
  Node* ovf = m.Projection(1, add);
  m.Branch(ovf, &blocka, &blockb);
  m.Bind(&blocka);
  bt.AddReturn(m.Int32Constant(constant));
  m.Bind(&blockb);
  Node* val = m.Projection(0, add);
  bt.AddReturn(val);
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected;
      if (bits::SignedAddOverflow32(*i, *j, &expected)) expected = constant;
      CHECK_EQ(expected, bt.call(*i, *j));
    }
  }
}


TEST(RunInt32SubWithOverflowP) {
  int32_t actual_val = -1;
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  Node* add = m.Int32SubWithOverflow(bt.param0, bt.param1);
  Node* val = m.Projection(0, add);
  Node* ovf = m.Projection(1, add);
  m.StoreToPointer(&actual_val, kMachInt32, val);
  bt.AddReturn(ovf);
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected_val;
      int expected_ovf = bits::SignedSubOverflow32(*i, *j, &expected_val);
      CHECK_EQ(expected_ovf, bt.call(*i, *j));
      CHECK_EQ(expected_val, actual_val);
    }
  }
}


TEST(RunInt32SubWithOverflowImm) {
  int32_t actual_val = -1, expected_val = 0;
  FOR_INT32_INPUTS(i) {
    {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      Node* add = m.Int32SubWithOverflow(m.Int32Constant(*i), m.Parameter(0));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      FOR_INT32_INPUTS(j) {
        int expected_ovf = bits::SignedSubOverflow32(*i, *j, &expected_val);
        CHECK_EQ(expected_ovf, m.Call(*j));
        CHECK_EQ(expected_val, actual_val);
      }
    }
    {
      RawMachineAssemblerTester<int32_t> m(kMachInt32);
      Node* add = m.Int32SubWithOverflow(m.Parameter(0), m.Int32Constant(*i));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      FOR_INT32_INPUTS(j) {
        int expected_ovf = bits::SignedSubOverflow32(*j, *i, &expected_val);
        CHECK_EQ(expected_ovf, m.Call(*j));
        CHECK_EQ(expected_val, actual_val);
      }
    }
    FOR_INT32_INPUTS(j) {
      RawMachineAssemblerTester<int32_t> m;
      Node* add =
          m.Int32SubWithOverflow(m.Int32Constant(*i), m.Int32Constant(*j));
      Node* val = m.Projection(0, add);
      Node* ovf = m.Projection(1, add);
      m.StoreToPointer(&actual_val, kMachInt32, val);
      m.Return(ovf);
      int expected_ovf = bits::SignedSubOverflow32(*i, *j, &expected_val);
      CHECK_EQ(expected_ovf, m.Call());
      CHECK_EQ(expected_val, actual_val);
    }
  }
}


TEST(RunInt32SubWithOverflowInBranchP) {
  int constant = 911999;
  MLabel blocka, blockb;
  RawMachineAssemblerTester<int32_t> m;
  Int32BinopTester bt(&m);
  Node* sub = m.Int32SubWithOverflow(bt.param0, bt.param1);
  Node* ovf = m.Projection(1, sub);
  m.Branch(ovf, &blocka, &blockb);
  m.Bind(&blocka);
  bt.AddReturn(m.Int32Constant(constant));
  m.Bind(&blockb);
  Node* val = m.Projection(0, sub);
  bt.AddReturn(val);
  FOR_INT32_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      int32_t expected;
      if (bits::SignedSubOverflow32(*i, *j, &expected)) expected = constant;
      CHECK_EQ(expected, bt.call(*i, *j));
    }
  }
}


TEST(RunWord64EqualInBranchP) {
  int64_t input;
  MLabel blocka, blockb;
  RawMachineAssemblerTester<int64_t> m;
  if (!m.machine()->Is64()) return;
  Node* value = m.LoadFromPointer(&input, kMachInt64);
  m.Branch(m.Word64Equal(value, m.Int64Constant(0)), &blocka, &blockb);
  m.Bind(&blocka);
  m.Return(m.Int32Constant(1));
  m.Bind(&blockb);
  m.Return(m.Int32Constant(2));
  input = V8_INT64_C(0);
  CHECK_EQ(1, m.Call());
  input = V8_INT64_C(1);
  CHECK_EQ(2, m.Call());
  input = V8_INT64_C(0x100000000);
  CHECK_EQ(2, m.Call());
}


TEST(RunChangeInt32ToInt64P) {
  if (kPointerSize < 8) return;
  int64_t actual = -1;
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  m.StoreToPointer(&actual, kMachInt64, m.ChangeInt32ToInt64(m.Parameter(0)));
  m.Return(m.Int32Constant(0));
  FOR_INT32_INPUTS(i) {
    int64_t expected = *i;
    CHECK_EQ(0, m.Call(*i));
    CHECK_EQ(expected, actual);
  }
}


TEST(RunChangeUint32ToUint64P) {
  if (kPointerSize < 8) return;
  int64_t actual = -1;
  RawMachineAssemblerTester<int32_t> m(kMachUint32);
  m.StoreToPointer(&actual, kMachUint64,
                   m.ChangeUint32ToUint64(m.Parameter(0)));
  m.Return(m.Int32Constant(0));
  FOR_UINT32_INPUTS(i) {
    int64_t expected = static_cast<uint64_t>(*i);
    CHECK_EQ(0, m.Call(*i));
    CHECK_EQ(expected, actual);
  }
}


TEST(RunTruncateInt64ToInt32P) {
  if (kPointerSize < 8) return;
  int64_t expected = -1;
  RawMachineAssemblerTester<int32_t> m;
  m.Return(m.TruncateInt64ToInt32(m.LoadFromPointer(&expected, kMachInt64)));
  FOR_UINT32_INPUTS(i) {
    FOR_UINT32_INPUTS(j) {
      expected = (static_cast<uint64_t>(*j) << 32) | *i;
      CHECK_EQ(static_cast<int32_t>(expected), m.Call());
    }
  }
}


TEST(RunTruncateFloat64ToInt32P) {
  struct {
    double from;
    double raw;
  } kValues[] = {{0, 0},
                 {0.5, 0},
                 {-0.5, 0},
                 {1.5, 1},
                 {-1.5, -1},
                 {5.5, 5},
                 {-5.0, -5},
                 {std::numeric_limits<double>::quiet_NaN(), 0},
                 {std::numeric_limits<double>::infinity(), 0},
                 {-std::numeric_limits<double>::quiet_NaN(), 0},
                 {-std::numeric_limits<double>::infinity(), 0},
                 {4.94065645841e-324, 0},
                 {-4.94065645841e-324, 0},
                 {0.9999999999999999, 0},
                 {-0.9999999999999999, 0},
                 {4294967296.0, 0},
                 {-4294967296.0, 0},
                 {9223372036854775000.0, 4294966272.0},
                 {-9223372036854775000.0, -4294966272.0},
                 {4.5036e+15, 372629504},
                 {-4.5036e+15, -372629504},
                 {287524199.5377777, 0x11234567},
                 {-287524199.5377777, -0x11234567},
                 {2300193596.302222, 2300193596.0},
                 {-2300193596.302222, -2300193596.0},
                 {4600387192.604444, 305419896},
                 {-4600387192.604444, -305419896},
                 {4823855600872397.0, 1737075661},
                 {-4823855600872397.0, -1737075661},
                 {4503603922337791.0, -1},
                 {-4503603922337791.0, 1},
                 {4503601774854143.0, 2147483647},
                 {-4503601774854143.0, -2147483647},
                 {9007207844675582.0, -2},
                 {-9007207844675582.0, 2},
                 {2.4178527921507624e+24, -536870912},
                 {-2.4178527921507624e+24, 536870912},
                 {2.417853945072267e+24, -536870912},
                 {-2.417853945072267e+24, 536870912},
                 {4.8357055843015248e+24, -1073741824},
                 {-4.8357055843015248e+24, 1073741824},
                 {4.8357078901445341e+24, -1073741824},
                 {-4.8357078901445341e+24, 1073741824},
                 {2147483647.0, 2147483647.0},
                 {-2147483648.0, -2147483648.0},
                 {9.6714111686030497e+24, -2147483648.0},
                 {-9.6714111686030497e+24, -2147483648.0},
                 {9.6714157802890681e+24, -2147483648.0},
                 {-9.6714157802890681e+24, -2147483648.0},
                 {1.9342813113834065e+25, 2147483648.0},
                 {-1.9342813113834065e+25, 2147483648.0},
                 {3.868562622766813e+25, 0},
                 {-3.868562622766813e+25, 0},
                 {1.7976931348623157e+308, 0},
                 {-1.7976931348623157e+308, 0}};
  double input = -1.0;
  RawMachineAssemblerTester<int32_t> m;
  m.Return(m.TruncateFloat64ToInt32(TruncationMode::kJavaScript,
                                    m.LoadFromPointer(&input, kMachFloat64)));
  for (size_t i = 0; i < arraysize(kValues); ++i) {
    input = kValues[i].from;
    uint64_t expected = static_cast<int64_t>(kValues[i].raw);
    CHECK_EQ(static_cast<int>(expected), m.Call());
  }
}


TEST(RunChangeFloat32ToFloat64) {
  double actual = 0.0f;
  float expected = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(
      &actual, kMachFloat64,
      m.ChangeFloat32ToFloat64(m.LoadFromPointer(&expected, kMachFloat32)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT32_INPUTS(i) {
    expected = *i;
    CHECK_EQ(0, m.Call());
    CHECK_EQ(static_cast<double>(expected), actual);
  }
}


TEST(RunChangeFloat32ToFloat64_spilled) {
  RawMachineAssemblerTester<int32_t> m;
  const int kNumInputs = 32;
  int32_t magic = 0x786234;
  float input[kNumInputs];
  double result[kNumInputs];
  Node* input_node[kNumInputs];

  for (int i = 0; i < kNumInputs; i++) {
    input_node[i] =
        m.Load(kMachFloat32, m.PointerConstant(&input), m.Int32Constant(i * 4));
  }

  for (int i = 0; i < kNumInputs; i++) {
    m.Store(kMachFloat64, m.PointerConstant(&result), m.Int32Constant(i * 8),
            m.ChangeFloat32ToFloat64(input_node[i]));
  }

  m.Return(m.Int32Constant(magic));

  for (int i = 0; i < kNumInputs; i++) {
    input[i] = 100.9f + i;
  }

  CHECK_EQ(magic, m.Call());

  for (int i = 0; i < kNumInputs; i++) {
    CHECK_EQ(result[i], static_cast<double>(input[i]));
  }
}


TEST(RunTruncateFloat64ToFloat32) {
  float actual = 0.0f;
  double input = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(
      &actual, kMachFloat32,
      m.TruncateFloat64ToFloat32(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT64_INPUTS(i) {
    input = *i;
    volatile double expected = DoubleToFloat32(input);
    CHECK_EQ(0, m.Call());
    CheckDoubleEq(expected, actual);
  }
}


TEST(RunFloat32Constant) {
  FOR_FLOAT32_INPUTS(i) {
    float expected = *i;
    float actual = *i;
    RawMachineAssemblerTester<int32_t> m;
    m.StoreToPointer(&actual, kMachFloat32, m.Float32Constant(expected));
    m.Return(m.Int32Constant(0));
    CHECK_EQ(0, m.Call());
    CHECK_EQ(expected, actual);
  }
}


TEST(RunFloat64ExtractLowWord32) {
  uint64_t input = 0;
  RawMachineAssemblerTester<int32_t> m;
  m.Return(m.Float64ExtractLowWord32(m.LoadFromPointer(&input, kMachFloat64)));
  FOR_FLOAT64_INPUTS(i) {
    input = bit_cast<uint64_t>(*i);
    int32_t expected = bit_cast<int32_t>(static_cast<uint32_t>(input));
    CHECK_EQ(expected, m.Call());
  }
}


TEST(RunFloat64ExtractHighWord32) {
  uint64_t input = 0;
  RawMachineAssemblerTester<int32_t> m;
  m.Return(m.Float64ExtractHighWord32(m.LoadFromPointer(&input, kMachFloat64)));
  FOR_FLOAT64_INPUTS(i) {
    input = bit_cast<uint64_t>(*i);
    int32_t expected = bit_cast<int32_t>(static_cast<uint32_t>(input >> 32));
    CHECK_EQ(expected, m.Call());
  }
}


TEST(RunFloat64InsertLowWord32) {
  uint64_t input = 0;
  uint64_t result = 0;
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  m.StoreToPointer(
      &result, kMachFloat64,
      m.Float64InsertLowWord32(m.LoadFromPointer(&input, kMachFloat64),
                               m.Parameter(0)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT64_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      input = bit_cast<uint64_t>(*i);
      uint64_t expected = (input & ~(V8_UINT64_C(0xFFFFFFFF))) |
                          (static_cast<uint64_t>(bit_cast<uint32_t>(*j)));
      CHECK_EQ(0, m.Call(*j));
      CHECK_EQ(expected, result);
    }
  }
}


TEST(RunFloat64InsertHighWord32) {
  uint64_t input = 0;
  uint64_t result = 0;
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  m.StoreToPointer(
      &result, kMachFloat64,
      m.Float64InsertHighWord32(m.LoadFromPointer(&input, kMachFloat64),
                                m.Parameter(0)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT64_INPUTS(i) {
    FOR_INT32_INPUTS(j) {
      input = bit_cast<uint64_t>(*i);
      uint64_t expected = (input & ~(V8_UINT64_C(0xFFFFFFFF) << 32)) |
                          (static_cast<uint64_t>(bit_cast<uint32_t>(*j)) << 32);
      CHECK_EQ(0, m.Call(*j));
      CHECK_EQ(expected, result);
    }
  }
}


TEST(RunFloat32Abs) {
  float input = -1.0;
  float result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(&result, kMachFloat32,
                   m.Float32Abs(m.LoadFromPointer(&input, kMachFloat32)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT32_INPUTS(i) {
    input = *i;
    float expected = std::abs(input);
    CHECK_EQ(0, m.Call());
    CheckFloatEq(expected, result);
  }
}


TEST(RunFloat64Abs) {
  double input = -1.0;
  double result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(&result, kMachFloat64,
                   m.Float64Abs(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(0));
  FOR_FLOAT64_INPUTS(i) {
    input = *i;
    double expected = std::abs(input);
    CHECK_EQ(0, m.Call());
    CheckDoubleEq(expected, result);
  }
}


static double two_30 = 1 << 30;             // 2^30 is a smi boundary.
static double two_52 = two_30 * (1 << 22);  // 2^52 is a precision boundary.
static double kValues[] = {0.1,
                           0.2,
                           0.49999999999999994,
                           0.5,
                           0.7,
                           1.0 - std::numeric_limits<double>::epsilon(),
                           -0.1,
                           -0.49999999999999994,
                           -0.5,
                           -0.7,
                           1.1,
                           1.0 + std::numeric_limits<double>::epsilon(),
                           1.5,
                           1.7,
                           -1,
                           -1 + std::numeric_limits<double>::epsilon(),
                           -1 - std::numeric_limits<double>::epsilon(),
                           -1.1,
                           -1.5,
                           -1.7,
                           std::numeric_limits<double>::min(),
                           -std::numeric_limits<double>::min(),
                           std::numeric_limits<double>::max(),
                           -std::numeric_limits<double>::max(),
                           std::numeric_limits<double>::infinity(),
                           -std::numeric_limits<double>::infinity(),
                           two_30,
                           two_30 + 0.1,
                           two_30 + 0.5,
                           two_30 + 0.7,
                           two_30 - 1,
                           two_30 - 1 + 0.1,
                           two_30 - 1 + 0.5,
                           two_30 - 1 + 0.7,
                           -two_30,
                           -two_30 + 0.1,
                           -two_30 + 0.5,
                           -two_30 + 0.7,
                           -two_30 + 1,
                           -two_30 + 1 + 0.1,
                           -two_30 + 1 + 0.5,
                           -two_30 + 1 + 0.7,
                           two_52,
                           two_52 + 0.1,
                           two_52 + 0.5,
                           two_52 + 0.5,
                           two_52 + 0.7,
                           two_52 + 0.7,
                           two_52 - 1,
                           two_52 - 1 + 0.1,
                           two_52 - 1 + 0.5,
                           two_52 - 1 + 0.7,
                           -two_52,
                           -two_52 + 0.1,
                           -two_52 + 0.5,
                           -two_52 + 0.7,
                           -two_52 + 1,
                           -two_52 + 1 + 0.1,
                           -two_52 + 1 + 0.5,
                           -two_52 + 1 + 0.7,
                           two_30,
                           two_30 - 0.1,
                           two_30 - 0.5,
                           two_30 - 0.7,
                           two_30 - 1,
                           two_30 - 1 - 0.1,
                           two_30 - 1 - 0.5,
                           two_30 - 1 - 0.7,
                           -two_30,
                           -two_30 - 0.1,
                           -two_30 - 0.5,
                           -two_30 - 0.7,
                           -two_30 + 1,
                           -two_30 + 1 - 0.1,
                           -two_30 + 1 - 0.5,
                           -two_30 + 1 - 0.7,
                           two_52,
                           two_52 - 0.1,
                           two_52 - 0.5,
                           two_52 - 0.5,
                           two_52 - 0.7,
                           two_52 - 0.7,
                           two_52 - 1,
                           two_52 - 1 - 0.1,
                           two_52 - 1 - 0.5,
                           two_52 - 1 - 0.7,
                           -two_52,
                           -two_52 - 0.1,
                           -two_52 - 0.5,
                           -two_52 - 0.7,
                           -two_52 + 1,
                           -two_52 + 1 - 0.1,
                           -two_52 + 1 - 0.5,
                           -two_52 + 1 - 0.7};


TEST(RunFloat64RoundDown1) {
  double input = -1.0;
  double result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  if (!m.machine()->Float64RoundDown().IsSupported()) return;
  m.StoreToPointer(&result, kMachFloat64,
                   m.Float64RoundDown(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(0));
  for (size_t i = 0; i < arraysize(kValues); ++i) {
    input = kValues[i];
    CHECK_EQ(0, m.Call());
    double expected = std::floor(kValues[i]);
    CHECK_EQ(expected, result);
  }
}


TEST(RunFloat64RoundDown2) {
  double input = -1.0;
  double result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  if (!m.machine()->Float64RoundDown().IsSupported()) return;
  m.StoreToPointer(&result, kMachFloat64,
                   m.Float64Sub(m.Float64Constant(-0.0),
                                m.Float64RoundDown(m.Float64Sub(
                                    m.Float64Constant(-0.0),
                                    m.LoadFromPointer(&input, kMachFloat64)))));
  m.Return(m.Int32Constant(0));
  for (size_t i = 0; i < arraysize(kValues); ++i) {
    input = kValues[i];
    CHECK_EQ(0, m.Call());
    double expected = std::ceil(kValues[i]);
    CHECK_EQ(expected, result);
  }
}


TEST(RunFloat64RoundTruncate) {
  double input = -1.0;
  double result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  if (!m.machine()->Float64RoundTruncate().IsSupported()) return;
  m.StoreToPointer(
      &result, kMachFloat64,
      m.Float64RoundTruncate(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(0));
  for (size_t i = 0; i < arraysize(kValues); ++i) {
    input = kValues[i];
    CHECK_EQ(0, m.Call());
    double expected = trunc(kValues[i]);
    CHECK_EQ(expected, result);
  }
}


TEST(RunFloat64RoundTiesAway) {
  double input = -1.0;
  double result = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  if (!m.machine()->Float64RoundTiesAway().IsSupported()) return;
  m.StoreToPointer(
      &result, kMachFloat64,
      m.Float64RoundTiesAway(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(0));
  for (size_t i = 0; i < arraysize(kValues); ++i) {
    input = kValues[i];
    CHECK_EQ(0, m.Call());
    double expected = round(kValues[i]);
    CHECK_EQ(expected, result);
  }
}


#if !USE_SIMULATOR

namespace {

int32_t const kMagicFoo0 = 0xdeadbeef;


int32_t foo0() { return kMagicFoo0; }


int32_t foo1(int32_t x) { return x; }


int32_t foo2(int32_t x, int32_t y) { return x - y; }


int32_t foo8(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f,
             int32_t g, int32_t h) {
  return a + b + c + d + e + f + g + h;
}

}  // namespace


TEST(RunCallCFunction0) {
  auto* foo0_ptr = &foo0;
  RawMachineAssemblerTester<int32_t> m;
  Node* function = m.LoadFromPointer(&foo0_ptr, kMachPtr);
  m.Return(m.CallCFunction0(kMachInt32, function));
  CHECK_EQ(kMagicFoo0, m.Call());
}


TEST(RunCallCFunction1) {
  auto* foo1_ptr = &foo1;
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  Node* function = m.LoadFromPointer(&foo1_ptr, kMachPtr);
  m.Return(m.CallCFunction1(kMachInt32, kMachInt32, function, m.Parameter(0)));
  FOR_INT32_INPUTS(i) {
    int32_t const expected = *i;
    CHECK_EQ(expected, m.Call(expected));
  }
}


TEST(RunCallCFunction2) {
  auto* foo2_ptr = &foo2;
  RawMachineAssemblerTester<int32_t> m(kMachInt32, kMachInt32);
  Node* function = m.LoadFromPointer(&foo2_ptr, kMachPtr);
  m.Return(m.CallCFunction2(kMachInt32, kMachInt32, kMachInt32, function,
                            m.Parameter(0), m.Parameter(1)));
  FOR_INT32_INPUTS(i) {
    int32_t const x = *i;
    FOR_INT32_INPUTS(j) {
      int32_t const y = *j;
      CHECK_EQ(x - y, m.Call(x, y));
    }
  }
}


TEST(RunCallCFunction8) {
  auto* foo8_ptr = &foo8;
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  Node* function = m.LoadFromPointer(&foo8_ptr, kMachPtr);
  Node* param = m.Parameter(0);
  m.Return(m.CallCFunction8(kMachInt32, kMachInt32, kMachInt32, kMachInt32,
                            kMachInt32, kMachInt32, kMachInt32, kMachInt32,
                            kMachInt32, function, param, param, param, param,
                            param, param, param, param));
  FOR_INT32_INPUTS(i) {
    int32_t const x = *i;
    CHECK_EQ(x * 8, m.Call(x));
  }
}
#endif  // USE_SIMULATOR

#if V8_TARGET_ARCH_64_BIT
// TODO(titzer): run int64 tests on all platforms when supported.
TEST(RunCheckedLoadInt64) {
  int64_t buffer[] = {0x66bbccddeeff0011LL, 0x1122334455667788LL};
  RawMachineAssemblerTester<int64_t> m(kMachInt32);
  Node* base = m.PointerConstant(buffer);
  Node* index = m.Parameter(0);
  Node* length = m.Int32Constant(16);
  Node* load =
      m.AddNode(m.machine()->CheckedLoad(kMachInt64), base, index, length);
  m.Return(load);

  CHECK_EQ(buffer[0], m.Call(0));
  CHECK_EQ(buffer[1], m.Call(8));
  CHECK_EQ(0, m.Call(16));
}


TEST(RunCheckedStoreInt64) {
  const int64_t write = 0x5566778899aabbLL;
  const int64_t before = 0x33bbccddeeff0011LL;
  int64_t buffer[] = {before, before};
  RawMachineAssemblerTester<int32_t> m(kMachInt32);
  Node* base = m.PointerConstant(buffer);
  Node* index = m.Parameter(0);
  Node* length = m.Int32Constant(16);
  Node* value = m.Int64Constant(write);
  Node* store = m.AddNode(m.machine()->CheckedStore(kMachInt64), base, index,
                          length, value);
  USE(store);
  m.Return(m.Int32Constant(11));

  CHECK_EQ(11, m.Call(16));
  CHECK_EQ(before, buffer[0]);
  CHECK_EQ(before, buffer[1]);

  CHECK_EQ(11, m.Call(0));
  CHECK_EQ(write, buffer[0]);
  CHECK_EQ(before, buffer[1]);

  CHECK_EQ(11, m.Call(8));
  CHECK_EQ(write, buffer[0]);
  CHECK_EQ(write, buffer[1]);
}


TEST(RunBitcastInt64ToFloat64) {
  int64_t input = 1;
  double output = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(
      &output, kMachFloat64,
      m.BitcastInt64ToFloat64(m.LoadFromPointer(&input, kMachInt64)));
  m.Return(m.Int32Constant(11));
  FOR_INT64_INPUTS(i) {
    input = *i;
    CHECK_EQ(11, m.Call());
    double expected = bit_cast<double>(input);
    CHECK_EQ(bit_cast<int64_t>(expected), bit_cast<int64_t>(output));
  }
}


TEST(RunBitcastFloat64ToInt64) {
  double input = 0;
  int64_t output = 0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(
      &output, kMachInt64,
      m.BitcastFloat64ToInt64(m.LoadFromPointer(&input, kMachFloat64)));
  m.Return(m.Int32Constant(11));
  FOR_FLOAT64_INPUTS(i) {
    input = *i;
    CHECK_EQ(11, m.Call());
    double expected = bit_cast<int64_t>(input);
    CHECK_EQ(expected, output);
  }
}
#endif


TEST(RunBitcastFloat32ToInt32) {
  float input = 32.25;
  RawMachineAssemblerTester<int32_t> m;
  m.Return(m.BitcastFloat32ToInt32(m.LoadFromPointer(&input, kMachFloat32)));
  FOR_FLOAT32_INPUTS(i) {
    input = *i;
    int32_t expected = bit_cast<int32_t>(input);
    CHECK_EQ(expected, m.Call());
  }
}


TEST(RunBitcastInt32ToFloat32) {
  int32_t input = 1;
  float output = 0.0;
  RawMachineAssemblerTester<int32_t> m;
  m.StoreToPointer(
      &output, kMachFloat32,
      m.BitcastInt32ToFloat32(m.LoadFromPointer(&input, kMachInt32)));
  m.Return(m.Int32Constant(11));
  FOR_INT32_INPUTS(i) {
    input = *i;
    CHECK_EQ(11, m.Call());
    float expected = bit_cast<float>(input);
    CHECK_EQ(bit_cast<int32_t>(expected), bit_cast<int32_t>(output));
  }
}


TEST(RunComputedCodeObject) {
  RawMachineAssemblerTester<int32_t> a;
  a.Return(a.Int32Constant(33));
  CHECK_EQ(33, a.Call());

  RawMachineAssemblerTester<int32_t> b;
  b.Return(b.Int32Constant(44));
  CHECK_EQ(44, b.Call());

  RawMachineAssemblerTester<int32_t> r(kMachInt32);
  RawMachineAssembler::Label tlabel;
  RawMachineAssembler::Label flabel;
  RawMachineAssembler::Label merge;
  r.Branch(r.Parameter(0), &tlabel, &flabel);
  r.Bind(&tlabel);
  Node* fa = r.HeapConstant(a.GetCode());
  r.Goto(&merge);
  r.Bind(&flabel);
  Node* fb = r.HeapConstant(b.GetCode());
  r.Goto(&merge);
  r.Bind(&merge);
  Node* phi = r.Phi(kMachInt32, fa, fb);

  // TODO(titzer): all this descriptor hackery is just to call the above
  // functions as code objects instead of direct addresses.
  CSignature0<int32_t> sig;
  CallDescriptor* c = Linkage::GetSimplifiedCDescriptor(r.zone(), &sig);
  LinkageLocation ret[] = {c->GetReturnLocation(0)};
  Signature<LinkageLocation> loc(1, 0, ret);
  CallDescriptor* desc = new (r.zone()) CallDescriptor(  // --
      CallDescriptor::kCallCodeObject,                   // kind
      kMachAnyTagged,                                    // target_type
      c->GetInputLocation(0),                            // target_loc
      &sig,                                              // machine_sig
      &loc,                                              // location_sig
      0,                                                 // stack count
      Operator::kNoProperties,                           // properties
      c->CalleeSavedRegisters(),                         // callee saved
      c->CalleeSavedFPRegisters(),                       // callee saved FP
      CallDescriptor::kNoFlags,                          // flags
      "c-call-as-code");
  Node* call = r.AddNode(r.common()->Call(desc), phi);
  r.Return(call);

  CHECK_EQ(33, r.Call(1));
  CHECK_EQ(44, r.Call(0));
}
