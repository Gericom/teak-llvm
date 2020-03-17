//===-- TeakRegisterInfo.td - Teak Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the LEG register file 
//===----------------------------------------------------------------------===//

class TeakReg<bits<7> Enc, string n, list<Register> subregs = []> : Register<n> {
  let HWEncoding{15-7} = 0;
  let HWEncoding{6-0} = Enc;
  let Namespace = "Teak";
  let SubRegs = subregs;
}

let Namespace = "Teak" in {
  def sub_16bit     : SubRegIndex<16>;
  def sub_16bit_hi  : SubRegIndex<16, 16>;
  def sub_8bit_acc_ext  : SubRegIndex<8, 32>;
}

def A0L : TeakReg<0, "a0l">;
def A0H : TeakReg<0, "a0h">;
def A0E : TeakReg<0, "a0e">;

def A1L : TeakReg<0, "a1l">;
def A1H : TeakReg<0, "a1h">;
def A1E : TeakReg<0, "a1e">;

def B0L : TeakReg<0, "b0l">;
def B0H : TeakReg<0, "b0h">;
def B0E : TeakReg<0, "b0e">;

def B1L : TeakReg<0, "b1l">;
def B1H : TeakReg<0, "b1h">;
def B1E : TeakReg<0, "b1e">;

let SubRegIndices = [sub_16bit, sub_16bit_hi, sub_8bit_acc_ext], CoveredBySubRegs = 1 in {
  def A0  : TeakReg<0, "a0", [A0L, A0H, A0E]>;
  def A1  : TeakReg<0, "a1", [A1L, A1H, A1E]>;
  def B0  : TeakReg<0, "b0", [B0L, B0H, B0E]>;
  def B1  : TeakReg<0, "b1", [B1L, B1H, B1E]>;
}

let SubRegIndices = [sub_16bit, sub_16bit_hi], CoveredBySubRegs = 1 in {
  def A0_32  : TeakReg<0, "a0", [A0L, A0H]>;
  def A1_32  : TeakReg<0, "a1", [A1L, A1H]>;
  def B0_32  : TeakReg<0, "b0", [B0L, B0H]>;
  def B1_32  : TeakReg<0, "b1", [B1L, B1H]>;
}

def R0  : TeakReg<0, "r0">;
def R1  : TeakReg<0, "r1">;
def R2  : TeakReg<0, "r2">;
def R3  : TeakReg<0, "r3">;
def R4  : TeakReg<0, "r4">;
def R5  : TeakReg<0, "r5">;
def R6  : TeakReg<0, "r6">;
def R7  : TeakReg<0, "r7">;

def X0  : TeakReg<0, "x0">;
def X1  : TeakReg<0, "x1">;

def Y0  : TeakReg<0, "y0">;
def Y1  : TeakReg<0, "y1">;

def P0  : TeakReg<0, "p0">;
//def P0E  : TeakReg<0, "p0e">;
def P1  : TeakReg<0, "p1">;
//def P1E  : TeakReg<0, "p1e">;

def PC  : TeakReg<0, "pc">;
def SP  : TeakReg<0, "sp">;
def SV  : TeakReg<0, "sv">;
def MIXP  : TeakReg<0, "mixp">;
def LC  : TeakReg<0, "lc">;
def REPC  : TeakReg<0, "repc">;
def DVM  : TeakReg<0, "dvm">;

def VTR0  : TeakReg<0, "vtr0">;
def VTR1  : TeakReg<0, "vtr1">;
def PRPAGE  : TeakReg<0, "prpage">;

def STT0 : TeakReg<0, "stt0">;
def STT1 : TeakReg<0, "stt1">;
def STT2 : TeakReg<0, "stt2">;

def MOD0 : TeakReg<0, "mod0">;
def MOD1 : TeakReg<0, "mod1">;
def MOD2 : TeakReg<0, "mod2">;
def MOD3 : TeakReg<0, "mod3">;

def ICC : TeakReg<0, "ICC">;//for flag setting ops, maybe rename

// Register classes.
//
def GRRegs : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7)>;
def RegNoBRegs40 : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
def RegNoBRegs16 : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, B0H, B1H, B0L, B1L, A0L, A1L, A0H, A1H, LC, SV)>;
def RegNoBRegs16_nolh : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, LC, SV)>;
def RegNoBRegs16_noh : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, A0L, A1L, B0L, B1L, LC, SV)>;
//def RegNoBRegsP040 : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
//def RegNoBRegsP016 : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R7, Y0, /**/ SP, B0H, B1H, B0L, B1L, A0L, A1L, A0H, A1H, LC, SV)>;
def ARegs : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
def BRegs : RegisterClass<"Teak", [i40], 32, (add B0, B1)>;
def B0Reg : RegisterClass<"Teak", [i40], 32, (add B0)>;
def B1Reg : RegisterClass<"Teak", [i40], 32, (add B1)>;
def ABRegs : RegisterClass<"Teak", [i40], 32, (add A0, A1, B0, B1)>;
def ABLRegs : RegisterClass<"Teak", [i16], 16, (add A0L, A1L, B0L, B1L)>;
def ALRegs : RegisterClass<"Teak", [i16], 16, (add A0L, A1L)>;
// {
//     let CopyCost = -1;
// }
def ABHRegs : RegisterClass<"Teak", [i16], 16, (add A0H, A1H, B0H, B1H)>;
def ArArpSttMod : RegisterClass<"Teak", [i16], 16, (add STT0)>;
def FP : RegisterClass<"Teak", [i16], 16, (add R7)>;
def Y0Regs : RegisterClass<"Teak", [i16], 16, (add Y0)>;
def P0Regs : RegisterClass<"Teak", [i32], 32, (add P0)>;
def PRegs : RegisterClass<"Teak", [i32], 32, (add P0, P1)>;
def SVReg : RegisterClass<"Teak", [i16], 16, (add SV)>;
//def XRegs : RegisterClass<"Teak", [i16], 16, (add X0, X1)>;
//def YRegs : RegisterClass<"Teak", [i16], 16, (add Y0, Y1)>;
//def PRegs : RegisterClass<"Teak", [i32], 32, (add P0, P1)>;

def SttModRegs : RegisterClass<"Teak", [i16], 16, (add STT0, STT1, STT2, MOD0, MOD1, MOD2, MOD3)>;