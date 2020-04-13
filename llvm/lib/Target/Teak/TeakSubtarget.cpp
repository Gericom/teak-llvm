//===-- TeakSubtarget.cpp - Teak Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Teak specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "TeakSubtarget.h"
#include "Teak.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "teak-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TeakGenSubtargetInfo.inc"

using namespace llvm;

void TeakSubtarget::anchor() {}

TeakSubtarget::TeakSubtarget(const Triple &TT, const std::string &CPU, const std::string &FS,
                           const TeakTargetMachine &TM)
    : TeakGenSubtargetInfo(TT, CPU, FS),
      DL("E-m:e-P1-p0:16:16:16-p1:32:32:32-i1:16:16-i8:16:16-i16:16:16-i32:32:32-a:0:32-n16:40"),
      InstrInfo(), TLInfo(TM), TSInfo(), FrameLowering() {}