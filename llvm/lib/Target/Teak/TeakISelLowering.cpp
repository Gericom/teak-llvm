//===-- TeakISelLowering.cpp - Teak DAG Lowering Implementation ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TeakTargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "TeakISelLowering.h"
#include "Teak.h"
#include "TeakMachineFunctionInfo.h"
#include "TeakSubtarget.h"
#include "TeakTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Signals.h"

using namespace llvm;

const char *TeakTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  default:
	return NULL;
  case TeakISD::RET_FLAG:
	return "RetFlag";
  case TeakISD::LOAD_SYM:
	return "LOAD_SYM";
  case TeakISD::MOVEi32:
	return "MOVEi32";
  case TeakISD::CALL:
	return "CALL";
  }
}

MVT TeakTargetLowering::getScalarShiftAmountTy(const DataLayout &DL,
											   EVT) const {
  return MVT::i40;
}

EVT TeakTargetLowering::getSetCCResultType(const DataLayout &, LLVMContext &, EVT VT) const {
  return MVT::i40;
} 

TeakTargetLowering::TeakTargetLowering(const TeakTargetMachine &TeakTM)
	: TargetLowering(TeakTM), Subtarget(*TeakTM.getSubtargetImpl())
{
	setBooleanContents(ZeroOrOneBooleanContent);
  	setBooleanVectorContents(ZeroOrOneBooleanContent);
	// Set up the register classes.
	//addRegisterClass(MVT::i16, &Teak::GRRegsRegClass);
	addRegisterClass(MVT::i16, &Teak::ABLRegsRegClass);
	//addRegisterClass(MVT::i16, &Teak::ALRegsRegClass);
	addRegisterClass(MVT::i16, &Teak::RegNoBRegs16_nohRegClass);
	//addRegisterClass(MVT::i16, &Teak::Y0RegsRegClass);
	//addRegisterClass(MVT::i32, &Teak::P0RegsRegClass);
	//addRegisterClass(MVT::i16, &Teak::ABHRegsRegClass);
	//addRegisterClass(MVT::i16, &Teak::ABLRegsRegClass);
	//addRegisterClass(MVT::i16, &Teak::ABHRegsRegClass);

	// addRegisterClass(MVT::i16, &Teak::RegNoBRegs16RegClass);
	//addRegisterClass(MVT::i40, &Teak::RegNoBRegs40RegClass);
	addRegisterClass(MVT::i40, &Teak::ABRegsRegClass);
	//addRegisterClass(MVT::i40, &Teak::ARegsRegClass);
	//addRegisterClass(MVT::i40, &Teak::BRegsRegClass);



	// Compute derived properties from the register classes
	computeRegisterProperties(Subtarget.getRegisterInfo());
	
	//for (unsigned Op = 0; Op < ISD::BUILTIN_OP_END; ++Op)
    //  	setOperationAction(Op, MVT::i32, Expand);

	//setOperationAction(ISD::MUL, MVT::i16, Legal);
	//setOperationAction(ISD::MUL, MVT::i32, Legal);
	//setOperationAction(ISD::MUL, MVT::i40, Expand);

	setOperationAction(ISD::ADD, MVT::i16, Custom);
	setOperationAction(ISD::SUB, MVT::i16, Custom);
	setOperationAction(ISD::AND, MVT::i16, Custom);
	setOperationAction(ISD::AND, MVT::i40, Custom);
	setOperationAction(ISD::XOR, MVT::i16, Custom);
	setOperationAction(ISD::XOR, MVT::i40, Custom);
	setOperationAction(ISD::OR, MVT::i16, Custom);
	setOperationAction(ISD::OR, MVT::i40, Custom);
	setOperationAction(ISD::SHL, MVT::i16, Custom);
	setOperationAction(ISD::SHL, MVT::i40, Custom);
	setOperationAction(ISD::SRL, MVT::i16, Custom);
	setOperationAction(ISD::SRL, MVT::i40, Custom);
	setOperationAction(ISD::SRA, MVT::i16, Custom);
	setOperationAction(ISD::SRA, MVT::i40, Custom);

	setStackPointerRegisterToSaveRestore(Teak::SP);

	setSchedulingPreference(Sched::RegPressure);

	// Nodes that require custom lowering
	setOperationAction(ISD::GlobalAddress, MVT::i16, Custom);
	setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
	setOperationAction(ISD::GlobalAddress, MVT::i40, Custom);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i40, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
	setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);

	setOperationAction(ISD::Constant, MVT::i32, Promote);
	setOperationAction(ISD::Constant, MVT::i8, Promote);

	// setOperationAction(ISD::SIGN_EXTEND, MVT::i40, Expand);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i32, Promote);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i16, Promote);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i8, Promote);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i1, Promote);

	setOperationAction(ISD::SETCC, MVT::i40, Expand);
	setOperationAction(ISD::SETCC, MVT::i32, Expand);
	setOperationAction(ISD::SETCC, MVT::i16, Expand);
	setOperationAction(ISD::SETCC, MVT::i8, Expand);
	setOperationAction(ISD::SELECT, MVT::i40, Expand);
	setOperationAction(ISD::SELECT, MVT::i32, Expand);
	setOperationAction(ISD::SELECT, MVT::i16, Expand);
	setOperationAction(ISD::SELECT, MVT::i8, Expand);
	setOperationAction(ISD::SELECT_CC, MVT::i40, Custom);
	setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
	setOperationAction(ISD::SELECT_CC, MVT::i16, Custom);
	setOperationAction(ISD::SELECT_CC, MVT::i8, Custom);
	setOperationAction(ISD::BRCOND, MVT::Other, Expand);
	setOperationAction(ISD::BRIND, MVT::Other, Expand);
	setOperationAction(ISD::BR_JT, MVT::Other, Expand);
	setOperationAction(ISD::BR_CC, MVT::i8, Custom);
	setOperationAction(ISD::BR_CC, MVT::i16, Custom);
	setOperationAction(ISD::BR_CC, MVT::i32, Custom);
	setOperationAction(ISD::BR_CC, MVT::i40, Custom);

	//support for post increment and decrement
	setIndexedLoadAction(ISD::POST_INC, MVT::i16, Legal);
	setIndexedLoadAction(ISD::POST_DEC, MVT::i16, Legal);
	setIndexedStoreAction(ISD::POST_INC, MVT::i16, Legal);
	setIndexedStoreAction(ISD::POST_DEC, MVT::i16, Legal);

	// setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
	// setOperationAction(ISD::STACKSAVE, MVT::i40, Expand);
  	// setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);
  	// setOperationAction(ISD::STACKRESTORE, MVT::i40, Expand);

	//setOperationAction(ISD::SHL, MVT::i16, Promote);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i40, Expand);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i32, Expand);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i16, Expand);
	// setOperationAction(ISD::SIGN_EXTEND, MVT::i8, Expand);

	// setOperationAction(ISD::AND, MVT::i16, Promote);
	// setOperationAction(ISD::ADD, MVT::i16, Promote);
	// setOperationAction(ISD::Constant, MVT::i32, Promote);
	// setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8,  Expand);
	// setOperationAction(ISD::TRUNCATE, MVT::i40, Expand);
	// setOperationAction(ISD::TRUNCATE, MVT::i32, Expand);
	// setOperationAction(ISD::TRUNCATE, MVT::i16, Expand);
	// setOperationAction(ISD::TRUNCATE, MVT::i8, Expand);
	// setOperationAction(ISD::TRUNCATE, MVT::i8,  Promote);
	// setOperationAction(ISD::SRA, MVT::i40, Custom);
	// setOperationAction(ISD::SRA, MVT::i32, Custom);
	// setOperationAction(ISD::SRA, MVT::i16, Custom);
	// setOperationAction(ISD::SRA, MVT::i8, Custom);
	// setOperationAction(ISD::TRUNCATE, MVT::i32, Promote);
	// setOperationAction(ISD::CopyToReg, MVT::i32,  Custom);
	// setOperationAction(ISD::CopyFromReg, MVT::i32,  Custom);
	// setOperationAction(ISD::CopyToReg, MVT::i40,  Expand);

	setLoadExtAction(ISD::ZEXTLOAD, MVT::i40, MVT::i32, Expand);

	setLoadExtAction(ISD::EXTLOAD, MVT::i16, MVT::i1, Promote);
	setLoadExtAction(ISD::EXTLOAD, MVT::i40, MVT::i1, Promote);
	setLoadExtAction(ISD::EXTLOAD, MVT::i16, MVT::i8, Promote);
	setLoadExtAction(ISD::EXTLOAD, MVT::i40, MVT::i8, Promote);
	setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, MVT::i1, Promote);
	setLoadExtAction(ISD::ZEXTLOAD, MVT::i40, MVT::i1, Promote);
	setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, MVT::i8, Promote);
	setLoadExtAction(ISD::ZEXTLOAD, MVT::i40, MVT::i8, Promote);
	setLoadExtAction(ISD::SEXTLOAD, MVT::i16, MVT::i1, Promote);
	setLoadExtAction(ISD::SEXTLOAD, MVT::i40, MVT::i1, Promote);
	setLoadExtAction(ISD::SEXTLOAD, MVT::i16, MVT::i8, Promote);
	setLoadExtAction(ISD::SEXTLOAD, MVT::i40, MVT::i8, Promote);

	setTruncStoreAction(MVT::i40, MVT::i1, Expand);
	setTruncStoreAction(MVT::i32, MVT::i1, Expand);
	setTruncStoreAction(MVT::i16, MVT::i1, Expand);
	setTruncStoreAction(MVT::i8, MVT::i1, Expand);
	setTruncStoreAction(MVT::i40, MVT::i8, Expand);
	setTruncStoreAction(MVT::i32, MVT::i8, Expand);
	setTruncStoreAction(MVT::i16, MVT::i8, Expand);
}

