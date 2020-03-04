//===-- TeakSelectionDAGInfo.h - Teak SelectionDAG Info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Teak subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKSELECTIONDAGINFO_H
#define TEAKSELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class TeakSelectionDAGInfo : public SelectionDAGTargetInfo {
public:
  ~TeakSelectionDAGInfo();
};
}

#endif