//===-- TeakRegisterInfo.cpp - Teak Register Information ----------------===//
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

#include "TeakRegisterInfo.h"
#include "Teak.h"
#include "TeakFrameLowering.h"
#include "TeakInstrInfo.h"
#include "TeakSubtarget.h"
#include "TeakMachineFunctionInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/CodeGen/TargetFrameLowering.h" 
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "TeakGenRegisterInfo.inc"

TeakRegisterInfo::TeakRegisterInfo() : TeakGenRegisterInfo(0) {}

const MCPhysReg* TeakRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
    return CSR_SaveList;
}

BitVector TeakRegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
    BitVector Reserved(getNumRegs());

    Reserved.set(Teak::SP);
    Reserved.set(Teak::PC);
    Reserved.set(Teak::R7);
    Reserved.set(Teak::LC);
    //Reserved.set(Teak::SV);
    return Reserved;
}

const uint32_t* TeakRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
    return CSR_RegMask;
}

const TargetRegisterClass* TeakRegisterInfo::getPointerRegClass(
    const MachineFunction &MF, unsigned Kind) const
{
    if(Kind == 1)
      return &Teak::ABRegsRegClass;
    return &Teak::GRRegsRegClass;
}

bool TeakRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const
{
    return true;
}

bool TeakRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const
{
    return true;
}

bool TeakRegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const
{
    return false;
}

bool TeakRegisterInfo::requiresFrameIndexScavenging(const MachineFunction &MF) const
{
    return false;//true;
}

bool TeakRegisterInfo::requiresVirtualBaseRegisters(const MachineFunction &MF) const
{
    return false;//true;
}

unsigned TeakRegisterInfo::getRegPressureLimit(const TargetRegisterClass *RC, MachineFunction &MF) const
{
    switch (RC->getID())
    {
        default:
            return 0;
        case Teak::GRRegsRegClassID:
            return 5;
        case Teak::SVRegRegClassID:
            return 0;
        case Teak::ABRegsRegClassID:
            return 4;
        case Teak::ALRegsRegClassID:
            return 2;
        case Teak::ABLRegsRegClassID:
            return 4;
        case Teak::RegNoBRegs16_nohRegClassID:
            return 9;
    }
}

void TeakRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
    dbgs() << "eliminateFrameIndex\n";
    MachineInstr &MI = *II;
    const MachineFunction &MF = *MI.getParent()->getParent();
    const MachineFrameInfo& MFI = MF.getFrameInfo();
    MachineOperand &FIOp = MI.getOperand(FIOperandNum);
    unsigned FI = FIOp.getIndex();

    // Determine if we can eliminate the index from this kind of instruction.
    unsigned ImmOpIdx = 0;
    switch (MI.getOpcode()) {
    default:
      // Not supported yet.
      return;
    case Teak::STORE_REG_TO_STACK_PSEUDO_16:
    case Teak::STORE_REG_TO_STACK_PSEUDO_TRUNC16:
    case Teak::STORE_REG_TO_STACK_PSEUDO_32:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40:
      ImmOpIdx = FIOperandNum + 1;
      break;
    }

    // FIXME: check the size of offset.
    MachineOperand &ImmOp = MI.getOperand(ImmOpIdx);
    int Offset = MFI.getObjectOffset(FI) + MFI.getStackSize() + ImmOp.getImm();
    FIOp.ChangeToRegister(Teak::R7, false);
    ImmOp.setImm(-(Offset >> 1) - 1);
}

Register TeakRegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
    return Teak::R7;
}

bool TeakRegisterInfo::isFrameOffsetLegal(
    const MachineInstr* 	MI, unsigned 	BaseReg, int64_t Offset) const
{
    dbgs() << "isFrameOffsetLegal\n";
    return true;//Offset == 0;
}

bool TeakRegisterInfo::needsFrameBaseReg(MachineInstr *MI, int64_t Offset) const
{ 
    return false;//Offset != 0;
}

int64_t TeakRegisterInfo::getFrameIndexInstrOffset(const MachineInstr* MI, int Idx) const
{
    switch (MI->getOpcode())
    {
        default:
            return 0;
        case Teak::STORE_REG_TO_STACK_PSEUDO_16:
        case Teak::STORE_REG_TO_STACK_PSEUDO_TRUNC16:
        case Teak::STORE_REG_TO_STACK_PSEUDO_32:
        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16:
        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40:
        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40:
        case Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40:
            return MI->getOperand(Idx + 1).getImm();
    }
}

