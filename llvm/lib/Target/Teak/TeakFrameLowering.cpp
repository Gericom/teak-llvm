//===-- TeakFrameLowering.cpp - Frame info for Teak Target --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains Teak frame information that doesn't fit anywhere else
// cleanly...
//
//===----------------------------------------------------------------------===//

#include "TeakFrameLowering.h"
#include "Teak.h"
#include "TeakSubtarget.h"
#include "TeakInstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm> // std::sort

using namespace llvm;

//===----------------------------------------------------------------------===//
// TeakFrameLowering:
//===----------------------------------------------------------------------===//
TeakFrameLowering::TeakFrameLowering()
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(2), 0) {
  // Do nothing
}

bool TeakFrameLowering::hasFP(const MachineFunction &MF) const {
  return true;
}

uint64_t TeakFrameLowering::computeStackSize(MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  uint64_t StackSize = MFI.getStackSize();
  unsigned StackAlign = getStackAlignment();
  if (StackAlign > 0) {
    StackSize = alignTo(StackSize, StackAlign);
  }
  return StackSize;
}

// Materialize an offset for a ADD/SUB stack operation.
// Return zero if the offset fits into the instruction as an immediate,
// or the number of the register where the offset is materialized.
// static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB,
//                                   MachineBasicBlock::iterator MBBI,
//                                   unsigned Offset) {
//   const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
//   DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
//   const uint64_t MaxSubImm = 0xfff;
//   if (Offset <= MaxSubImm) {
//     // The stack offset fits in the ADD/SUB instruction.
//     return 0;
//   } else {
//     // The stack offset does not fit in the ADD/SUB instruction.
//     // Materialize the offset using MOVLO/MOVHI.
//     unsigned OffsetReg = Teak::R4;
//     unsigned OffsetLo = (unsigned)(Offset & 0xffff);
//     unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);
//     BuildMI(MBB, MBBI, dl, TII.get(Teak::MOVLOi16), OffsetReg)
//         .addImm(OffsetLo)
//         .setMIFlag(MachineInstr::FrameSetup);
//     if (OffsetHi) {
//       BuildMI(MBB, MBBI, dl, TII.get(Teak::MOVHIi16), OffsetReg)
//           .addReg(OffsetReg)
//           .addImm(OffsetHi)
//           .setMIFlag(MachineInstr::FrameSetup);
//     }
//     return OffsetReg;
//   }
// }

void TeakFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
	// Compute the stack size, to determine if we need a prologue at all.
	const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
	MachineBasicBlock::iterator MBBI = MBB.begin();
	DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
	uint64_t StackSize = computeStackSize(MF);
	if (!StackSize)
		return;

	MachineFrameInfo& MFI = MF.getFrameInfo();
	const std::vector<CalleeSavedInfo>& CSI = MFI.getCalleeSavedInfo();

	StackSize -= CSI.size() * 2;

	if (!StackSize)
		return;

	BuildMI(MBB, MBBI, dl, TII.get(Teak::PUSH_regnob16), Teak::R7)
		.setMIFlag(MachineInstr::FrameSetup);

	BuildMI(MBB, MBBI, dl, TII.get(Teak::MOV_regnobp016_regnob16), Teak::R7)
		.addReg(Teak::SP)
		.setMIFlag(MachineInstr::FrameSetup);

	BuildMI(MBB, MBBI, dl, TII.get(Teak::ADDV_imm16_RegNoBRegs16), Teak::SP)
		.addImm(-(StackSize >> 1))
		.addReg(Teak::SP)
		.setMIFlag(MachineInstr::FrameSetup);
}

void TeakFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const
{
    // Compute the stack size, to determine if we need an epilogue at all.
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
    DebugLoc dl = MBBI->getDebugLoc();
    uint64_t StackSize = computeStackSize(MF);
    if (!StackSize)
        return;

	MachineFrameInfo& MFI = MF.getFrameInfo();
	const std::vector<CalleeSavedInfo>& CSI = MFI.getCalleeSavedInfo();

	StackSize -= CSI.size() * 2;

	if (!StackSize)
		return;

    BuildMI(MBB, MBBI, dl, TII.get(Teak::MOV_regnobp016_regnob16), Teak::SP)
            .addReg(Teak::R7)
            .setMIFlag(MachineInstr::FrameSetup);
    BuildMI(MBB, MBBI, dl, TII.get(Teak::POP_regnob16), Teak::R7)
            .setMIFlag(MachineInstr::FrameSetup);
}

// This function eliminates ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo
// instructions
MachineBasicBlock::iterator TeakFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  // if (!hasReservedCallFrame(MF)) {
  //   MachineInstr &MI = *I;
  //   int Size = MI.getOperand(0).getImm();
  //   if (MI.getOpcode() == VE::ADJCALLSTACKDOWN)
  //     Size = -Size;

  //   if (Size)
  //     emitSPAdjustment(MF, MBB, I, Size);
  // }
  return MBB.erase(I);
}

bool TeakFrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    const std::vector<CalleeSavedInfo> &CSI, const TargetRegisterInfo *TRI) const
{
    if (CSI.empty())
        return false;

    DebugLoc DL = MBB.findDebugLoc(MI);
    MachineFunction &MF = *MBB.getParent();
    const TeakSubtarget &STI = MF.getSubtarget<TeakSubtarget>();
    const TargetInstrInfo &TII = *STI.getInstrInfo();

    for (unsigned i = 0; i < CSI.size(); i++)
    {
        unsigned Reg = CSI[i].getReg();
        bool IsNotLiveIn = !MBB.isLiveIn(Reg);

        // Add the callee-saved register as live-in only if it is not already a
        // live-in register, this usually happens with arguments that are passed
        // through callee-saved registers.
        if (IsNotLiveIn)
            MBB.addLiveIn(Reg);

        // Do not kill the register when it is an input argument.
        BuildMI(MBB, MI, DL, TII.get(Teak::PUSH_regnob16))
            .addReg(Reg, getKillRegState(IsNotLiveIn))
            .setMIFlag(MachineInstr::FrameSetup);
    }

    return true;
} 

bool TeakFrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    std::vector<CalleeSavedInfo> &CSI, const TargetRegisterInfo *TRI) const
{
    if (CSI.empty())
        return false;

    DebugLoc DL = MBB.findDebugLoc(MI);
    MachineFunction &MF = *MBB.getParent();
    const TeakSubtarget &STI = MF.getSubtarget<TeakSubtarget>();
    const TargetInstrInfo &TII = *STI.getInstrInfo();

    for (unsigned i = CSI.size(); i != 0; --i)
    {
        unsigned Reg = CSI[i - 1].getReg();

        BuildMI(MBB, MI, DL, TII.get(Teak::POP_regnob16), Reg);
    }

    return true;
}