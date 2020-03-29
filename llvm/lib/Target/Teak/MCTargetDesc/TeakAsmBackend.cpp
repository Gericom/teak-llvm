//===-- TeakAsmBackend.cpp - Teak Assembler Backend -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TeakMCTargetDesc.h"
#include "MCTargetDesc/TeakFixupKinds.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
//#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
//#include "llvm/Support/MachO.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Endian.h"
using namespace llvm;
using namespace llvm::support::endian;

namespace {
class TeakELFObjectWriter : public MCELFObjectTargetWriter {
public:
  TeakELFObjectWriter(uint8_t OSABI)
      : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_TEAK,
                                /*HasRelocationAddend*/ false) {}
};

class TeakAsmBackend : public MCAsmBackend {
public:
  TeakAsmBackend(const Target &T, const StringRef TT) : MCAsmBackend(support::little) {}

  ~TeakAsmBackend() {}

  unsigned getNumFixupKinds() const override {
    return Teak::NumTargetFixupKinds;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[Teak::NumTargetFixupKinds] =
    {
        // This table *must* be in the order that the fixup_* kinds are defined in
        // TeakFixupKinds.h.
        //
        // Name                      Offset (bits) Size (bits)     Flags
        { "fixup_teak_call_imm18", 0, 18, 0 },
        { "fixup_teak_rel7", 0, 7, MCFixupKindInfo::FKF_IsPCRel },
        { "fixup_teak_ptr_imm16", 0, 16, 0 },
    };

    if (Kind < FirstTargetFixupKind)
        return MCAsmBackend::getFixupKindInfo(Kind);

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() && "Invalid kind!");
    return Infos[Kind - FirstTargetFixupKind];
  }

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override { return false; }

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
                        MCInst &Res) const override {}

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override {
    if (Count == 0) {
      return true;
    }
    return false;
  }

  unsigned getPointerSize() const { return 2; }
};
} // end anonymous namespace

// static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value, MCContext *Ctx = NULL)
// {
//     unsigned Kind = Fixup.getKind();
//     switch (Kind)
//     {
//         case R_TEAK_CALL_IMM18:
//             return 
//           write16le(loc, (read16le(loc) & ~0x30) | (((val >> 17) & 3) << 4));
//           write16le(loc + 2, (val >> 1) & 0xFFFF);
//           break;
//         case R_TEAK_PTR_IMM16:
//           write16le(loc + 2, (val >> 1) & 0xFFFF);
//           break;
//     }
//     return Value;
// }

void TeakAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved,
                               const MCSubtargetInfo *STI) const {
    unsigned Offset = Fixup.getOffset();
    unsigned Kind = Fixup.getKind();
    switch (Kind)
    {
        case Teak::fixup_teak_call_imm18:
            write16le(&Data[Offset], (read16le(&Data[Offset]) & ~0x30) | (((Value >> 16) & 3) << 4));
            write16le(&Data[Offset + 2], Value & 0xFFFF);
            break;
        case Teak::fixup_teak_rel7:
            write16le(&Data[Offset], (read16le(&Data[Offset]) & ~0x7F0) | ((((Value >> 1) - 1) & 0x7F) << 4));
            break;
        case Teak::fixup_teak_ptr_imm16:
            write16le(&Data[Offset + 2], Value);
            break;
    }
//   unsigned NumBytes = 4;
//   Value = adjustFixupValue(Fixup, Value);
//   if (!Value) {
//     return; // Doesn't change encoding.
//   }

//   unsigned Offset = Fixup.getOffset();
//   assert(Offset + NumBytes <= DataSize && "Invalid fixup offset!");

//   // For each byte of the fragment that the fixup touches, mask in the bits from
//   // the fixup value. The Value has been "split up" into the appropriate
//   // bitfields above.
//   for (unsigned i = 0; i != NumBytes; ++i) {
//     Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
//   }
}

namespace {

class ELFTeakAsmBackend : public TeakAsmBackend {
public:
  uint8_t OSABI;
  ELFTeakAsmBackend(const Target &T, const StringRef TT, uint8_t _OSABI)
      : TeakAsmBackend(T, TT), OSABI(_OSABI) {}

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override {
    return createTeakELFObjectWriter(OSABI);
  }
};

} // end anonymous namespace

MCAsmBackend *llvm::createTeakAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const llvm::MCTargetOptions &TO) {
  const uint8_t ABI = MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new ELFTeakAsmBackend(T, STI.getTargetTriple().getTriple(), ABI);
}