// SDValue TeakTargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const
// {
// 	SDValue LHS = Op.getOperand(0);
// 	SDValue RHS = Op.getOperand(1);
// 	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
// 	SDLoc DL(Op);

// 	SDValue TargetCC;
// 	SDValue Cmp = getAVRCmp(LHS, RHS, CC, TargetCC, DAG, DL);

// 	SDValue TrueV = DAG.getConstant(1, DL, Op.getValueType());
// 	SDValue FalseV = DAG.getConstant(0, DL, Op.getValueType());
// 	SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
// 	SDValue Ops[] = {TrueV, FalseV, TargetCC, Cmp};

// 	return DAG.getNode(AVRISD::SELECT_CC, DL, VTs, Ops);
// }

bool TeakTargetLowering::isNarrowingProfitable(EVT VT1, EVT VT2) const
{
  	return VT2 != MVT::i8 && VT2 != MVT::i1;
}

bool TeakTargetLowering::getPostIndexedAddressParts(SDNode* N, SDNode* Op,
	SDValue &Base, SDValue &Offset, ISD::MemIndexedMode &AM, SelectionDAG &DAG) const 
{
	return false;
	// SDLoc DL(N);
	// const LoadSDNode* ld = dyn_cast<LoadSDNode>(N);
	// if (!ld)
	// 	return false;
	// if(ld->getMemoryVT() != MVT::i16)
	// 	return false;
	// if (Op->getOpcode() != ISD::ADD && Op->getOpcode() != ISD::SUB)
	// 	return false;	

	// if (const ConstantSDNode* rhs = dyn_cast<ConstantSDNode>(Op->getOperand(1)))
	// {
	// 	int rhsc = rhs->getSExtValue();
	// 	if (Op->getOpcode() == ISD::SUB)
	// 		rhsc = -rhsc;
	// 	if (rhsc != 1 && rhsc != -1)
	// 		return false;

	// 	Base = Op->getOperand(0);
	// 	Offset = DAG.getConstant(rhsc, DL, MVT::i16);
	// 	AM = ISD::POST_INC;

	// 	return true;
	// }
	// // dbgs() << "getPostIndexedAddressParts\n";
	// // EVT VT;
	// // SDLoc DL(N);
	// // Op->dump();
	// // N->dump();
	// return false;
}

