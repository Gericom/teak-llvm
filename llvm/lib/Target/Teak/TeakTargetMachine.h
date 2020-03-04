//===-- TeakTargetMachine.h - Define TargetMachine for Teak ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Teak specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKTARGETMACHINE_H
#define TEAKTARGETMACHINE_H

#include "Teak.h"
#include "TeakFrameLowering.h"
#include "TeakISelLowering.h"
#include "TeakInstrInfo.h"
#include "TeakSelectionDAGInfo.h"
#include "TeakSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class TeakTargetMachine : public LLVMTargetMachine {
  TeakSubtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  TeakTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options, Optional<Reloc::Model> RM, 
                   Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT);
  
  const TeakSubtarget * getSubtargetImpl() const {
    return &Subtarget;
  }
  
  virtual const TargetSubtargetInfo *
  getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};

} // end namespace llvm

#endif