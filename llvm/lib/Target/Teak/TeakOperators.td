//===-- TeakOperators.td - Teak-specific operators ------*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Type profiles
//===----------------------------------------------------------------------===//

// def MoveImm32Ty : SDTypeProfile<1, 1, [
//   SDTCisSameAs<0, 1>, SDTCisInt<0>
// ]>;

def SDT_TeakCallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i16>, SDTCisVT<1, i16> ]>;

def SDT_TeakCallSeqEnd   : SDCallSeqEnd<[
  SDTCisVT<0, i16>, SDTCisVT<1, i16>
]>;

def TeakRetFlag    : SDNode<"TeakISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_TeakCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_TeakCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_TeakCall    : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;

def SDT_TeakCmpICC : SDTypeProfile<0, 2, [SDTCisInt<0>, SDTCisSameAs<0, 1>]>;
def TeakCmpICC : SDNode<"TeakISD::CMPICC", SDT_TeakCmpICC, [SDNPOutGlue]>;

def SDT_TeakSelectCC : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisVT<3, i40>]>;
def TeakSelectICC : SDNode<"TeakISD::SELECT_ICC", SDT_TeakSelectCC, [SDNPInGlue]>;

// Operand for printing out a condition code.
let PrintMethod = "printCondCode" in
  def CCOp : Operand<i40>;

def SDT_TeakBRCC : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i40>]>;
def TeakBRICC : SDNode<"TeakISD::BRICC", SDT_TeakBRCC, [SDNPHasChain, SDNPInGlue]>;

// //===----------------------------------------------------------------------===//
// // Custom SDNodes.
// //===----------------------------------------------------------------------===//

// def load_sym : SDNode<"TeakISD::LOAD_SYM", SDTIntUnaryOp>;

// def movei32 : SDNode<"TeakISD::MOVEi32", MoveImm32Ty>;

def teak_call
    : SDNode<"TeakISD::CALL", SDT_TeakCall,
             [ SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic ]>;

def memsrc : Operand<i16> {
  let MIOperandInfo = (ops FP, i16imm);
  let PrintMethod = "printAddrModeMemSrc";
  let EncoderMethod = "getMemSrcValue";
}

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], []>;

def SDT_TeakWrapper : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;
def TeakWrapper : SDNode<"TeakISD::WRAPPER", SDT_TeakWrapper>; 

// //===----------------------------------------------------------------------===//
// // Operand Definitions.
// //===----------------------------------------------------------------------===//

// def bl_target : Operand<i32>;

// def b_target : Operand<OtherVT>;

// def cc_val : Operand<i32> {
//   let PrintMethod = "printCondCode";
// }

// def memsrc : Operand<i32> {
//   let MIOperandInfo = (ops GRRegs, i32imm);
//   let PrintMethod = "printAddrModeMemSrc";
//   let EncoderMethod = "getMemSrcValue";
// }

// def i32imm_lo : Operand<i32>, ImmLeaf<i32, [{
//   return Imm >= 0 && Imm < 65536;
// }]>;

// def Teakimm8 : Operand<i32>, ImmLeaf<i32, [{
//   return Imm >= 0 && Imm < 256;
// }]>;

// //===----------------------------------------------------------------------===//
// // Complex Pattern Definitions.
// //===----------------------------------------------------------------------===//

// def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;