bool TeakTargetLowering::allowsMemoryAccess(LLVMContext &Context, const DataLayout &DL, EVT VT, unsigned AddrSpace,
	unsigned Alignment, MachineMemOperand::Flags Flags, bool *Fast) const
{
	if(VT.getSizeInBits() != 16 && VT.getSizeInBits() != 32)
		return false;
	return TargetLowering::allowsMemoryAccess(Context, DL, VT, AddrSpace, Alignment, Flags, Fast);
}

bool TeakTargetLowering::shouldReduceLoadWidth(SDNode* Load, ISD::LoadExtType ExtTy, EVT NewVT) const
{
	if(NewVT.getSizeInBits() == 8)
		return false;
	return TargetLowering::shouldReduceLoadWidth(Load, ExtTy, NewVT);
}


bool TeakTargetLowering::isLegalICmpImmediate(int64_t Imm) const
{
	return Imm >= -32768 && Imm <= 32767;
}

bool TeakTargetLowering::isLegalAddImmediate(int64_t Imm) const
{
	return Imm >= -32768 && Imm <= 32767;
}

bool TeakTargetLowering::isTruncateFree(Type *SrcTy, Type *DstTy) const
{
	if (!SrcTy->isIntegerTy() || !DstTy->isIntegerTy())
		return false;
	unsigned SrcBits = SrcTy->getPrimitiveSizeInBits();
	unsigned DestBits = DstTy->getPrimitiveSizeInBits();
	return (SrcBits == 40 && DestBits == 16);
}

bool TeakTargetLowering::isTruncateFree(EVT SrcVT, EVT DstVT) const
{
	if (SrcVT.isVector() || DstVT.isVector() || !SrcVT.isInteger() || !DstVT.isInteger())
		return false;
	unsigned SrcBits = SrcVT.getSizeInBits();
	unsigned DestBits = DstVT.getSizeInBits();
	return (SrcBits == 40 && DestBits == 16);
}

// Look at LHS/RHS/CC and see if they are a lowered setcc instruction.  If so
// set LHS/RHS and SPCC to the LHS/RHS of the setcc and SPCC to the condition.
static void lookThroughSetCC(SDValue &LHS, SDValue &RHS, ISD::CondCode CC, unsigned &SPCC)
{
	if (isNullConstant(RHS) && CC == ISD::SETNE && LHS.getOpcode() == TeakISD::SELECT_ICC &&
		LHS.getOperand(3).getOpcode() == TeakISD::CMPICC &&	isOneConstant(LHS.getOperand(0)) &&
		isNullConstant(LHS.getOperand(1)))
	{
		SDValue CMPCC = LHS.getOperand(3);
		SPCC = cast<ConstantSDNode>(LHS.getOperand(2))->getZExtValue();
		LHS = CMPCC.getOperand(0);
		RHS = CMPCC.getOperand(1);
	}
}

