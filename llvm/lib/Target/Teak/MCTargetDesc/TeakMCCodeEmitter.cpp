//===-- Teak/TeakMCCodeEmitter.cpp - Convert Teak code to machine code -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TeakMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "../Teak.h"
#include "../TeakMachineFunctionInfo.h"
#include "../TeakSubtarget.h"
#include "../TeakTargetMachine.h"
#include "MCTargetDesc/TeakMCTargetDesc.h"
#include "MCTargetDesc/TeakFixupKinds.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted.");

namespace {
class TeakMCCodeEmitter : public MCCodeEmitter {
  TeakMCCodeEmitter(const TeakMCCodeEmitter &) = delete;
  void operator=(const TeakMCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  const MCContext &CTX;

public:
  TeakMCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx)
      : MCII(mcii), CTX(ctx) {}

  ~TeakMCCodeEmitter() {}

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getMemSrcValue(const MCInst &MI, unsigned OpIdx,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;

  void EmitByte(unsigned char C, raw_ostream &OS) const { OS << (char)C; }

  void EmitConstant(uint64_t Val, unsigned Size, raw_ostream &OS) const {
    // Output the constant in little endian byte order.
    for (unsigned i = 0; i != Size; ++i) {
      EmitByte(Val & 255, OS);
      Val >>= 8;
    }
  }
  
  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;
};

} // end anonymous namespace

MCCodeEmitter *llvm::createTeakMCCodeEmitter(const MCInstrInfo &MCII,
                                            const MCRegisterInfo &MRI,
                                            MCContext &Ctx) {
  return new TeakMCCodeEmitter(MCII, Ctx);
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned TeakMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const
{
  return 0;
}

unsigned TeakMCCodeEmitter::getMemSrcValue(const MCInst &MI, unsigned OpIdx,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  unsigned Bits = 0;
//   const MCOperand &RegMO = MI.getOperand(OpIdx);
//   const MCOperand &ImmMO = MI.getOperand(OpIdx + 1);
//   assert(ImmMO.getImm() >= 0);
//   Bits |= (getMachineOpValue(MI, RegMO, Fixups, STI) << 12);
//   Bits |= (unsigned)ImmMO.getImm() & 0xfff;
  return Bits;
}

static unsigned encodeRegisterOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::R0: return 0;
        case Teak::R1: return 1;
        case Teak::R2: return 2;
        case Teak::R3: return 3;
        case Teak::R4: return 4;
        case Teak::R5: return 5;
        case Teak::R7: return 6;
        case Teak::Y0: return 7;
        //case Teak::ST0: return 8;
        //case Teak::ST1: return 9;
        //case Teak::ST2: return 0xA;
        //case Teak::P0H: return 0xB;
        case Teak::PC: return 0xC;
        case Teak::SP: return 0xD;
        //case Teak::CFGI: return 0xE;
        //case Teak::CFGJ: return 0xF;
        case Teak::B0H: return 0x10;
        case Teak::B1H: return 0x11;
        case Teak::B0L: return 0x12;
        case Teak::B1L: return 0x13;
        //case Teak::EXT0: return 0x14;
        //case Teak::EXT1: return 0x15;
        //case Teak::EXT2: return 0x16;
        //case Teak::EXT3: return 0x17;
        case Teak::A0: return 0x18;
        case Teak::A1: return 0x19;
        case Teak::A0L: return 0x1A;
        case Teak::A1L: return 0x1B;
        case Teak::A0H: return 0x1C;
        case Teak::A1H: return 0x1D;
        case Teak::LC: return 0x1E;
        case Teak::SV: return 0x1F;
        default:
            dbgs() << "encodeRegisterOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeRegisterP0Op(unsigned reg)
{
    switch (reg)
    {
        case Teak::R0: return 0;
        case Teak::R1: return 1;
        case Teak::R2: return 2;
        case Teak::R3: return 3;
        case Teak::R4: return 4;
        case Teak::R5: return 5;
        case Teak::R7: return 6;
        case Teak::Y0: return 7;
        //case Teak::ST0: return 8;
        //case Teak::ST1: return 9;
        //case Teak::ST2: return 0xA;
        case Teak::P0: return 0xB;
        case Teak::PC: return 0xC;
        case Teak::SP: return 0xD;
        //case Teak::CFGI: return 0xE;
        //case Teak::CFGJ: return 0xF;
        case Teak::B0H: return 0x10;
        case Teak::B1H: return 0x11;
        case Teak::B0L: return 0x12;
        case Teak::B1L: return 0x13;
        //case Teak::EXT0: return 0x14;
        //case Teak::EXT1: return 0x15;
        //case Teak::EXT2: return 0x16;
        //case Teak::EXT3: return 0x17;
        case Teak::A0: return 0x18;
        case Teak::A1: return 0x19;
        case Teak::A0L: return 0x1A;
        case Teak::A1L: return 0x1B;
        case Teak::A0H: return 0x1C;
        case Teak::A1H: return 0x1D;
        case Teak::LC: return 0x1E;
        case Teak::SV: return 0x1F;
        default:
            dbgs() << "encodeRegisterOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeAbOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::B0: return 0;
        case Teak::B1: return 1;
        case Teak::A0: return 2;
        case Teak::A1: return 3;
        default:
            dbgs() << "encodeAbOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeAxOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::A0: return 0;
        case Teak::A1: return 1;
        default:
            dbgs() << "encodeAxOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeAxlOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::A0L: return 0;
        case Teak::A1L: return 1;
        default:
            dbgs() << "encodeAxlOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeBxOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::B0: return 0;
        case Teak::B1: return 1;
        default:
            dbgs() << "encodeBxOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeRnOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::R0: return 0;
        case Teak::R1: return 1;
        case Teak::R2: return 2;
        case Teak::R3: return 3;
        case Teak::R4: return 4;
        case Teak::R5: return 5;
        case Teak::R6: return 6;
        case Teak::R7: return 7;
        default:
            dbgs() << "encodeRnOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

static unsigned encodeArArpSttModOp(unsigned reg)
{
    switch (reg)
    {
        case Teak::STT0: return 8;
        default:
            dbgs() << "encodeArArpSttModOp(" << reg << ")\n";
            llvm_unreachable("Unsupported register");
            break;
    }
}

void TeakMCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const
{
    const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
    //if (Desc.getSize() != 4) {
    //   llvm_unreachable("Unexpected instruction size!");
    //}
    //dbgs() << "op: " << MI.getOpcode() << "\n";
    switch(MI.getOpcode())
    {
        case Teak::ADD_ab_ab:
        {
            unsigned dstReg = MI.getOperand(0).getReg();
            unsigned reg = MI.getOperand(1).getReg();
            if(TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(dstReg))
                EmitConstant(0x86A0 | encodeRegisterOp(reg) | (encodeAxOp(dstReg) << 8), 2, OS);
            else if(TeakMCRegisterClasses[Teak::ABRegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(dstReg))
                EmitConstant(0xD2DA | (encodeAbOp(reg) << 10) | encodeBxOp(dstReg), 2, OS);
            else if(TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(dstReg))
                EmitConstant(0x5DF0 | (encodeBxOp(reg) << 1) | encodeAxOp(dstReg), 2, OS);
            else
                llvm_unreachable("Unsupported registers");
            break;
        }
        case Teak::AND_ab_ab_a:
            EmitConstant(0x6770
                | (encodeAbOp(MI.getOperand(1).getReg()) << 2)
                | (encodeAbOp(MI.getOperand(2).getReg()) << 0)
                | (encodeAxOp(MI.getOperand(0).getReg()) << 12), 2, OS);
            break;
        case Teak::ADD_imm16_a:
            EmitConstant(0x86C0 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            break; 
        case Teak::ADDV_imm16_RegNoBRegs16:
            EmitConstant(0x87E0 | encodeRegisterOp(MI.getOperand(0).getReg()), 2, OS);
            EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::ADDV_imm16_memrn:
            EmitConstant(0x86E0 | encodeRnOp(MI.getOperand(1).getReg()), 2, OS);
            EmitConstant(MI.getOperand(0).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::AND_imm16_a:
            EmitConstant(0x82C0 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::BR_imm18:
        case Teak::BRCond_imm18:
        {
            unsigned cond = MI.getOpcode() == Teak::BR_imm18 ? TeakCC::True : MI.getOperand(1).getImm();
            EmitConstant(0x4180 | cond, 2, OS);
            EmitConstant(0, 2, OS);
            Fixups.push_back(MCFixup::create(0, MI.getOperand(0).getExpr(), MCFixupKind(Teak::fixup_teak_call_imm18)));
            break;
        }
        case Teak::CALL_imm:
        {
            EmitConstant(0x41C0, 2, OS);
            EmitConstant(0, 2, OS);
            Fixups.push_back(MCFixup::create(0, MI.getOperand(0).getExpr(), MCFixupKind(Teak::fixup_teak_call_imm18)));
            break;
        }
        case Teak::CMP_ab_ab:
        {
            unsigned bReg = MI.getOperand(0).getReg();
            unsigned aReg = MI.getOperand(1).getReg();
            if(TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(bReg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(aReg))
                EmitConstant(0x8CA0 | encodeRegisterP0Op(bReg) | (encodeAxOp(aReg) << 8), 2, OS);
            else if(TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(bReg) && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(aReg))
                EmitConstant(0x4D8C | (encodeAxOp(bReg) << 1) | encodeBxOp(aReg), 2, OS);
            else if(bReg == Teak::B0 && aReg == Teak::B1)
                EmitConstant(0xD483, 2, OS);
            else if(bReg == Teak::B1 && aReg == Teak::B0)
                EmitConstant(0xD583, 2, OS);
            else if(TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(bReg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(aReg))
                EmitConstant(0xDA9A | (encodeBxOp(bReg) << 10) | encodeAxOp(aReg), 2, OS);
            else
                llvm_unreachable("Unsupported registers");
            break;
        }
        case Teak::CMP_imm16_a:
        {
            EmitConstant(0x8CC0 | (encodeAxOp(MI.getOperand(1).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(0).getImm() & 0xFFFF, 2, OS);            
            break;
        }
        case Teak::CMPV_imm16_RegNoBRegs16:
        {
            EmitConstant(0x8DE0 | encodeRegisterOp(MI.getOperand(1).getReg()), 2, OS);
            EmitConstant(MI.getOperand(0).getImm() & 0xFFFF, 2, OS);   
            break;
        }
        case Teak::MOV_imm16_regnob16:
        case Teak::MOV_imm16neg_ab:
        case Teak::MOV_imm16_ab:
        case Teak::MOV_imm16hi_ab:
        {
            unsigned reg = MI.getOperand(0).getReg();
            if(MI.getOpcode() == Teak::MOV_imm16_ab)
                reg = teakGetAbLReg(reg);
            else if(MI.getOpcode() == Teak::MOV_imm16hi_ab)
                reg = teakGetAbHReg(reg);
            if(TeakMCRegisterClasses[Teak::RegNoBRegs16RegClassID].contains(reg) || TeakMCRegisterClasses[Teak::RegNoBRegs40RegClassID].contains(reg))
                EmitConstant(0x5E00 | encodeRegisterOp(reg), 2, OS);
            else
                EmitConstant(0x5E20 | (encodeBxOp(reg) << 8), 2, OS);
            if(MI.getOperand(1).isImm() || MI.getOpcode() == Teak::MOV_imm16hi_ab)
                EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            else
            {
                EmitConstant(0, 2, OS);
                Fixups.push_back(MCFixup::create(0, MI.getOperand(1).getExpr(), MCFixupKind(Teak::fixup_teak_ptr_imm16)));
            }
            break;
        }
        case Teak::MOV_ab_ab:
            EmitConstant(0xD290 | (encodeAbOp(MI.getOperand(1).getReg()) << 10) | (encodeAbOp(MI.getOperand(0).getReg()) << 5), 2, OS);
            break;
        case Teak::MOV_regnobp016_regnob16:
        case Teak::MOV_regnobp016_ab:
        case Teak::MOV_regnobp016_abl:
        case Teak::MOV_p0_ab:
        case Teak::MOV_p0_regnob16:
        {
            unsigned dstReg = MI.getOperand(0).getReg();
            if(MI.getOpcode() == Teak::MOV_regnobp016_abl)
                dstReg = teakGetAbLReg(dstReg);
            EmitConstant(0x5800 | encodeRegisterP0Op(MI.getOperand(1).getReg()) | (encodeRegisterOp(dstReg) << 5), 2, OS);
            break;
        }
        case Teak::MOV_regnob16_memrn:
        case Teak::MOV_abl_memrn:
        {
            unsigned reg = MI.getOperand(0).getReg();
            if(MI.getOpcode() == Teak::MOV_abl_memrn)
                reg = teakGetAbLReg(reg);
            EmitConstant(0x1800 | encodeRnOp(MI.getOperand(1).getReg()) | (encodeRegisterOp(reg) << 5), 2, OS);
            break;
        }
        case Teak::MOV_memimm16_a:
        {
            EmitConstant(0xD4B8 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            if(MI.getOperand(1).isImm())
                EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            else
            {
                EmitConstant(0, 2, OS);
                Fixups.push_back(MCFixup::create(0, MI.getOperand(1).getExpr(), MCFixupKind(Teak::fixup_teak_ptr_imm16)));
            }
            break;
        }
        case Teak::MOV_memrn_regnob16:
        case Teak::MOV_memrn_ab:
        case Teak::MOV_memrn_ab1:
        {
            unsigned dstReg = MI.getOperand(0).getReg();
            if(MI.getOpcode() == Teak::MOV_memrn_ab1)
                dstReg = teakGetAbLReg(dstReg);
            EmitConstant(0x1C00 | encodeRnOp(MI.getOperand(1).getReg()) | (encodeRegisterOp(dstReg) << 5), 2, OS);
            break;
        }
        case Teak::MOV_r7offset16_a:
            EmitConstant(0xD498 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(2).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::MOV_a_r7offset16:
            EmitConstant(0xD49C | (encodeAxlOp(teakGetAbLReg(MI.getOperand(0).getReg())) << 8), 2, OS);
            EmitConstant(MI.getOperand(2).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::MOV_al_memimm16:
        case Teak::MOV_al2_memimm16:
        {
            unsigned reg = teakGetAbLReg(MI.getOperand(0).getReg());
            EmitConstant(0xD4BC | (encodeAxlOp(reg) << 8), 2, OS);
            if(MI.getOperand(1).isImm())
                EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            else
            {
                EmitConstant(0, 2, OS);
                Fixups.push_back(MCFixup::create(0, MI.getOperand(1).getExpr(), MCFixupKind(Teak::fixup_teak_ptr_imm16)));
            }
            break;
        }
        case Teak::MPY_y0_regnob16:
            EmitConstant(0x8040 | encodeRegisterOp(MI.getOperand(2).getReg()), 2, OS);
            break;
        case Teak::OR_ab_ab_a:
        {
            unsigned dstReg = MI.getOperand(0).getReg();
            unsigned bReg = MI.getOperand(1).getReg();
            unsigned aReg = MI.getOperand(2).getReg();
            if(TeakMCRegisterClasses[Teak::ABRegsRegClassID].contains(bReg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(aReg))
                EmitConstant(0xD291 | (encodeAbOp(bReg) << 10) | (encodeAxOp(aReg) << 6) | (encodeAxOp(dstReg) << 5), 2, OS);
            else if(TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(bReg) && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(aReg))
                EmitConstant(0xD4A4 | (encodeAxOp(bReg) << 8) | (encodeBxOp(aReg) << 1) | encodeAxOp(dstReg), 2, OS);
            else if(bReg == Teak::B0 && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(aReg))
                EmitConstant(0xD3C4 | (encodeBxOp(aReg) << 1) | encodeAxOp(dstReg), 2, OS);
            else if(bReg == Teak::B1 && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(aReg))
                EmitConstant(0xD7C4 | (encodeBxOp(aReg) << 1) | encodeAxOp(dstReg), 2, OS);
            else
                llvm_unreachable("Unsupported registers");
            break;
        }
        case Teak::OR_imm16_a:
            EmitConstant(0x80C0 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(1).getImm() & 0xFFFF, 2, OS);
            break;
        case Teak::OR_r7offset16_a:
            EmitConstant(0xD4D8 | (encodeAxOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            EmitConstant(MI.getOperand(2).getImm() & 0xFFFF, 2, OS);
            break;
        //todo: these three should be pseudo ops
        case Teak::SHFI_ab_ab:
        case Teak::SHFI2_ab_ab:
        case Teak::SHFI3_ab_ab:
            EmitConstant(0x9240
                | (encodeAbOp(MI.getOperand(1).getReg()) << 10)
                | (encodeAbOp(MI.getOperand(0).getReg()) << 7)
                | ((MI.getOperand(2).getImm() * (MI.getOpcode() == Teak::SHFI2_ab_ab ? 1 : -1)) & 0x7F), 2, OS);
            break;
        
        case Teak::SUB_ab_ab:
        {
            unsigned dstReg = MI.getOperand(0).getReg();
            unsigned reg = MI.getOperand(1).getReg();
            if(TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(dstReg))
                EmitConstant(0x8EA0 | encodeRegisterOp(reg) | (encodeAxOp(dstReg) << 8), 2, OS);
            else if(TeakMCRegisterClasses[Teak::ABRegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(dstReg))
                EmitConstant(0x8A61 | (encodeAbOp(reg) << 3) | (encodeBxOp(dstReg) << 8), 2, OS);
            else if(TeakMCRegisterClasses[Teak::BRegsRegClassID].contains(reg) && TeakMCRegisterClasses[Teak::ARegsRegClassID].contains(dstReg))
                EmitConstant(0x8861 | (encodeBxOp(reg) << 4) | (encodeAxOp(dstReg) << 3), 2, OS);
            else
                llvm_unreachable("Unsupported registers");
            break;
        }
        case Teak::PUSH_regnob16:
        {
            unsigned reg = MI.getOperand(0).getReg();
            if(TeakMCRegisterClasses[Teak::RegNoBRegs16RegClassID].contains(reg))
                EmitConstant(0x5E40 | encodeRegisterOp(reg), 2, OS);
            else
                llvm_unreachable("Unsupported register");
            break;
        }
        case Teak::PUSH_ararpsttmod:
            EmitConstant(0xD3D0 | encodeArArpSttModOp(MI.getOperand(0).getReg()), 2, OS);
            break;
        case Teak::POP_regnob16:
        {
            unsigned reg = MI.getOperand(0).getReg();
            if(TeakMCRegisterClasses[Teak::RegNoBRegs16RegClassID].contains(reg))
                EmitConstant(0x5E60 | encodeRegisterOp(reg), 2, OS);
            else
                llvm_unreachable("Unsupported register");
            break;
        }
        case Teak::POP_ararpsttmod:
            EmitConstant(0x80C7 | (encodeArArpSttModOp(MI.getOperand(0).getReg()) << 8), 2, OS);
            break;
        case Teak::RET:
            EmitConstant(0x4580, 2, OS);
            break;
        case Teak::RawAsmOp:
            EmitConstant(MI.getOperand(0).getImm(), 2, OS);
            break;
        case Teak::RawAsmOpExtended:
            EmitConstant(MI.getOperand(0).getImm(), 2, OS);
            EmitConstant(MI.getOperand(1).getImm(), 2, OS);
            break;
        default:
            dbgs() << "Unsupported opcode " << MI.getOpcode() << "!\n";
            llvm_unreachable("Unsupported opcode");
            EmitConstant(0, 2, OS);
            break;
    }
    ++MCNumEmitted;
}

#include "TeakGenMCCodeEmitter.inc"