void TeakRegisterInfo::materializeFrameBaseRegister(MachineBasicBlock *MBB,
                             unsigned BaseReg, int FrameIdx, int64_t Offset) const
{ 
    // dbgs() << "materializeFrameBaseRegister: " << BaseReg << "\n";
    // MachineBasicBlock::iterator Ins = MBB->begin();
    // DebugLoc DL;                  // Defaults to "unknown"
    // if (Ins != MBB->end())
    //   DL = Ins->getDebugLoc();

    // const MachineFunction &MF = *MBB->getParent();
    // MachineRegisterInfo &MRI = MBB->getParent()->getRegInfo();
    // const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    // MRI.constrainRegClass(BaseReg, &Teak::FPRegClass);

    // BuildMI(*MBB, Ins, DL, get(Teak::ADDV_imm16_RegNoBRegs16), BaseReg)
    //             .addImm(Offset)
    //             .addReg(Teak::R7);
}

void TeakRegisterInfo::resolveFrameIndex(MachineInstr &MI, unsigned BaseReg, int64_t Offset) const
{
    dbgs() << "resolveFrameIndex\n";
    MI.dump();
    const MachineFunction &MF = *MI.getParent()->getParent();
    const MachineFrameInfo& MFI = MF.getFrameInfo();
    MachineOperand* FIOp;
    //unsigned FI = FIOp.getIndex();

    // Determine if we can eliminate the index from this kind of instruction.
    unsigned ImmOpIdx = 0;
    switch (MI.getOpcode()) {
    default:
      // Not supported yet.
      return;
    case Teak::STORE_REG_TO_STACK_PSEUDO_16:
    case Teak::STORE_REG_TO_STACK_PSEUDO_TRUNC16:
    case Teak::STORE_REG_TO_STACK_PSEUDO_32:
      FIOp = &MI.getOperand(1);
      ImmOpIdx = 2;
      break;
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40:
    case Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40:
      FIOp = &MI.getOperand(1);
      ImmOpIdx = 2;
      break;
    }

    // FIXME: check the size of offset.
    MachineOperand &ImmOp = MI.getOperand(ImmOpIdx);
    FIOp->ChangeToRegister(BaseReg, false);
    ImmOp.setImm(Offset);
}

bool TeakRegisterInfo::shouldCoalesce(MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg, const TargetRegisterClass *NewRC, LiveIntervals &LIS) const
{
    dbgs() << "shouldCoalesce\n";
    MI->dump();
    dbgs() << "SrcRC: " << SrcRC->getID() << "\n";
    dbgs() << "SubReg: " << SubReg << "\n";
    dbgs() << "DstRC: " << DstRC->getID() << "\n";
    dbgs() << "DstSubReg: " << DstSubReg << "\n";
    dbgs() << "NewRC: " << NewRC->getID() << "\n";

    if (SrcRC->hasSuperClassEq(&Teak::SVRegRegClass) ||
        DstRC->hasSuperClassEq(&Teak::SVRegRegClass) ||
        NewRC->hasSuperClassEq(&Teak::SVRegRegClass))
        return false;

    if(DstSubReg == Teak::sub_16bit)
        return true;
    //if(SrcRC->hasSubClassEq(&Teak::GRRegsRegClass) && DstRC->hasSubClassEq(&Teak::GRRegsRegClass) && NewRC->hasSubClassEq(&Teak::GRRegsRegClass))
    //    return true;
    if(SrcRC->hasSuperClassEq(&Teak::ABLRegsRegClass) && DstRC->hasSuperClassEq(&Teak::ABRegsRegClass) && NewRC->hasSuperClassEq(&Teak::ABRegsRegClass))
        return true;

    if(SrcRC->hasSuperClassEq(&Teak::ABRegsRegClass) && DstRC->hasSuperClassEq(&Teak::ABRegsRegClass) && NewRC->hasSuperClassEq(&Teak::ABRegsRegClass))
        return true;
    //return true;//false;
    if(SrcRC->hasSuperClassEq(&Teak::ARegsRegClass) && DstRC->hasSuperClassEq(&Teak::ARegsRegClass) && NewRC->hasSuperClassEq(&Teak::ARegsRegClass))
        return true;
    if(SrcRC->hasSuperClassEq(&Teak::ALRegsRegClass) && DstRC->hasSuperClassEq(&Teak::ALRegsRegClass) && NewRC->hasSuperClassEq(&Teak::ALRegsRegClass))
        return true;

    if (DstRC->hasSuperClassEq(&Teak::ALRegsRegClass) || NewRC->hasSuperClassEq(&Teak::ALRegsRegClass) ||
        DstRC->hasSuperClassEq(&Teak::ARegsRegClass) || NewRC->hasSuperClassEq(&Teak::ARegsRegClass))
        return false;

    return true;
}