static TeakCC::CondCodes IntCondCCodeToICC(ISD::CondCode CC)
{
	switch (CC)
	{
		default: llvm_unreachable("Unknown integer condition code!");
		case ISD::SETEQ:  return TeakCC::Eq;
		case ISD::SETNE:  return TeakCC::Neq;
		case ISD::SETLT:  return TeakCC::Lt;
		case ISD::SETGT:  return TeakCC::Gt;
		case ISD::SETLE:  return TeakCC::Le;
		case ISD::SETGE:  return TeakCC::Ge;
		case ISD::SETULT: return TeakCC::Lt;
		case ISD::SETULE: return TeakCC::Le;
		case ISD::SETUGT: return TeakCC::Gt;
		case ISD::SETUGE: return TeakCC::Ge;
	}
}

SDValue TeakTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const
{
	SDValue LHS = Op.getOperand(0);
	SDValue RHS = Op.getOperand(1);
	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
	SDValue TrueVal = Op.getOperand(2);
	SDValue FalseVal = Op.getOperand(3);
	SDLoc dl(Op);
	unsigned Opc, SPCC = ~0U;

	// If this is a select_cc of a "setcc", and if the setcc got lowered into
	// an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
	lookThroughSetCC(LHS, RHS, CC, SPCC);

	bool unsgn = false;
	if(CC == ISD::SETULT || CC == ISD::SETULE || CC == ISD::SETUGT || CC == ISD::SETUGE)
		unsgn = true;

	if(LHS.getValueType() != MVT::i40)
		LHS = DAG.getNode(unsgn ? ISD::ZERO_EXTEND : ISD::SIGN_EXTEND, dl, MVT::i40, LHS);

	if(RHS.getValueType() != MVT::i40)
		RHS = DAG.getNode(unsgn ? ISD::ZERO_EXTEND : ISD::SIGN_EXTEND, dl, MVT::i40, RHS);

	SDValue CompareFlag;
	if (LHS.getValueType().isInteger())
	{
		CompareFlag = DAG.getNode(TeakISD::CMPICC, dl, MVT::Glue, LHS, RHS);
		Opc = TeakISD::SELECT_ICC;
		if (SPCC == ~0U)
			SPCC = IntCondCCodeToICC(CC);
	}
	else
		llvm_unreachable("!LHS.getValueType().isInteger()");
	return DAG.getNode(Opc, dl, TrueVal.getValueType(), TrueVal, FalseVal,
						DAG.getConstant(SPCC, dl, MVT::i40), CompareFlag);
}

SDValue TeakTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const
{
	SDValue Chain = Op.getOperand(0);
	ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
	SDValue LHS = Op.getOperand(2);
	SDValue RHS = Op.getOperand(3);
	SDValue Dest = Op.getOperand(4);
	SDLoc dl(Op);
	unsigned Opc, SPCC = ~0U;

	// If this is a br_cc of a "setcc", and if the setcc got lowered into
	// an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
	lookThroughSetCC(LHS, RHS, CC, SPCC);

	bool unsgn = false;
	if(CC == ISD::SETULT || CC == ISD::SETULE || CC == ISD::SETUGT || CC == ISD::SETUGE)
		unsgn = true;

	if(LHS.getValueType() != MVT::i40)
		LHS = DAG.getNode(unsgn ? ISD::ZERO_EXTEND : ISD::SIGN_EXTEND, dl, MVT::i40, LHS);

	if(RHS.getValueType() != MVT::i40)
		RHS = DAG.getNode(unsgn ? ISD::ZERO_EXTEND : ISD::SIGN_EXTEND, dl, MVT::i40, RHS);

	// Get the condition flag.
	SDValue CompareFlag;
	if (LHS.getValueType().isInteger())
	{
		CompareFlag = DAG.getNode(TeakISD::CMPICC, dl, MVT::Glue, LHS, RHS);
		if (SPCC == ~0U)
			SPCC = IntCondCCodeToICC(CC);
		// 32-bit compares use the icc flags, 64-bit uses the xcc flags.
		Opc = TeakISD::BRICC;
	}
	else
		llvm_unreachable("!LHS.getValueType().isInteger()");
	return DAG.getNode(Opc, dl, MVT::Other, Chain, Dest,
						DAG.getConstant(SPCC, dl, MVT::i40), CompareFlag);
}

