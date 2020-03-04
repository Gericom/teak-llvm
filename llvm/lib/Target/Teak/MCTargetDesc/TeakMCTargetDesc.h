//===-- TeakMCTargetDesc.h - Teak Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Teak specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKMCTARGETDESC_H
#define TEAKMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class Target;
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCContext;
class MCCodeEmitter;
class MCAsmInfo;
class MCCodeGenInfo;
class MCInstPrinter;
class MCObjectWriter;
class MCAsmBackend;
class MCTargetOptions;
class StringRef;
class raw_ostream;
class MCObjectTargetWriter;
class raw_pwrite_stream;
class Triple;

extern Target TheTeakTarget;

MCCodeEmitter *createTeakMCCodeEmitter(const MCInstrInfo &MCII,
                                      const MCRegisterInfo &MRI,
                                      MCContext &Ctx);

MCAsmBackend *createTeakAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &TO);

std::unique_ptr<MCObjectTargetWriter> createTeakELFObjectWriter(uint8_t OSABI);

} // End llvm namespace

// Defines symbolic names for Teak registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "TeakGenRegisterInfo.inc"

// Defines symbolic names for the Teak instructions.
//
#define GET_INSTRINFO_ENUM
#include "TeakGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TeakGenSubtargetInfo.inc"

#endif