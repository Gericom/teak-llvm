//===-- TeakInstrInfo.h - Teak Instruction Information --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Teak implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKINSTRUCTIONINFO_H
#define TEAKINSTRUCTIONINFO_H

#include "TeakRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "TeakGenInstrInfo.inc"

namespace llvm {

class TeakInstrInfo : public TeakGenInstrInfo {
  const TeakRegisterInfo RI;
  virtual void anchor();

public:
  TeakInstrInfo();

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  const TeakRegisterInfo &getRegisterInfo() const { return RI; }

  /// isLoadFromStackSlot - If the specified machine instruction is a direct
  /// load from a stack slot, return the virtual or physical register number of
  /// the destination along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  // /// any side effects other than loading from the stack slot.
  // virtual unsigned isLoadFromStackSlot(const MachineInstr *MI,
  //                                      int &FrameIndex) const override;

  // /// isStoreToStackSlot - If the specified machine instruction is a direct
  // /// store to a stack slot, return the virtual or physical register number of
  // /// the source reg along with the FrameIndex of the loaded stack slot.  If
  // /// not, return 0.  This predicate must return 0 if the instruction has
  // /// any side effects other than storing to the stack slot.
  // virtual unsigned isStoreToStackSlot(const MachineInstr *MI,
  //                                     int &FrameIndex) const override;

  virtual bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify = false) const override; 

  virtual unsigned removeBranch(MachineBasicBlock &MBB, int *BytesRemoved = nullptr) const override;
  
  
  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;

  virtual bool reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

  virtual bool isBranchOffsetInRange(unsigned BranchOpc, int64_t BrOffset) const override;
  virtual MachineBasicBlock* getBranchDestBlock(const MachineInstr &MI) const override;
  virtual unsigned insertIndirectBranch(MachineBasicBlock &MBB, MachineBasicBlock &NewDestBB,
      const DebugLoc &DL, int64_t BrOffset = 0, RegScavenger *RS = nullptr) const override;
  virtual unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

  virtual void copyPhysReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator I, const DebugLoc &DL,
                           MCRegister DestReg, MCRegister SrcReg,
                           bool KillSrc) const override;

  virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MI,
                                   unsigned SrcReg, bool isKill, int FrameIndex,
                                   const TargetRegisterClass *RC,
                                   const TargetRegisterInfo *TRI) const
      override;

  virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MI,
                                    unsigned DestReg, int FrameIndex,
                                    const TargetRegisterClass *RC,
                                    const TargetRegisterInfo *TRI) const
      override;

  virtual bool isPredicated(const MachineInstr &MI) const override;
  virtual bool PredicateInstruction(MachineInstr &MI, ArrayRef<MachineOperand> Pred) const override;
  virtual bool isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles, unsigned ExtraPredCycles, BranchProbability Probability) const;
  virtual bool isProfitableToIfCvt(MachineBasicBlock &TBB, unsigned TCycles, unsigned TExtra, MachineBasicBlock &FBB, unsigned FCycles, unsigned FExtra, BranchProbability Probability) const;

  virtual bool expandPostRAPseudo(MachineInstr &MI) const
     override;
};
}

#endif