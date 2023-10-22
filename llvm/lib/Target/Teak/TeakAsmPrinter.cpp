//===-- TeakAsmPrinter.cpp - Teak LLVM assembly writer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the XAS-format Teak assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "TargetInfo/TeakTargetInfo.h"
#include "Teak.h"
#include "InstPrinter/TeakInstPrinter.h"
#include "TeakInstrInfo.h"
#include "TeakMCInstLower.h"
#include "TeakSubtarget.h"
#include "TeakTargetMachine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include <algorithm>
#include <cctype>
using namespace llvm;

namespace {
class TeakAsmPrinter : public AsmPrinter {
  TeakMCInstLower MCInstLowering;
  Mangler *Mang;

public:
  explicit TeakAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(*this), Mang(new Mangler()) {}

  StringRef getPassName() const override { return "Teak Assembly Printer"; }

  void EmitFunctionEntryLabel();
  void EmitInstruction(const MachineInstr *MI);
  void EmitFunctionBodyStart();
};
} // end of anonymous namespace

void TeakAsmPrinter::EmitFunctionBodyStart() {
  MCInstLowering.Initialize(Mang, &MF->getContext());
}

void TeakAsmPrinter::EmitFunctionEntryLabel() {
  OutStreamer->EmitLabel(CurrentFnSym);
}

void TeakAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);

  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTeakAsmPrinter() {
  RegisterAsmPrinter<TeakAsmPrinter> X(getTheTeakTarget());
}