SDValue TeakTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
	// dbgs() << "LowerOperation\n";
	// sys::PrintStackTrace(dbgs());

	// DAG.dump();
	// Op.dump(&DAG);
	switch (Op.getOpcode()) 
	{
		default:
			Op.dump(&DAG);
			llvm_unreachable("Unimplemented operand");
	//case ISD::SELECT:        return LowerSELECT(Op, DAG);
		case ISD::SELECT_CC:
			return LowerSELECT_CC(Op, DAG); 
		case ISD::BR_CC:
			return LowerBR_CC(Op, DAG); 
		//case ISD::SETCC:
		//	return LowerSETCC(Op, DAG);
		case ISD::GlobalAddress:
			return LowerGlobalAddress(Op, DAG);
		case ISD::ADD:
		case ISD::SUB:
		{
			const SDNode *N = Op.getNode();
			SDLoc dl(N);
			assert(N->getValueType(0) == MVT::i16 && "Unexpected custom legalisation");
			//this seems to give problems for some reason
			// if(isa<ConstantSDNode>(N->getOperand(1)))
			// {
			// 	dbgs() << "constant!\n";
			// 	return Op;
			// }
			//dbgs() << "Lowering!\n";
			SDValue NewOp0 = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i40, N->getOperand(0));
			SDValue NewOp1 = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i40, N->getOperand(1));
			SDValue NewWOp = DAG.getNode(Op.getOpcode(), dl, MVT::i40, NewOp0, NewOp1);
  			return DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, NewWOp);
		}
		case ISD::AND:
		case ISD::OR:
		case ISD::XOR:
		{
			const SDNode *N = Op.getNode();
			SDLoc dl(N);
			TeakISD::NodeType nodeType;
			switch(Op.getOpcode())
			{
				case ISD::AND:
					nodeType = TeakISD::AND;
					break;
				case ISD::OR:
					nodeType = TeakISD::OR;
					break;
				case ISD::XOR:
					nodeType = TeakISD::XOR;
					break;
			}
			if(N->getValueType(0) == MVT::i40)
				return DAG.getNode(nodeType, dl, MVT::i40, N->getOperand(0), N->getOperand(1));
			assert(N->getValueType(0) == MVT::i16 && "Unexpected custom legalisation");
			if(isa<ConstantSDNode>(N->getOperand(1)))
				return DAG.getNode(nodeType, dl, MVT::i16, N->getOperand(0), N->getOperand(1));
			SDValue NewOp0 = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i40, N->getOperand(0));
			SDValue NewOp1 = DAG.getNode(ISD::ANY_EXTEND, dl, MVT::i40, N->getOperand(1));
			SDValue NewWOp = DAG.getNode(nodeType, dl, MVT::i40, NewOp0, NewOp1);
  			return DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, NewWOp);
		}
		case ISD::SHL:
		{
			const SDNode *N = Op.getNode();
		 	SDLoc dl(N);
			if(N->getValueType(0) == MVT::i16)
			{
				return 
					DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
						DAG.getNode(TeakISD::SHIFT_ARITH, dl, MVT::i40, 
							DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40,
								N->getOperand(0)), 
							DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, N->getOperand(1))));
			}
			else if(N->getValueType(0) == MVT::i40)
			{
				//emit arithmetic shift, because it is the same as logical for left
				//shifts, and we don't want to modify the shift flag if not needed
				return DAG.getNode(TeakISD::SHIFT_ARITH, dl, MVT::i40, 
					N->getOperand(0), 
					DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, N->getOperand(1)));
			}
			else
				llvm_unreachable("Unimplemented shift operand");
		}
		case ISD::SRL:
		{
			const SDNode* N = Op.getNode();
		 	SDLoc dl(N);
			SDValue shiftOp = N->getOperand(1);
			if(N->getValueType(0) == MVT::i16)
			{
				return
					DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
						DAG.getNode(TeakISD::SHIFT_LOGIC, dl, MVT::i40, 
							DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40,
								N->getOperand(0)), 
							//negation for right shift
							DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
								DAG.getNode(ISD::SUB, dl, MVT::i40,
									DAG.getConstant(0, dl, MVT::i40), N->getOperand(1)))));
			}
			else if(N->getValueType(0) == MVT::i40)
				return
					DAG.getNode(TeakISD::SHIFT_LOGIC, dl, MVT::i40,
						N->getOperand(0), 
						//negation for right shift
						DAG.getNode(ISD::TRUNCATE, dl, MVT::i16,
							DAG.getNode(ISD::SUB, dl, MVT::i40,
								DAG.getConstant(0, dl, MVT::i40), N->getOperand(1))));
			else
				llvm_unreachable("Unimplemented shift operand");
		}
		case ISD::SRA:
		{
			const SDNode *N = Op.getNode();
		 	SDLoc dl(N);
			if(N->getValueType(0) == MVT::i16)
			{
				return
					DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
						DAG.getNode(TeakISD::SHIFT_ARITH, dl, MVT::i40, 
							DAG.getNode(ISD::SIGN_EXTEND, dl, MVT::i40,
								N->getOperand(0)), 
							//negation for right shift
							DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
								DAG.getNode(ISD::SUB, dl, MVT::i40,
									DAG.getConstant(0, dl, MVT::i40), N->getOperand(1)))));
			}
			else if(N->getValueType(0) == MVT::i40)
				return
					DAG.getNode(TeakISD::SHIFT_ARITH, dl, MVT::i40,
						N->getOperand(0), 
						//negation for right shift
						DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, 
							DAG.getNode(ISD::SUB, dl, MVT::i40,
								DAG.getConstant(0, dl, MVT::i40), N->getOperand(1))));
			else
				llvm_unreachable("Unimplemented shift operand");
		}
		// case ISD::SHL:
		// case ISD::SRL:
		// case ISD::SRA:
		// {
		// 	DAG.dump();
		// 	const SDNode *N = Op.getNode();
		// 	SDLoc dl(N);
		// 	assert(N->getValueType(0) == MVT::i16 && "Unexpected custom legalisation");
		// 	SDValue NewOp0 = DAG.getNode(Op.getOpcode() == ISD::SRA ? ISD::SIGN_EXTEND : ISD::ZERO_EXTEND, dl, MVT::i40, N->getOperand(0));
		// 	SDValue NewWOp = DAG.getNode(N->getOpcode(), dl, MVT::i40, NewOp0, N->getOperand(1));
		// 	//SDValue NewRes = DAG.getNode(ISD::SIGN_EXTEND_INREG, dl, MVT::i40, NewWOp, DAG.getValueType(MVT::i16));
  		// 	return DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, NewWOp);//NewRes);
		// }
		// case ISD::MUL:
		// {
		// 	const SDNode *N = Op.getNode();
		// 	SDLoc dl(N);
		// 	assert(N->getValueType(0) == MVT::i16 && "Unexpected custom legalisation");
		// 	//SDValue NewOp0 = DAG.getNode(ISD::SIGN_EXTEND, dl, MVT::i32, N->getOperand(0));
		// 	//SDValue NewOp1 = DAG.getNode(ISD::SIGN_EXTEND, dl, MVT::i32, N->getOperand(1));
		// 	SDValue NewWOp = DAG.getNode(Teak::MOV_p0_regnob16, dl, MVT::i16, DAG.getNode(Teak::MPY_y0_regnob16, dl, MVT::i32, N->getOperand(0), N->getOperand(1)));
		// 	//SDValue NewRes = DAG.getNode(ISD::SIGN_EXTEND_INREG, dl, MVT::i40, NewWOp, DAG.getValueType(MVT::i16));
  		// 	return NewWOp;//DAG.getNode(ISD::TRUNCATE, dl, MVT::i16, NewWOp);//NewRes);
		// }
		// case ISD::SRA:
		// {
		//   const SDNode *N = Op.getNode();
		//    SDLoc dl(N);
		//   return DAG.getNode(Teak::SHFI_ab_ab, dl, MVT::i40, Op->getOperand(0),
		//   Op->getOperand(1));
		// }
		// case ISD::CopyToReg:
		// {
		//   dbgs() << "LowerOperation = ISD::CopyToReg\n";
		//   const SDNode *N = Op.getNode();
		//   SDLoc dl(N);
		//   return DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40,
		//   Op->getOperand(1));//DAG.getCopyToReg(Op.getCha, dl, Op->getOperand(0),
		//   DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40, Op->getOperand(1)),
		//   Op->getOperand(2));
		// }
		// case ISD::CopyFromReg:
		// {
		//   dbgs() << "LowerOperation = ISD::CopyFromReg\n";
		//   const SDNode *N = Op.getNode();
		//   SDLoc dl(N);
		//   return DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40,
		//   Op);//DAG.getCopyToReg(Op.getCha, dl, Op->getOperand(0),
		//   DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40, Op->getOperand(1)),
		//   Op->getOperand(2));
		// }
	}
	return SDValue();
}

