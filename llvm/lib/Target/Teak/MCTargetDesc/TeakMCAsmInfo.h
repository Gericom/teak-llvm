//===-- TeakMCAsmInfo.h - Teak asm properties --------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TeakMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKTARGETASMINFO_H
#define TEAKTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class StringRef;
class Target;
class Triple;

class TeakMCAsmInfo : public MCAsmInfoELF {
  virtual void anchor();

public:
  explicit TeakMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif