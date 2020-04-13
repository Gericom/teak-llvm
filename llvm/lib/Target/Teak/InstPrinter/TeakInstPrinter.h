//== TeakInstPrinter.h - Convert Teak MCInst to assembly syntax -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the TeakInstPrinter class,
/// which is used to print Teak MCInst to a .s file.
///
//===----------------------------------------------------------------------===//

#ifndef TEAKINSTPRINTER_H
#define TEAKINSTPRINTER_H
#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class TargetMachine;

class TeakInstPrinter : public MCInstPrinter {
public:
  TeakInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                 const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  // Autogenerated by tblgen.
  void printInstruction(const MCInst *, uint64_t, raw_ostream &);
  static const char *getRegisterName(unsigned RegNo);

  void printRegName(raw_ostream &OS, unsigned RegNo) const override;
  void printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                 const MCSubtargetInfo &STI, raw_ostream &OS) override;

private:
  void printCondCode(const MCInst *MI, unsigned OpNum, raw_ostream &O);
  void printMemR0425(const MCInst *MI, unsigned OpNum, raw_ostream &O);
  void printAddrModeMemSrc(const MCInst *MI, unsigned OpNum, raw_ostream &O);
  void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O);
  void printMemOperand(const MCInst *MI, int opNum, raw_ostream &O);
};
} // end namespace llvm

#endif