SDValue TeakTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const
{
	EVT VT = Op.getValueType();
	GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(Op.getNode());
	int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();
	if(VT == MVT::i16)
		return DAG.getNode(TeakISD::WRAPPER, SDLoc(Op), MVT::i16, DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), Op, MVT::i16, Offset));
	else
		return DAG.getNode(TeakISD::WRAPPER, SDLoc(Op), MVT::i40, DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), Op, MVT::i40, Offset));
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "TeakGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// Teak call implementation
SDValue TeakTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
									  SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &Loc = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  const bool isVarArg = CLI.IsVarArg;

  CLI.IsTailCall = false;

  if (isVarArg) {
	llvm_unreachable("Unimplemented");
  }

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
				 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Teak);

  // Get the size of the outgoing arguments stack space requirement.
  const unsigned NumBytes = CCInfo.getNextStackOffset();

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, Loc);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
	CCValAssign &VA = ArgLocs[i];
	SDValue Arg = OutVals[i];

	// We only handle fully promoted arguments.
	//dbgs() << VA.getLocInfo() << "\n";
	//assert(VA.getLocInfo() == CCValAssign::Full && "Unhandled loc info");

	// Promote the value if needed. With Clang this should not happen.
    switch (VA.getLocInfo()) {
		default:
			llvm_unreachable("Unknown loc info!");
		case CCValAssign::Full:
			break;
		case CCValAssign::SExt:
			Arg = DAG.getNode(ISD::SIGN_EXTEND, Loc, VA.getLocVT(), Arg);
			break;
		case CCValAssign::ZExt:
			Arg = DAG.getNode(ISD::ZERO_EXTEND, Loc, VA.getLocVT(), Arg);
			break;
		case CCValAssign::AExt:
			Arg = DAG.getNode(ISD::ANY_EXTEND, Loc, VA.getLocVT(), Arg);
			break;
		case CCValAssign::BCvt:
			Arg = DAG.getNode(ISD::BITCAST, Loc, VA.getLocVT(), Arg);
			break;
    }

	if (VA.isRegLoc()) {
	  RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
	  continue;
	}

	assert(VA.isMemLoc() &&
		   "Only support passing arguments through registers or via the stack");

	SDValue StackPtr = DAG.getRegister(Teak::SP, MVT::i32);
	SDValue PtrOff = DAG.getIntPtrConstant(VA.getLocMemOffset(), Loc);
	PtrOff = DAG.getNode(ISD::ADD, Loc, MVT::i32, StackPtr, PtrOff);
	MemOpChains.push_back(
		DAG.getStore(Chain, Loc, Arg, PtrOff, MachinePointerInfo()));
  }

  // Emit all stores, make sure they occur before the call.
  if (!MemOpChains.empty()) {
	Chain = DAG.getNode(ISD::TokenFactor, Loc, MVT::Other, MemOpChains);
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain
  // and flag operands which copy the outgoing args into the appropriate regs.
  SDValue InFlag;
  for (auto &Reg : RegsToPass)
  {
	Chain = DAG.getCopyToReg(Chain, Loc, Reg.first, Reg.second, InFlag);
	InFlag = Chain.getValue(1);
  }

  // We only support calling global addresses.
  //GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee);
  //assert(G && "We only support the calling of global addresses");

  //EVT PtrVT = getPointerTy(DAG.getDataLayout());
  //Callee = DAG.getGlobalAddress(G->getGlobal(), Loc, PtrVT, 0);
  if (const GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
  {
    	const GlobalValue *GV = G->getGlobal();
    	Callee = DAG.getTargetGlobalAddress(GV, Loc, MVT::i40);
  } 
  else if (const ExternalSymbolSDNode *ES = dyn_cast<ExternalSymbolSDNode>(Callee))
    	Callee = DAG.getTargetExternalSymbol(ES->getSymbol(), MVT::i40);

  std::vector<SDValue> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (auto &Reg : RegsToPass) {
	Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  }

  // Add a register mask operand representing the call-preserved registers.
  const uint32_t *Mask;
  const TargetRegisterInfo *TRI = DAG.getSubtarget().getRegisterInfo();
  Mask = TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);

  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode()) {
	Ops.push_back(InFlag);
  }

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  // Returns a chain and a flag for retval copy to use.
  Chain = DAG.getNode(TeakISD::CALL, Loc, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, Loc, true),
							 DAG.getIntPtrConstant(0, Loc, true), InFlag, Loc);
  if (!Ins.empty()) {
	InFlag = Chain.getValue(1);
  }

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, Loc, DAG,
						 InVals);
}

