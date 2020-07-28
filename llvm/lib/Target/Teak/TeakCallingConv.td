//===- TeakCallingConv.td - Calling Conventions for Teak -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for LEG architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Teak Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_Teak : CallingConv<[
  //CCIfType<[i8], CCPromoteToType<i16>>,
  CCIfType<[i1], CCPromoteToType<i16>>,
  CCIfType<[i8], CCPromoteToType<i16>>,
  //CCIfType<[i16], CCPromoteToType<i40>>,
  CCIfType<[i16], CCAssignToReg<[A0L, A1L]>>,

  CCIfType<[i32], CCPromoteToType<i40>>,
  // i32 are returned in register A0 and A1  
  CCIfType<[i40], CCAssignToReg<[A0, A1]>>//,
  //CCIfType<[i40], CCAssignToRegWithShadow<[A0, A1], [A0L, A1L]>>//,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  //CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// Teak Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_Teak : CallingConv<[
  // Promote i8/i16 arguments to i32.
  //CCIfType<[i8], CCPromoteToType<i16>>,

  //CCIfType<[i16], CCAssignToRegWithShadow<[A0L, A1L, B0L, B1L], [A0, A1, B0, B1]>>,

  CCIfType<[i1], CCPromoteToType<i16>>,
  CCIfType<[i8], CCPromoteToType<i16>>,

  CCIfType<[i16], CCAssignToReg<[A0L, A1L, B0L, B1L]>>,
  //CCIfType<[i16], CCPromoteToType<i40>>,

  CCIfType<[i32], CCPromoteToType<i40>>,

  // The first 4 integer arguments are passed in integer registers.
  CCIfType<[i40], CCAssignToReg<[A0, A1, B0, B1]>>//,
  //CCIfType<[i40], CCAssignToRegWithShadow<[A0, A1, B0, B1], [A0L, A1L, B0L, B1L]>>//,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  //CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

def CSR : CalleeSavedRegs<(add Y0, Y1, R0, R1, R2, R3, R4, R5, R6, R7, (sequence "PAGE%u", 0, 39))>;
def CSR_NoRegs : CalleeSavedRegs<(add)>;