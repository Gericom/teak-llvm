//===-- TeakInstrInfo.cpp - Teak Instruction Information ----------------===//
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

#include "TeakInstrInfo.h"
#include "Teak.h"
#include "TeakMachineFunctionInfo.h"
//#include "MCTargetDesc/TeakBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "TeakSubtarget.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "TeakGenInstrInfo.inc"

using namespace llvm;

// Pin the vtable to this file.
void TeakInstrInfo::anchor() {}

TeakInstrInfo::TeakInstrInfo()
  : TeakGenInstrInfo(Teak::ADJCALLSTACKDOWN, Teak::ADJCALLSTACKUP),
    RI() {
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
// unsigned
// TeakInstrInfo::isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const{
//   assert(0 && "Unimplemented");
//   return 0;
// }
  
//   /// isStoreToStackSlot - If the specified machine instruction is a direct
//   /// store to a stack slot, return the virtual or physical register number of
//   /// the source reg along with the FrameIndex of the loaded stack slot.  If
//   /// not, return 0.  This predicate must return 0 if the instruction has
//   /// any side effects other than storing to the stack slot.
// unsigned
// TeakInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
//                                    int &FrameIndex) const {
//   assert(0 && "Unimplemented");
//   return 0;
// }

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//
//
/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with an conditional branch and it falls through to
///    an successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
/// 4. If this block ends with an conditional branch and an unconditional
///    block, it returns the 'true' destination in TBB, the 'false' destination
///    in FBB, and a list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success.
// ///
bool TeakInstrInfo::analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                            MachineBasicBlock *&FBB,
                            SmallVectorImpl<MachineOperand> &Cond,
                            bool AllowModify) const
{
    bool HasCondBranch = false;
    TBB = nullptr;
    FBB = nullptr;
    for (MachineInstr &MI : MBB)
    {        
        if(MI.getOpcode() == Teak::BRR_rel7)
        {
            MachineBasicBlock *TargetBB = MI.getOperand(0).getMBB();
            if (HasCondBranch)
                FBB = TargetBB;
            else
                TBB = TargetBB;
        }
        else if (MI.getOpcode() == Teak::BRRCond_rel7 || MI.getOpcode() == Teak::BR_imm18)
        {
            TBB = MI.getOperand(0).getMBB();
            Cond.push_back(MI.getOperand(1));
            //if(MI.getOpcode() == Teak::BRRCond_rel7)
            //    Cond.push_back(MachineOperand::CreateReg(Teak::ICC, false, true));
            //else
            //    Cond.push_back(MI.getOperand(2));
            HasCondBranch = true;
        }
        else if (MI.getOpcode() == Teak::RET)
            return true;
    }
    return false;
}

/// RemoveBranch - Remove the branching code at the end of the specific MBB.
/// This is only invoked in cases where AnalyzeBranch returns success. It
/// returns the number of instructions that were removed.
unsigned TeakInstrInfo::removeBranch(MachineBasicBlock &MBB, int *BytesRemoved) const
{
    if(BytesRemoved)
        *BytesRemoved = 0;
    if (MBB.empty())
        return 0;
    unsigned NumRemoved = 0;
    auto I = MBB.end();
    do
    {
        --I;
        unsigned Opc = I->getOpcode();
        if (Opc == Teak::BRR_rel7 || Opc == Teak::BRRCond_rel7 || Opc == Teak::BR_imm18)
        {
            auto ToDelete = I;
            if(BytesRemoved)
                *BytesRemoved += getInstSizeInBytes(*ToDelete);
            ++I;
            MBB.erase(ToDelete);
            NumRemoved++;
        }
    } while (I != MBB.begin());
    return NumRemoved;
}