SDValue TeakTargetLowering::LowerCallResult(
	SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
	const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
	SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  assert(!isVarArg && "Unsupported");

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
				 *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Teak);

  // Copy all of the result registers out of their specified physreg.
  for (auto &Loc : RVLocs) {
	SDValue RV =
		DAG.getCopyFromReg(Chain, dl, Loc.getLocReg(), Loc.getValVT(), InGlue);
	Chain = RV.getValue(1);
	InGlue = Chain.getValue(2);

	// if (Loc.isExtInLoc())
	//   RV = DAG.getNode(ISD::TRUNCATE, dl, Loc.getValVT(), RV);
	InVals.push_back(RV);
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// Teak formal arguments implementation
SDValue TeakTargetLowering::LowerFormalArguments(
	SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
	const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
	SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  assert(!isVarArg && "VarArg not supported");

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
				 *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_Teak);

  for (auto &VA : ArgLocs) {
	if (VA.isRegLoc()) {
	  // Arguments passed in registers
	  EVT RegVT = VA.getLocVT();
	  const TargetRegisterClass *RC;

	  if (RegVT == MVT::i16)
		RC = &Teak::ABLRegsRegClass;
	  else if (RegVT == MVT::i40)
		RC = &Teak::ABRegsRegClass;
	  else
		llvm_unreachable("RegVT not supported by FORMAL_ARGUMENTS Lowering");

	  // assert((RegVT.getSimpleVT().SimpleTy == MVT::i16 ||
	  // RegVT.getSimpleVT().SimpleTy == MVT::i40) &&
	  //      "Only support MVT::i16 and MVT::i40 register passing");
	  dbgs() << "LowerFormalArguments: " << RegVT.getEVTString()
			 << " locreg=" << VA.getLocReg() << "\n";
	  unsigned VReg = MF.addLiveIn(VA.getLocReg(), RC);
	  SDValue ArgIn = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
	  // Truncate the register down to the argument type.
	  // if (VA.isExtInLoc())
	  //   ArgIn = DAG.getNode(ISD::TRUNCATE, dl, VA.getValVT(), ArgIn);

	  InVals.push_back(ArgIn);
	  continue;
	}

	assert(VA.isRegLoc() && "TODO implement argument passing on stack");

	// assert(VA.isMemLoc() &&
	//        "Can only pass arguments as either registers or via the stack");

	// const unsigned Offset = VA.getLocMemOffset();

	// const int FI = MF.getFrameInfo().CreateFixedObject(4, Offset, true);
	// EVT PtrTy = getPointerTy(DAG.getDataLayout());
	// SDValue FIPtr = DAG.getFrameIndex(FI, PtrTy);

	// assert(VA.getValVT() == MVT::i32 &&
	//        "Only support passing arguments as i32");
	// SDValue Load = DAG.getLoad(VA.getValVT(), dl, Chain, FIPtr,
	//                            MachinePointerInfo(), false, false, false, 0);

	// InVals.push_back(Load);
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool TeakTargetLowering::CanLowerReturn(
	CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg,
	const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
  if (!CCInfo.CheckReturn(Outs, RetCC_Teak)) {
	return false;
  }
  if (CCInfo.getNextStackOffset() != 0 && isVarArg) {
	return false;
  }
  return true;
}

SDValue
TeakTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
								bool isVarArg,
								const SmallVectorImpl<ISD::OutputArg> &Outs,
								const SmallVectorImpl<SDValue> &OutVals,
								const SDLoc &dl, SelectionDAG &DAG) const {
  if (isVarArg) {
	report_fatal_error("VarArg not supported");
  }

  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
				 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_Teak);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
	CCValAssign &VA = RVLocs[i];
	assert(VA.isRegLoc() && "Can only return in registers!");

	Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);

	Flag = Chain.getValue(1);
	RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode()) {
	RetOps.push_back(Flag);
  }

  return DAG.getNode(TeakISD::RET_FLAG, dl, MVT::Other, RetOps);
}

