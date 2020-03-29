//===-- TeakELFObjectWriter.cpp - Teak ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TeakMCTargetDesc.h"
#include "MCTargetDesc/TeakFixupKinds.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  class TeakELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    TeakELFObjectWriter(uint8_t OSABI);

    virtual ~TeakELFObjectWriter();

    unsigned getRelocType(MCContext &Ctx,
                        const MCValue &Target,
                        const MCFixup &Fixup,
                        bool IsPCRel) const override;
  };
}

unsigned TeakELFObjectWriter::getRelocType(MCContext &Ctx,
                                          const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
//   if (!IsPCRel) {
//     llvm_unreachable("Only dealying with PC-relative fixups for now");
//   }

  unsigned Type = 0;
  switch ((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("Unimplemented");
  case Teak::fixup_teak_call_imm18:
    Type = ELF::R_TEAK_CALL_IMM18;
    break;
  case Teak::fixup_teak_rel7:
    Type = ELF::R_TEAK_REL7;
    break;
  case Teak::fixup_teak_ptr_imm16:
    Type = ELF::R_TEAK_PTR_IMM16;
    break;
  }
  return Type;
}

TeakELFObjectWriter::TeakELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_TEAK,
                              /*HasRelocationAddend*/ false) {}

TeakELFObjectWriter::~TeakELFObjectWriter() {}

std::unique_ptr<MCObjectTargetWriter> llvm::createTeakELFObjectWriter(uint8_t OSABI) {
    return std::make_unique<TeakELFObjectWriter>(OSABI);
}