//===-- TeakTargetInfo.cpp - Teak Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Teak.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
  Target &getTheTeakTarget() {
    static Target TheTeakTarget;
    return TheTeakTarget;
  }
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTeakTargetInfo() {
  llvm::RegisterTarget<llvm::Triple::teak> X(llvm::getTheTeakTarget(), "teak", "Teak DSP", "Teak");
}