void TeakTargetLowering::ReplaceNodeResults(SDNode *N,
											SmallVectorImpl<SDValue> &Results,
											SelectionDAG &DAG) const
{
	dbgs() << "Opcode = " << N->getOpcode();
	DAG.dump();
	N->dumpr(&DAG);
	SDLoc dl(N);
	// if(N->getOpcode() == ISD::GlobalAddress)
	// {
	// 	GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(N);
	// 	SDValue TargetAddr =
	// 		DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), dl, MVT::i40);
	// 	Results.push_back(TargetAddr); // DAG.getConstant(0, dl, MVT::i40));
	// 	//Results.push_back(DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i40,
	// 	//N->getOperand(0)));
	// 	return;
	// }

	// dbgs() << "ReplaceNodeResults not implemented for this op!\n";

	SDValue Res = LowerOperation(SDValue(N, 0), DAG);

    for (unsigned I = 0, E = Res->getNumValues(); I != E; ++I)
      Results.push_back(Res.getValue(I));

	// SDValue InOp = N->getOperand(0);
	// InOp.dump();
	// EVT InVT = InOp.getValueType();
	// dbgs() << "evt: " << InVT.getEVTString();

	// Results[0].dump();
	//llvm_unreachable("ReplaceNodeResults not implemented for this target!");
}

MachineBasicBlock* TeakTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI, MachineBasicBlock *BB) const
{
	switch (MI.getOpcode())
	{
		default:
			llvm_unreachable("Unknown SELECT_CC!");
		case Teak::SELECT_CC_Int_ICC:
		case Teak::SELECT_CC_Int_ICC_i16:
			return ExpandSelectCC(MI, BB, Teak::BRRCond_rel7);
	}
}

MachineBasicBlock* TeakTargetLowering::ExpandSelectCC(MachineInstr &MI, MachineBasicBlock *BB, unsigned BROpcode) const
{
	const TargetInstrInfo &TII = *Subtarget.getInstrInfo();
	DebugLoc dl = MI.getDebugLoc();
	unsigned CC = (TeakCC::CondCodes)MI.getOperand(3).getImm();

	// To "insert" a SELECT_CC instruction, we actually have to insert the
	// triangle control-flow pattern. The incoming instruction knows the
	// destination vreg to set, the condition code register to branch on, the
	// true/false values to select between, and the condition code for the branch.
	//
	// We produce the following control flow:
	//     ThisMBB
	//     |  \
	//     |  IfFalseMBB
	//     | /
	//    SinkMBB
	const BasicBlock *LLVM_BB = BB->getBasicBlock();
	MachineFunction::iterator It = ++BB->getIterator();

	MachineBasicBlock *ThisMBB = BB;
	MachineFunction *F = BB->getParent();
	MachineBasicBlock *IfFalseMBB = F->CreateMachineBasicBlock(LLVM_BB);
	MachineBasicBlock *SinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
	F->insert(It, IfFalseMBB);
	F->insert(It, SinkMBB);

	// Transfer the remainder of ThisMBB and its successor edges to SinkMBB.
	SinkMBB->splice(SinkMBB->begin(), ThisMBB, std::next(MachineBasicBlock::iterator(MI)), ThisMBB->end());
	SinkMBB->transferSuccessorsAndUpdatePHIs(ThisMBB);

	// Set the new successors for ThisMBB.
	ThisMBB->addSuccessor(IfFalseMBB);
	ThisMBB->addSuccessor(SinkMBB);

	BuildMI(ThisMBB, dl, TII.get(BROpcode))
		.addMBB(SinkMBB)
		.addImm(CC);
		//.addReg(Teak::ICC, RegState::Implicit);
		//.addReg(Teak::ICC, RegState::Implicit);

	// IfFalseMBB just falls through to SinkMBB.
	IfFalseMBB->addSuccessor(SinkMBB);

	// %Result = phi [ %TrueValue, ThisMBB ], [ %FalseValue, IfFalseMBB ]
	BuildMI(*SinkMBB, SinkMBB->begin(), dl, TII.get(Teak::PHI), MI.getOperand(0).getReg())
		.addReg(MI.getOperand(1).getReg())
		.addMBB(ThisMBB)
		.addReg(MI.getOperand(2).getReg())
		.addMBB(IfFalseMBB);

	MI.eraseFromParent(); // The pseudo instruction is gone now.
	return SinkMBB;
}