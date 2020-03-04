//===-- TeakMCAsmInfo.cpp - Teak asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TeakMCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

void TeakMCAsmInfo::anchor() {}

TeakMCAsmInfo::TeakMCAsmInfo(const Triple &TT) {
  SupportsDebugInformation = true;
  Data16bitsDirective = "\t.short\t";
  Data32bitsDirective = "\t.long\t";
  Data64bitsDirective = 0;
  ZeroDirective = "\t.space\t";
  CommentString = "#";

  AscizDirective = ".asciiz";

  HiddenVisibilityAttr = MCSA_Invalid;
  HiddenDeclarationVisibilityAttr = MCSA_Invalid;
  ProtectedVisibilityAttr = MCSA_Invalid;
}
