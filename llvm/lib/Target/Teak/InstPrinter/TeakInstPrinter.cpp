//===-- TeakInstPrinter.cpp - Convert Teak MCInst to assembly syntax ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Teak MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "Teak.h"
#include "TeakInstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include "TeakGenAsmWriter.inc"

void TeakInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
  OS << StringRef(getRegisterName(RegNo)).lower();
}

void TeakInstPrinter::printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                 const MCSubtargetInfo &STI, raw_ostream &OS) {
  printInstruction(MI, Address, OS);
  printAnnotation(OS, Annot);
}

static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  } else {
    SRE = dyn_cast<MCSymbolRefExpr>(Expr);
    assert(SRE && "Unexpected MCExpr type.");
  }
  const MCSymbolRefExpr::VariantKind Kind = SRE->getKind();
  assert(Kind == MCSymbolRefExpr::VK_None);// ||
         //Kind == MCSymbolRefExpr::VK_Teak_LO ||
        // Kind == MCSymbolRefExpr::VK_Teak_HI);

  OS << SRE->getSymbol();

  if (Offset) {
    if (Offset > 0) {
      OS << '+';
    }
    OS << Offset;
  }
}

// Print a condition code (e.g. for predication).
void TeakInstPrinter::printCondCode(const MCInst *MI, unsigned OpNum, raw_ostream &O)
{
    O << TeakCCondCodeToString((TeakCC::CondCodes)MI->getOperand(OpNum).getImm());
}

// Print a 'memsrc' operand which is a (Register, Offset) pair.
void TeakInstPrinter::printAddrModeMemSrc(const MCInst *MI, unsigned OpNum, raw_ostream &O)
{
    const MCOperand &Op1 = MI->getOperand(OpNum);
    const MCOperand &Op2 = MI->getOperand(OpNum + 1);
    O << "[";
    printRegName(O, Op1.getReg());

    int Offset = Op2.getImm();
    if (Offset) 
    {
        if(Offset < 0)
        {
            O << "-0x";
            O.write_hex(-Offset);
        }
        else
        {
            O << "+0x";
            O.write_hex(Offset);
        }
    }
    O << "]";
}

void TeakInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O)
{
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isReg())
    {
        printRegName(O, Op.getReg());
        return;
    }

    if (Op.isImm())
    {
        O << "0x";
        O.write_hex((unsigned short)Op.getImm());
        return;
    }

    assert(Op.isExpr() && "unknown operand kind in printOperand");
    printExpr(Op.getExpr(), O);
}