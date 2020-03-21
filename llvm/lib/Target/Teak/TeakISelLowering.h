//===-- TeakISelLowering.h - Teak DAG Lowering Interface ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Teak uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef TEAKISELLOWERING_H
#define TEAKISELLOWERING_H

#include "Teak.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

// Forward delcarations
class TeakSubtarget;
class TeakTargetMachine;

namespace TeakISD {
enum NodeType {
  // Start the numbering where the builtin ops and target ops leave off.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  // This loads the symbol (e.g. global address) into a register.
  LOAD_SYM,
  // This loads a 32-bit immediate into a register.
  MOVEi32,
  CALL,
  CMPICC,
  BRICC,
  SELECT_ICC,
  WRAPPER,
  SHIFT_ARITH,
  SHIFT_LOGIC,
  AND,
  OR,
  XOR
};
}

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//
class TeakTargetLowering : public TargetLowering {
public:
  explicit TeakTargetLowering(const TeakTargetMachine &TM);

  bool isNarrowingProfitable(EVT VT1, EVT VT2) const override;

  bool getPostIndexedAddressParts(SDNode* N, SDNode* Op, SDValue &Base,
      SDValue &Offset, ISD::MemIndexedMode &AM, SelectionDAG &DAG) const override;

  virtual bool isTruncateFree(Type *SrcTy, Type *DstTy) const override;
  virtual bool isTruncateFree(EVT SrcVT, EVT DstVT) const override;

  /// LowerOperation - Provide custom lowering hooks for some operations.
  virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
  virtual const char *getTargetNodeName(unsigned Opcode) const override;

  virtual MVT getScalarShiftAmountTy(const DataLayout &DL, EVT) const override;
  virtual EVT getSetCCResultType(const DataLayout &, LLVMContext &, EVT VT) const override;

  virtual void ReplaceNodeResults(SDNode * N, SmallVectorImpl<SDValue> &Results, SelectionDAG &DAG) const override;
  virtual MachineBasicBlock* EmitInstrWithCustomInserter(MachineInstr &MI, MachineBasicBlock *BB) const override;

private:
    const TeakSubtarget &Subtarget;

    SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
    SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;

    SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                bool isVarArg,
                                const SmallVectorImpl<ISD::InputArg> &Ins,
                                const SDLoc &dl, SelectionDAG &DAG,
                                SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                      SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals, const SDLoc &dl,
                        SelectionDAG &DAG) const override;

    SDValue LowerCallResult(SDValue Chain, SDValue InGlue,
                            CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
                            SelectionDAG &DAG,
                            SmallVectorImpl<SDValue> &InVals) const;

    bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                        bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &ArgsFlags,
                        LLVMContext &Context) const override;

    // LowerGlobalAddress - Emit a constant load to the global address.
    SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

    MachineBasicBlock* ExpandSelectCC(MachineInstr &MI, MachineBasicBlock *BB, unsigned BROpcode) const;
};
}

#endif // TEAKISELLOWERING_H
