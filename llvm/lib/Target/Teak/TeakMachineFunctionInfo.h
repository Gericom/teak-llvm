//===-- TeakMachineFuctionInfo.h - Teak machine function info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Teak-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKMACHINEFUNCTIONINFO_H
#define TEAKMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

// Forward declarations
class Function;

/// TeakFunctionInfo - This class is derived from MachineFunction private
/// Teak target-specific information for each MachineFunction.
class TeakFunctionInfo : public MachineFunctionInfo {
public:
  TeakFunctionInfo() {}

  ~TeakFunctionInfo() {}
};
} // End llvm namespace

#endif // TEAKMACHINEFUNCTIONINFO_H
