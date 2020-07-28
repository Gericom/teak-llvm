//===-- TeakTargetMachine.cpp - Define TargetMachine for Teak -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/TeakTargetInfo.h"
#include "TeakTargetMachine.h"
#include "Teak.h"
#include "TeakFrameLowering.h"
#include "TeakInstrInfo.h"
#include "TeakISelLowering.h"
#include "TeakSelectionDAGInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options) {
  // XXX Build the triple from the arguments.
  // This is hard-coded for now for this example target.
  return "E-m:e-P1-p0:16:16:16-p1:32:32:32-i1:16:16-i8:16:16-i16:16:16-i32:32:32-a:0:32-n16:40";
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

TeakTargetMachine::TeakTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                                   CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options), TT, CPU, FS,
                        Options, getEffectiveRelocModel(RM), getEffectiveCodeModel(CM, CodeModel::Small), OL),
      Subtarget(TT, CPU, FS, *this),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()) {
  initAsmInfo();
}

namespace {
/// Teak Code Generator Pass Configuration Options.
class TeakPassConfig : public TargetPassConfig
{
public:
  TeakPassConfig(TeakTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

    TeakTargetMachine& getTeakTargetMachine() const {
        return getTM<TeakTargetMachine>();
    }

    virtual bool addPreISel() override;
    virtual void addPreSched2() override;
    virtual bool addInstSelector() override;
    virtual void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *TeakTargetMachine::createPassConfig(PassManagerBase &PM) 
{
    return new TeakPassConfig(*this, PM);
}

bool TeakPassConfig::addPreISel() 
{ 
    addPass(createHardwareLoopsPass());
    return false;
}

void TeakPassConfig::addPreSched2()
{
    if (getOptLevel() != CodeGenOpt::None)
        addPass(&IfConverterID);
}

bool TeakPassConfig::addInstSelector()
{
    addPass(createTeakISelDag(getTeakTargetMachine(), getOptLevel()));
    return false;
}

void TeakPassConfig::addPreEmitPass()
{
    addPass(createTeakOptimizeMovImmPass());
    addPass(&BranchRelaxationPassID);
}

// Force static initialization.
extern "C" void LLVMInitializeTeakTarget()
{
    RegisterTargetMachine<TeakTargetMachine> X(getTheTeakTarget());
}