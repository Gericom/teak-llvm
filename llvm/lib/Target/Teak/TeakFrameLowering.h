//===-- TeakFrameLowering.h - Frame info for Teak Target ------*- C++ -*-===//
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

#ifndef TEAKFRAMEINFO_H
#define TEAKFRAMEINFO_H

#include "llvm/CodeGen/TargetFrameLowering.h" 
#include "llvm/Target/TargetMachine.h"

namespace llvm
{
    class TeakSubtarget;

    class TeakFrameLowering : public TargetFrameLowering
    {
    public:
        TeakFrameLowering();

        /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
        /// the function.
        void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
        void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

        MachineBasicBlock::iterator eliminateCallFramePseudoInstr(MachineFunction &MF,
            MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const override;

        bool spillCalleeSavedRegisters(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
          	const std::vector<CalleeSavedInfo> &CSI, const TargetRegisterInfo *TRI) const override;
		
		bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
			std::vector<CalleeSavedInfo> &CSI, const TargetRegisterInfo *TRI) const override;

        bool hasFP(const MachineFunction &MF) const override;

        //! Stack slot size (4 bytes)
        static int stackSlotSize() { return 2; }

    private:
        uint64_t computeStackSize(MachineFunction &MF) const;
    };
}

#endif // TEAKFRAMEINFO_H
