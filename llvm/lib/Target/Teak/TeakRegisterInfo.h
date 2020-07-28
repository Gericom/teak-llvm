//===-- TeakRegisterInfo.h - Teak Register Information Impl ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Teak implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKREGISTERINFO_H
#define TEAKREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "TeakGenRegisterInfo.inc"

namespace llvm {
struct TeakRegisterInfo : public TeakGenRegisterInfo {
public:
  TeakRegisterInfo();

  /// Code Generation virtual methods...
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const
      override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  const TargetRegisterClass* getLargestLegalSuperClass(
    const TargetRegisterClass* RC, const MachineFunction &MF) const override;

  const TargetRegisterClass* getPointerRegClass(
    const MachineFunction &MF, unsigned Kind) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override;

  bool trackLivenessAfterRegAlloc(const MachineFunction &MF) const override;

  bool useFPForScavengingIndex(const MachineFunction &MF) const override;

  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override;

  bool requiresVirtualBaseRegisters(const MachineFunction &MF) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  unsigned getRegPressureLimit(const TargetRegisterClass *RC, MachineFunction &MF) const override;

  // Debug information queries.
  Register getFrameRegister(const MachineFunction &MF) const override;

  virtual bool isFrameOffsetLegal(const MachineInstr* MI, unsigned BaseReg, int64_t Offset) const override;
  virtual bool needsFrameBaseReg(MachineInstr *MI, int64_t Offset) const override;
  virtual int64_t getFrameIndexInstrOffset(const MachineInstr* MI, int Idx) const override;
  virtual void materializeFrameBaseRegister(MachineBasicBlock *MBB, unsigned BaseReg,
      int FrameIdx, int64_t Offset) const override;
  virtual void resolveFrameIndex(MachineInstr &MI, unsigned BaseReg, int64_t Offset) const override;

  virtual bool shouldCoalesce(MachineInstr *MI,
                                     const TargetRegisterClass *SrcRC,
                                     unsigned SubReg,
                                     const TargetRegisterClass *DstRC,
                                     unsigned DstSubReg,
                                     const TargetRegisterClass *NewRC,
                                     LiveIntervals &LIS) const override;

  virtual const TargetRegisterClass* getCrossCopyRegClass(const TargetRegisterClass* RC) const override;
};

} // end namespace llvm

#endif