//===-- Teak.h - Top-level interface for Teak representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// LEG back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_TEAK_H
#define TARGET_TEAK_H

#include "MCTargetDesc/TeakMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm 
{
    class TargetMachine;
    class TeakTargetMachine;

    FunctionPass *createTeakISelDag(TeakTargetMachine &TM, CodeGenOpt::Level OptLevel);

    namespace TeakCC
    {
        enum CondCodes
        {
            True = 0,
            Eq,
            Neq,
            Gt,
            Ge,
            Lt,
            Le,
            Nn,
            C,
            V,
            E,
            L,
            Nr,
            Niu0,
            Iu0,
            Iu1
        };
    }

    namespace TeakStepZIDS
    {
        enum Steps
        {
            Zero = 0,
            AddOne = 1,
            SubOne = 2,
            AddStep = 3
        };
    }

    inline static const char* TeakCCondCodeToString(TeakCC::CondCodes CC)
    {
        switch (CC)
        {
            case TeakCC::True:
                return "always";
            case TeakCC::Eq:
                return "eq";
            case TeakCC::Neq:
                return "neq";
            case TeakCC::Gt:
                return "gt";
            case TeakCC::Ge:
                return "ge";
            case TeakCC::Lt:
                return "lt";
            case TeakCC::Le:
                return "le";
            case TeakCC::Nn:
                return "mn";
            case TeakCC::C:
                return "c";
            case TeakCC::V:
                return "v";
            case TeakCC::E:
                return "e";
            case TeakCC::L:
                return "l";
            case TeakCC::Nr:
                return "nr";
            case TeakCC::Niu0:
                return "niu0";
            case TeakCC::Iu0:
                return "iu0";
            case TeakCC::Iu1:
                return "iu1";
            default:
                llvm_unreachable("Invalid cond code");
        }    
    }

    inline static unsigned teakGetAbLReg(unsigned reg)
    {
        switch(reg)
        {
            case Teak::A0: return Teak::A0L;
            case Teak::A0L: return Teak::A0L;
            case Teak::A1: return Teak::A1L;
            case Teak::A1L: return Teak::A1L;
            case Teak::B0: return Teak::B0L;
            case Teak::B0L: return Teak::B0L;
            case Teak::B1: return Teak::B1L;
            case Teak::B1L: return Teak::B1L;
            default:
                llvm_unreachable("Invalid reg");
        }
    }

    inline static unsigned teakGetAbHReg(unsigned reg)
    {
        switch(reg)
        {
            case Teak::A0: return Teak::A0H;
            case Teak::A0H: return Teak::A0H;
            case Teak::A1: return Teak::A1H;
            case Teak::A1H: return Teak::A1H;
            case Teak::B0: return Teak::B0H;
            case Teak::B0H: return Teak::B0H;
            case Teak::B1: return Teak::B1H;
            case Teak::B1H: return Teak::B1H;
            default:
                llvm_unreachable("Invalid reg");
        }
    }
} // end namespace llvm;

#endif