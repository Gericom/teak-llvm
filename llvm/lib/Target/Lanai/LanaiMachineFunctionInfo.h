//===- LanaiMachineFuctionInfo.h - Lanai machine func info -------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares Lanai-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_LANAI_LANAIMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_LANAI_LANAIMACHINEFUNCTIONINFO_H

#include "LanaiRegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

namespace llvm {

// LanaiMachineFunctionInfo - This class is derived from MachineFunction and
// contains private Lanai target-specific information for each MachineFunction.
class LanaiMachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

  MachineFunction &MF;

  // SRetReturnReg - Lanai ABI require that sret lowering includes
  // returning the value of the returned struct in a register. This field
  // holds the virtual register into which the sret argument is passed.
  unsigned SRetReturnReg;

  // GlobalBaseReg - keeps track of the virtual register initialized for
  // use as the global base register. This is used for PIC in some PIC
  // relocation models.
  unsigned GlobalBaseReg;

  // VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex;

public:
  explicit LanaiMachineFunctionInfo(MachineFunction &MF)
      : MF(MF), SRetReturnReg(0), GlobalBaseReg(0), VarArgsFrameIndex(0) {}

  unsigned getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(unsigned Reg) { SRetReturnReg = Reg; }

  unsigned getGlobalBaseReg();

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_LANAI_LANAIMACHINEFUNCTIONINFO_H