/// InsertBranch - Insert branch code into the end of the specified
/// MachineBasicBlock.  The operands to this method are the same as those
/// returned by AnalyzeBranch.  This is only invoked in cases where
/// AnalyzeBranch returns success. It returns the number of instructions
/// inserted.
///
/// It is also invoked by tail merging to add unconditional branches in
/// cases where AnalyzeBranch doesn't apply because there was no original
/// branch to analyze.  At least this much must be implemented, else tail
/// merging needs to be disabled.
unsigned TeakInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *TBB,
                                    MachineBasicBlock *FBB,
                                    ArrayRef<MachineOperand> Cond,
                                    const DebugLoc &DL, int *BytesAdded) const
{
    unsigned NumInserted = 0;
    if(BytesAdded)
        *BytesAdded = 0;
    
    //Insert any conditional branch.
    if (Cond.size() > 0)
    {
        const MachineInstrBuilder& builder = BuildMI(MBB, MBB.end(), DL, get(Teak::BRRCond_rel7));
        //if(Cond[0].getImm() != TeakCC::True)
        //    builder.addUse(Teak::ICC, RegState::Implicit);
        builder.addMBB(TBB)
            .add(Cond[0]);
            //.add(Cond[1]);
            //.addReg(0);
        NumInserted++;
        if(BytesAdded)
            *BytesAdded += 1;
    }
    
    // Insert any unconditional branch.
    if (Cond.empty() || FBB)
    {
        BuildMI(MBB, MBB.end(), DL, get(Teak::BRR_rel7))
            .addMBB(Cond.empty() ? TBB : FBB);
           // .addImm(TeakCC::True)
            //.addReg(0);
        NumInserted++;
        if(BytesAdded)
            *BytesAdded += 1;
    }
    return NumInserted;
}

bool TeakInstrInfo::reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const
{
    if(!Cond[0].isImm())
        return true;
    TeakCC::CondCodes CC = static_cast<TeakCC::CondCodes>(Cond[0].getImm());
    switch(CC)
    {
        default:
            return true;
        case TeakCC::Eq:
            CC = TeakCC::Neq;
            break;
        case TeakCC::Neq:
            CC = TeakCC::Eq;
            break;
        case TeakCC::Gt:
            CC = TeakCC::Le;
            break;
        case TeakCC::Ge:
            CC = TeakCC::Lt;
            break;
        case TeakCC::Lt:
            CC = TeakCC::Ge;
            break;
        case TeakCC::Le:
            CC = TeakCC::Gt;
            break;
    }
    Cond[0].setImm(CC);
    return false;
}

bool TeakInstrInfo::isBranchOffsetInRange(unsigned BranchOpc, int64_t BrOffset) const
{
    switch (BranchOpc)
    {
        default:
            llvm_unreachable("unexpected opcode!");
        case Teak::BR_imm18:
        case Teak::CALL_imm18:
            return true;
        case Teak::BRR_rel7:
        case Teak::BRRCond_rel7:
        case Teak::CALLR_rel7:
            return BrOffset >= -63 && BrOffset <= 64;
    }
}
 
MachineBasicBlock* TeakInstrInfo::getBranchDestBlock(const MachineInstr &MI) const
{
    switch (MI.getOpcode())
    {
        default:
            llvm_unreachable("unexpected opcode!");
        case Teak::BR_imm18:
        case Teak::BRR_rel7:
        case Teak::BRRCond_rel7:
        case Teak::CALL_imm18:
        case Teak::CALLR_rel7:
            return MI.getOperand(0).getMBB();
    }
}

unsigned TeakInstrInfo::insertIndirectBranch(MachineBasicBlock &MBB, MachineBasicBlock &NewDestBB,
    const DebugLoc &DL, int64_t BrOffset, RegScavenger *RS) const
{
    auto& MI = *BuildMI(&MBB, DL, get(Teak::BR_imm18))
        .addMBB(&NewDestBB)
        .addImm(TeakCC::True)
        .addReg(0);
    return getInstSizeInBytes(MI);
}

unsigned TeakInstrInfo::getInstSizeInBytes(const MachineInstr &MI) const
{
    return get(MI.getOpcode()).getSize() >> 1;
}

void TeakInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I, const DebugLoc &DL,
                                 MCRegister DestReg, MCRegister SrcReg,
                                 bool KillSrc) const
{
    MachineFunction &MF = *MBB.getParent();
    const TeakSubtarget &st = MF.getSubtarget<TeakSubtarget>();
    const TargetRegisterInfo *RegInfo = st.getRegisterInfo();
    bool keepFlags = MBB.computeRegisterLiveness(RegInfo, Teak::ICC, I) != MachineBasicBlock::LQR_Dead;
    dbgs() << "copyPhysReg(" << SrcReg.id() << ", " << DestReg.id() << ")\n";
    unsigned op;
    // if(Teak::ABLRegsRegClass.contains(DestReg))
    // {
    //     // unsigned fullDst = teakGetAbReg(DestReg.id());
    //     // unsigned hiDst = teakGetAbHReg(fullDst);

    //     // //special handling for subreg copies, which cannot be done directly
    //     // // if(keepFlags)
    //     // //     BuildMI(MBB, I, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
    //     // // BuildMI(MBB, I, DL, get(Teak::RST_imm16_regnob16), DestReg)
    //     // //     .addImm(0xFFFF)
    //     // //     .addReg(DestReg);
    //     // // if(keepFlags)
    //     // //     BuildMI(MBB, I, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
    //     // bool hiAlive = MBB.computeRegisterLiveness(RegInfo, hiDst, I) != MachineBasicBlock::LQR_Dead;
    //     // dbgs() << "Hi dst alive: " << hiAlive << "\n";
    //     // if(hiAlive)
    //     // {
    //          llvm_unreachable("Invalid low destination register!");
    //         return;
    //     // }
    // }
    // else if(Teak::ABHRegsRegClass.contains(DestReg))
    // {
    //     llvm_unreachable("Invalid high destination register!");
    //     return;
    // }
    // if(Teak::ARegsRegClass.contains(SrcReg) && Teak::ARegsRegClass.contains(DestReg) && SrcReg != DestReg)
    //     op = Teak::COPY_a;
    // else
    if(Teak::ARegsRegClass.contains(SrcReg) && Teak::ARegsRegClass.contains(DestReg))
        op = Teak::COPY_a;
    else if(Teak::ABRegsRegClass.contains(SrcReg) && Teak::ABRegsRegClass.contains(DestReg))
        op = Teak::MOV_ab_ab;
    else if(Teak::RegNoBRegs16RegClass.contains(SrcReg) && Teak::RegNoBRegs40RegClass.contains(DestReg))
        op = Teak::MOV_regnobp016_abl;
    else if(Teak::RegNoBRegs16RegClass.contains(SrcReg) && Teak::RegNoBRegs16RegClass.contains(DestReg))
        op = Teak::MOV_regnobp016_regnob16;
    // else if(Teak::P0RegsRegClass.contains(SrcReg) && Teak::ABRegsRegClass.contains(DestReg))
    //   op = Teak::MOV_p0_ab; //with sign extension, might be bad?
    // else if(Teak::P0RegsRegClass.contains(SrcReg) && Teak::RegNoBRegs16RegClass.contains(DestReg))
    //   op = Teak::MOV_p0_regnob16;
    else
        assert(0 && "Unimplemented copyPhysReg");

    
    if (!keepFlags)
    {
        if (op == Teak::COPY_a)
            BuildMI(MBB, I, DL, get(op), DestReg)
                .addReg(SrcReg, getKillRegState(KillSrc))
                .addImm(TeakCC::True)
                ->addRegisterDead(Teak::ICC, RegInfo);
        else
            BuildMI(MBB, I, DL, get(op), DestReg)
                .addReg(SrcReg, getKillRegState(KillSrc))
                ->addRegisterDead(Teak::ICC, RegInfo);
    }
    else
    {
        BuildMI(MBB, I, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
        if (op == Teak::COPY_a)
            BuildMI(MBB, I, DL, get(op), DestReg)
                .addReg(SrcReg, getKillRegState(KillSrc))
                .addImm(TeakCC::True);
        else
            BuildMI(MBB, I, DL, get(op), DestReg)
                .addReg(SrcReg, getKillRegState(KillSrc));
        BuildMI(MBB, I, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
        //llvm_unreachable("Help! Can't preserve ICC");
    }
}

void TeakInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I,  unsigned SrcReg, bool isKill,
    int FrameIndex, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const
{
    dbgs() << "storeRegToStackSlot\n";
    dbgs() << "SrcReg: " << SrcReg << "\n";
    dbgs() << "TargetRegisterClass: " << RC->getID() << "\n";
    dbgs() << "ICC live: " << MBB.computeRegisterLiveness(TRI, Teak::ICC, I) << "\n";
    DebugLoc DL;
    if (I != MBB.end())
        DL = I->getDebugLoc();
    if(RC->hasSuperClassEq(&Teak::ABRegsRegClass))
        BuildMI(MBB, I, DL, get(Teak::STORE_REG_TO_STACK_PSEUDO_32))
            .addReg(SrcReg, getKillRegState(isKill))
            .addFrameIndex(FrameIndex).addImm(0);
    else
        BuildMI(MBB, I, DL, get(Teak::STORE_REG_TO_STACK_PSEUDO_16))
        .addReg(SrcReg, getKillRegState(isKill))
        .addFrameIndex(FrameIndex).addImm(0);    

    //dbgs() << "SrcReg: " << SrcReg << "\n";
    //assert(0 && "Unimplemented storeRegToStackSlot");
    //dbgs() << "storeRegToStackSlot\n";
    //todo: use pseudo instruction and expand to complex trickery
    //if(Teak::ABRegsRegClass.contains(SrcReg))
    {
       // assert(0 && "Unimplemented storeRegToStackSlot");
        // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::MOV_a_r7offset16))
        //     .addReg(SrcReg, getKillRegState(isKill))
        //     .addFrameIndex(FrameIndex).addImm(0);
        // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::ADDV_imm16_RegNoBRegs16))
        //     .addReg(Teak::R7)
        //     .addImm(1);
        // unsigned hiReg;
        // if(SrcReg == Teak::A0)
        //     hiReg = Teal::A0H;
        // else if(SrcReg == Teak::A1)
        //     hiReg = Teal::A1H;
        // else if(SrcReg == Teak::B0)
        //     hiReg = Teal::A1H;
        // else if(SrcReg == Teak::B1)
        //     hiReg = Teal::A1H;
        // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::MOV_regnob16_memrn))
        //     .addReg(SrcReg)
        //     .addReg(Teak::R7)
        //     .addImm(1);
    }
    //else
    {
        // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::STORE_REG_TO_STACK_PSEUDO_32))
        //     .addReg(SrcReg, getKillRegState(isKill))
        //     .addFrameIndex(FrameIndex).addImm(0);
        // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::MOV_a_r7offset16))
        //     .addReg(SrcReg, getKillRegState(isKill))
        //     .addFrameIndex(FrameIndex).addImm(0);
    }
}

void TeakInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I,unsigned DestReg, int FrameIndex,
    const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const
{
    dbgs() << "loadRegFromStackSlot\n";
    dbgs() << "ICC live: " << MBB.computeRegisterLiveness(TRI, Teak::ICC, I) << "\n";
    DebugLoc DL;
    if (I != MBB.end())
        DL = I->getDebugLoc();
    if(RC->hasSuperClassEq(&Teak::ABRegsRegClass))
        BuildMI(MBB, I, DL, get(Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40), DestReg)
        .addFrameIndex(FrameIndex).addImm(0);
    else
        BuildMI(MBB, I, DL, get(Teak::LOAD_REG_FROM_STACK_PSEUDO_16), DestReg)
          .addFrameIndex(FrameIndex).addImm(0);
    
    //dbgs() << "DestReg: " << DestReg << "\n";
    //assert(0 && "Unimplemented loadRegFromStackSlot");
    //todo: use pseudo instruction and expand to complex trickery
   // if(Teak::ABRegsRegClass.contains(DestReg))
     //   assert(0 && "Unimplemented loadRegFromStackSlot");
    // BuildMI(MBB, I, I->getDebugLoc(), get(Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40), DestReg)
    //     .addFrameIndex(FrameIndex).addImm(0);
}

bool TeakInstrInfo::isPredicated(const MachineInstr &MI) const
{
    if (MI.isBundle())
    {
        MachineBasicBlock::const_instr_iterator I = MI.getIterator();
        MachineBasicBlock::const_instr_iterator E = MI.getParent()->instr_end();
        while (++I != E && I->isInsideBundle())
        {
            int PIdx = I->findFirstPredOperandIdx();
            if (PIdx != -1 && I->getOperand(PIdx).getImm() != TeakCC::True)
              return true;
        }
        return false;
    }

    int PIdx = MI.findFirstPredOperandIdx();
    return PIdx != -1 && MI.getOperand(PIdx).getImm() != TeakCC::True;
}

bool TeakInstrInfo::PredicateInstruction(MachineInstr &MI, ArrayRef<MachineOperand> Pred) const
{
    dbgs() << "PredicateInstruction\n";
    unsigned Opc = MI.getOpcode();

    int PIdx = MI.findFirstPredOperandIdx();
    if (PIdx != -1)
    {
        MachineOperand &PMO = MI.getOperand(PIdx);
        PMO.setImm(Pred[0].getImm());
        if(Pred[0].getImm() == TeakCC::True)
            MI.getOperand(PIdx + 1).ChangeToRegister(0, false);
        else
            MI.getOperand(PIdx + 1).ChangeToRegister(Teak::ICC, false, true);
        //MI.getOperand(PIdx+1).setReg(Pred[1].getReg());
        return true;
    }
    return false;
}

bool TeakInstrInfo::isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles, unsigned ExtraPredCycles, BranchProbability Probability) const
{
    dbgs() << "isProfitableToIfCvt\n";
    return true;
}

bool TeakInstrInfo::isProfitableToIfCvt(MachineBasicBlock &TBB, unsigned TCycles, unsigned TExtra,
    MachineBasicBlock &FBB, unsigned FCycles, unsigned FExtra, BranchProbability Probability) const
{
    dbgs() << "isProfitableToIfCvt TF\n";
    return true;
}

void TeakInstrInfo::makeRnRegDisplacement(MachineBasicBlock& mbb, MachineInstr& mi, const DebugLoc& dl, Register reg, signed short offset) const
{
    if(offset == 0)
        return;
    
    if(offset == 1)
        BuildMI(mbb, mi, dl, get(Teak::MODR_inc1), reg)
            .addReg(reg);
    else if(offset == -1)
        BuildMI(mbb, mi, dl, get(Teak::MODR_dec1), reg)
            .addReg(reg);
    else if(offset == 2)
        BuildMI(mbb, mi, dl, get(Teak::MODR_inc2), reg)
            .addReg(reg);
    else if(offset == -2)
        BuildMI(mbb, mi, dl, get(Teak::MODR_dec2), reg)
            .addReg(reg);
    else
        BuildMI(mbb, mi, dl, get(Teak::ADDV_imm16_RegNoBRegs16), reg)
            .addImm(offset)
            .addReg(reg);
}

bool TeakInstrInfo::expandPostRAPseudo(MachineInstr &MI) const
{
    DebugLoc DL = MI.getDebugLoc();
    MachineBasicBlock &MBB = *MI.getParent();
    MachineFunction &MF = *MBB.getParent();
    const TeakSubtarget &st = MF.getSubtarget<TeakSubtarget>();
    const TargetRegisterInfo *RegInfo = st.getRegisterInfo();
    dbgs() << "Expand post, live: " << MBB.computeRegisterLiveness(RegInfo, Teak::ICC, MI) << "\n";
    bool keepFlags = MBB.computeRegisterLiveness(RegInfo, Teak::ICC, MI) != MachineBasicBlock::LQR_Dead;
    switch (MI.getOpcode())
    {
        default:
            return false;
        case Teak::STORE_REG_TO_STACK_PSEUDO_16:
        {
            dbgs() << "expandPostRAPseudo for STORE_REG_TO_STACK_PSEUDO_16\n";
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags && frameOffset)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
            BuildMI(MBB, MI, DL, get(Teak::MOV_regnob16_memrn))
                .addReg(MI.getOperand(0).getReg(), getKillRegState(MI.getOperand(0).isKill()))
                .addReg(MI.getOperand(1).getReg());
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
            if(keepFlags && frameOffset)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }

        case Teak::STORE_REG_TO_STACK_PSEUDO_TRUNC16:
        {
            dbgs() << "expandPostRAPseudo for STORE_REG_TO_STACK_PSEUDO_TRUNC16\n";
            unsigned srcReg = MI.getOperand(0).getReg();
            unsigned loReg  = teakGetAbLReg(srcReg);
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags && frameOffset)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
            BuildMI(MBB, MI, DL, get(Teak::MOV_regnob16_memrn))
                .addReg(loReg, getKillRegState(MI.getOperand(0).isKill()))
                .addReg(MI.getOperand(1).getReg());
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
            if(keepFlags && frameOffset)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }     
        
        case Teak::STORE_REG_TO_STACK_PSEUDO_32:
        {
            dbgs() << "expandPostRAPseudo for STORE_REG_TO_STACK_PSEUDO_32\n";
            unsigned srcReg = MI.getOperand(0).getReg();
            unsigned loReg = teakGetAbLReg(srcReg);
            unsigned hiReg = teakGetAbHReg(srcReg);
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
            if(loReg == Teak::A0L || loReg == Teak::A1L)
            {
                BuildMI(MBB, MI, DL, get(Teak::MOV_regnob16_memrn))
                    .addReg(hiReg, getKillRegState(MI.getOperand(0).isKill()))
                    .addReg(MI.getOperand(1).getReg());
                BuildMI(MBB, MI, DL, get(Teak::MOV_al_r7offset7s))
                        .addReg(loReg, getKillRegState(MI.getOperand(0).isKill()))
                        .addReg(Teak::R7)
                        .addImm(-1);
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
            }
            else
            {
                BuildMI(MBB, MI, DL, get(Teak::MOV_regnob16_memrn_postdec), MI.getOperand(1).getReg())
                    .addReg(hiReg, getKillRegState(MI.getOperand(0).isKill()))
                    .addReg(MI.getOperand(1).getReg());
                BuildMI(MBB, MI, DL, get(Teak::MOV_regnob16_memrn))
                    .addReg(loReg, getKillRegState(MI.getOperand(0).isKill()))
                    .addReg(MI.getOperand(1).getReg());
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset + 1);
            }
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }

        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16:
        {
            dbgs() << "expandPostRAPseudo for LOAD_REG_FROM_STACK_PSEUDO_16\n";
            unsigned dstReg = MI.getOperand(0).getReg();
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            if(dstReg == Teak::A0L || dstReg == Teak::A1L)
            {
                if(frameOffset >= -64 && frameOffset <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset7s_a), dstReg == Teak::A0L ? Teak::A0 : Teak::A1)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
                else
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset16_a), dstReg == Teak::A0L ? Teak::A0 : Teak::A1)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
            }
            else
            {
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
                BuildMI(MBB, MI, DL, get(Teak::MOV_memrn_regnob16), dstReg)
                    .addReg(MI.getOperand(1).getReg());
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
            }
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }

        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40:
        {
            dbgs() << "expandPostRAPseudo for LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40\n";
            unsigned dstReg = MI.getOperand(0).getReg();
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            if(dstReg == Teak::A0 || dstReg == Teak::A1)
            {
                if(frameOffset >= -64 && frameOffset <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset7s_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);   
                else
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset16_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);                
            }
            else
            {
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
                BuildMI(MBB, MI, DL, get(Teak::MOV_memrn_ab), dstReg)
                    .addReg(MI.getOperand(1).getReg());
                makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
                if(keepFlags)
                    BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            }
            MBB.erase(MI);
            return true;
        }

        case Teak::LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40:
        {
            dbgs() << "expandPostRAPseudo for LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40\n";
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), frameOffset);
            BuildMI(MBB, MI, DL, get(Teak::MOV_memrn_ab1), MI.getOperand(0).getReg())
                .addReg(MI.getOperand(1).getReg());
            makeRnRegDisplacement(MBB, MI, DL, MI.getOperand(1).getReg(), -frameOffset);
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }
        case Teak::LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40:
        {
            dbgs() << "expandPostRAPseudo for LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40\n";
            unsigned dstReg = MI.getOperand(0).getReg();
            signed short frameOffset = (signed short)MI.getOperand(2).getImm();
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::PUSH_ararpsttmod), Teak::STT0);
            if(dstReg == Teak::A0 || dstReg == Teak::A1)
            {
                if(frameOffset >= -64 && frameOffset <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset7s_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
                else
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset16_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
                BuildMI(MBB, MI, DL, get(Teak::SHFI_arith_ab_ab), dstReg)
                    .addReg(dstReg)
                    .addImm(16);
                if(frameOffset - 1 >= -64 && frameOffset - 1 <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::OR_r7offset7s_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset - 1)
                        .addReg(dstReg);
                else
                    BuildMI(MBB, MI, DL, get(Teak::OR_r7offset16_a), dstReg)
                        .addReg(Teak::R7)
                        .addImm(frameOffset - 1)
                        .addReg(dstReg);
            }
            else
            {
                BuildMI(MBB, MI, DL, get(Teak::MOV_ab_ab), dstReg)
                    .addReg(Teak::A0);
                if(frameOffset >= -64 && frameOffset <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset7s_a), Teak::A0)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
                else
                    BuildMI(MBB, MI, DL, get(Teak::MOV_r7offset16_a), Teak::A0)
                        .addReg(Teak::R7)
                        .addImm(frameOffset);
                BuildMI(MBB, MI, DL, get(Teak::SHFI_arith_ab_ab), Teak::A0)
                    .addReg(Teak::A0)
                    .addImm(16);
                if(frameOffset - 1 >= -64 && frameOffset - 1 <= 63)
                    BuildMI(MBB, MI, DL, get(Teak::OR_r7offset7s_a), Teak::A0)
                        .addReg(Teak::R7)
                        .addImm(frameOffset - 1)
                        .addReg(Teak::A0);
                else
                    BuildMI(MBB, MI, DL, get(Teak::OR_r7offset16_a), Teak::A0)
                        .addReg(Teak::R7)
                        .addImm(frameOffset - 1)
                        .addReg(Teak::A0);
                BuildMI(MBB, MI, DL, get(Teak::SWAP_ab))
                    .addReg(dstReg, RegState::Define)
                    .addReg(Teak::A0, RegState::Define)
                    .addReg(Teak::A0)
                    .addReg(dstReg);
            }
            if(keepFlags)
                BuildMI(MBB, MI, DL, get(Teak::POP_ararpsttmod), Teak::STT0);
            MBB.erase(MI);
            return true;
        }
        case Teak::MPYI_y0_imm8s_ab:
        case Teak::MPYI_y0_imm8s_ab_ADD:
        {
            BuildMI(MBB, MI, DL, get(Teak::MPYI_y0_imm8s), Teak::P0)
                .addReg(MI.getOperand(1).getReg())
                .addImm(MI.getOperand(2).getImm());
            if (MI.getOpcode() == Teak::MPYI_y0_imm8s_ab_ADD)
                BuildMI(MBB, MI, DL, get(Teak::ADD_px_ab), MI.getOperand(0).getReg())
                    .addReg(Teak::P0)
                    .addReg(MI.getOperand(0).getReg());
            else
                BuildMI(MBB, MI, DL, get(Teak::MOV_p0_ab), MI.getOperand(0).getReg())
                    .addReg(Teak::P0);
            MBB.erase(MI);
            return true;
        }
        case Teak::MPY_y0_regnob16_ab:
        case Teak::MPY_y0_regnob16_ab_ADD:
        {
            BuildMI(MBB, MI, DL, get(Teak::MPY_y0_regnob16), Teak::P0)
                .addReg(MI.getOperand(1).getReg())
                .addReg(MI.getOperand(2).getReg());

            if (MI.getOpcode() == Teak::MPY_y0_regnob16_ab_ADD)
                BuildMI(MBB, MI, DL, get(Teak::ADD_px_ab), MI.getOperand(0).getReg())
                    .addReg(Teak::P0)
                    .addReg(MI.getOperand(0).getReg());
            else
                BuildMI(MBB, MI, DL, get(Teak::MOV_p0_ab), MI.getOperand(0).getReg())
                    .addReg(Teak::P0);
            MBB.erase(MI);
            return true;
        }
        //   case Teak::MPY_y0_regnob16_RegNoBRegs16:
        //   {
        //       BuildMI(MBB, MI, DL, get(Teak::MPY_y0_regnob16), Teak::P0)
        //           .addReg(MI.getOperand(1).getReg())
        //           .addReg(MI.getOperand(2).getReg());
        //       BuildMI(MBB, MI, DL, get(Teak::MOV_p0_regnob16), MI.getOperand(0).getReg())
        //           .addReg(Teak::P0);
        //       MBB.erase(MI);
        //       return true;
        //   }
        // case Teak::SHL_PSEUDO_ab_ab_sv:
        // {
        //     BuildMI(MBB, MI, DL, get(Teak::MOV_regnobp016_regnob16), Teak::SV)
        //         .addReg(MI.getOperand(2).getReg());
        //     BuildMI(MBB, MI, DL, get(Teak::SHFC_ab_ab_sv), MI.getOperand(0).getReg())
        //         .addReg(MI.getOperand(1).getReg());
        //         .addReg(MI.getOperand(2).getReg());
        //     MBB.erase(MI);
        //     return true;
        // }
        case Teak::SHFI_logic_ab_ab:
        {
            BuildMI(MBB, MI, DL, get(Teak::SET_imm16_sttmod), Teak::MOD0)
                .addImm(0x80)
                .addReg(Teak::MOD0);
            BuildMI(MBB, MI, DL, get(Teak::SHFI_arith_ab_ab), MI.getOperand(0).getReg())
                .add(MI.getOperand(1))
                .add(MI.getOperand(2));
            BuildMI(MBB, MI, DL, get(Teak::RST_imm16_sttmod), Teak::MOD0)
                .addImm(0x80)
                .addReg(Teak::MOD0);
            MBB.erase(MI);
            return true;
        }
        case Teak::SHFC_logic_ab_ab_sv:
        {
            BuildMI(MBB, MI, DL, get(Teak::SET_imm16_sttmod), Teak::MOD0)
                .addImm(0x80)
                .addReg(Teak::MOD0);
            BuildMI(MBB, MI, DL, get(Teak::SHFC_arith_ab_ab_sv), MI.getOperand(0).getReg())
                .add(MI.getOperand(1))
                .add(MI.getOperand(2))
                .add(MI.getOperand(3));
            BuildMI(MBB, MI, DL, get(Teak::RST_imm16_sttmod), Teak::MOD0)
                .addImm(0x80)
                .addReg(Teak::MOD0);
            MBB.erase(MI);
            return true;
        }
        // case Teak::SRA_PSEUDO_ab_ab_sv:
        // {
        //     BuildMI(MBB, MI, DL, get(Teak::MOV_regnobp016_regnob16), Teak::SV)
        //         .addReg(MI.getOperand(2).getReg());
        //     BuildMI(MBB, MI, DL, get(Teak::SHFC_ab_ab_sv), MI.getOperand(0).getReg())
        //         .addReg(MI.getOperand(1).getReg());
        //         .addReg(MI.getOperand(2).getReg());
        //     MBB.erase(MI);
        //     return true;
        // }
    }
    // switch (MI->getOpcode())
    // {
    // default:
    //   return false;
    // case Teak::MOVi32: {
    //   DebugLoc DL = MI->getDebugLoc();
    //   MachineBasicBlock &MBB = *MI->getParent();

    //   const unsigned DstReg = MI->getOperand(0).getReg();
    //   const bool DstIsDead = MI->getOperand(0).isDead();

    //   const MachineOperand &MO = MI->getOperand(1);

    //   auto LO16 = BuildMI(MBB, MI, DL, get(Teak::MOV_imm16_b), DstReg);
    //   auto HI16 = BuildMI(MBB, MI, DL, get(Teak::MOVHIi16))
    //                   .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
    //                   .addReg(DstReg);

    //   if (MO.isImm()) {
    //     const unsigned Imm = MO.getImm();
    //     const unsigned Lo16 = Imm & 0xffff;
    //     const unsigned Hi16 = (Imm >> 16) & 0xffff;
    //     LO16 = LO16.addImm(Lo16);
    //     HI16 = HI16.addImm(Hi16);
    //   } else {
    //     const GlobalValue *GV = MO.getGlobal();
    //     const unsigned TF = MO.getTargetFlags();
    //     LO16 = LO16.addGlobalAddress(GV, MO.getOffset(), TF | TeakII::MO_LO16);
    //     HI16 = HI16.addGlobalAddress(GV, MO.getOffset(), TF | TeakII::MO_HI16);
    //   }

    //   MBB.erase(MI);
    //   return true;
    // }
    // }
}