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

def EXT0 : TeakReg<0, "ext0">;
def EXT1 : TeakReg<0, "ext1">;
def EXT2 : TeakReg<0, "ext2">;
def EXT3 : TeakReg<0, "ext3">;

def ICC : TeakReg<0, "ICC">;//for flag setting ops, maybe rename

// Page registers: 80 fake registers that are actually in memory
// Since memory is as fast as the cpu, there's no performance difference
// between using those fake registers or real registers using the short
// 8 bit page addressing mode, given that an instruction supports this ofc

// The first 40 page registers are preserved over function calls
def PAGE0 : TeakReg<0, "[page:0x0000u8]">;
def PAGE1 : TeakReg<0, "[page:0x0001u8]">;
def PAGE2 : TeakReg<0, "[page:0x0002u8]">;
def PAGE3 : TeakReg<0, "[page:0x0003u8]">;
def PAGE4 : TeakReg<0, "[page:0x0004u8]">;
def PAGE5 : TeakReg<0, "[page:0x0005u8]">;
def PAGE6 : TeakReg<0, "[page:0x0006u8]">;
def PAGE7 : TeakReg<0, "[page:0x0007u8]">;
def PAGE8 : TeakReg<0, "[page:0x0008u8]">;
def PAGE9 : TeakReg<0, "[page:0x0009u8]">;
def PAGE10 : TeakReg<0, "[page:0x000Au8]">;
def PAGE11 : TeakReg<0, "[page:0x000Bu8]">;
def PAGE12 : TeakReg<0, "[page:0x000Cu8]">;
def PAGE13 : TeakReg<0, "[page:0x000Du8]">;
def PAGE14 : TeakReg<0, "[page:0x000Eu8]">;
def PAGE15 : TeakReg<0, "[page:0x000Fu8]">;
def PAGE16 : TeakReg<0, "[page:0x0010u8]">;
def PAGE17 : TeakReg<0, "[page:0x0011u8]">;
def PAGE18 : TeakReg<0, "[page:0x0012u8]">;
def PAGE19 : TeakReg<0, "[page:0x0013u8]">;
def PAGE20 : TeakReg<0, "[page:0x0014u8]">;
def PAGE21 : TeakReg<0, "[page:0x0015u8]">;
def PAGE22 : TeakReg<0, "[page:0x0016u8]">;
def PAGE23 : TeakReg<0, "[page:0x0017u8]">;
def PAGE24 : TeakReg<0, "[page:0x0018u8]">;
def PAGE25 : TeakReg<0, "[page:0x0019u8]">;
def PAGE26 : TeakReg<0, "[page:0x001Au8]">;
def PAGE27 : TeakReg<0, "[page:0x001Bu8]">;
def PAGE28 : TeakReg<0, "[page:0x001Cu8]">;
def PAGE29 : TeakReg<0, "[page:0x001Du8]">;
def PAGE30 : TeakReg<0, "[page:0x001Eu8]">;
def PAGE31 : TeakReg<0, "[page:0x001Fu8]">;
def PAGE32 : TeakReg<0, "[page:0x0020u8]">;
def PAGE33 : TeakReg<0, "[page:0x0021u8]">;
def PAGE34 : TeakReg<0, "[page:0x0022u8]">;
def PAGE35 : TeakReg<0, "[page:0x0023u8]">;
def PAGE36 : TeakReg<0, "[page:0x0024u8]">;
def PAGE37 : TeakReg<0, "[page:0x0025u8]">;
def PAGE38 : TeakReg<0, "[page:0x0026u8]">;
def PAGE39 : TeakReg<0, "[page:0x0027u8]">;

// The second 40 page registers are not preserved
def PAGE40 : TeakReg<0, "[page:0x0028u8]">;
def PAGE41 : TeakReg<0, "[page:0x0029u8]">;
def PAGE42 : TeakReg<0, "[page:0x002Au8]">;
def PAGE43 : TeakReg<0, "[page:0x002Bu8]">;
def PAGE44 : TeakReg<0, "[page:0x002Cu8]">;
def PAGE45 : TeakReg<0, "[page:0x002Du8]">;
def PAGE46 : TeakReg<0, "[page:0x002Eu8]">;
def PAGE47 : TeakReg<0, "[page:0x002Fu8]">;
def PAGE48 : TeakReg<0, "[page:0x0030u8]">;
def PAGE49 : TeakReg<0, "[page:0x0031u8]">;
def PAGE50 : TeakReg<0, "[page:0x0032u8]">;
def PAGE51 : TeakReg<0, "[page:0x0033u8]">;
def PAGE52 : TeakReg<0, "[page:0x0034u8]">;
def PAGE53 : TeakReg<0, "[page:0x0035u8]">;
def PAGE54 : TeakReg<0, "[page:0x0036u8]">;
def PAGE55 : TeakReg<0, "[page:0x0037u8]">;
def PAGE56 : TeakReg<0, "[page:0x0038u8]">;
def PAGE57 : TeakReg<0, "[page:0x0039u8]">;
def PAGE58 : TeakReg<0, "[page:0x003Au8]">;
def PAGE59 : TeakReg<0, "[page:0x003Bu8]">;
def PAGE60 : TeakReg<0, "[page:0x003Cu8]">;
def PAGE61 : TeakReg<0, "[page:0x003Du8]">;
def PAGE62 : TeakReg<0, "[page:0x003Eu8]">;
def PAGE63 : TeakReg<0, "[page:0x003Fu8]">;
def PAGE64 : TeakReg<0, "[page:0x0040u8]">;
def PAGE65 : TeakReg<0, "[page:0x0041u8]">;
def PAGE66 : TeakReg<0, "[page:0x0042u8]">;
def PAGE67 : TeakReg<0, "[page:0x0043u8]">;
def PAGE68 : TeakReg<0, "[page:0x0044u8]">;
def PAGE69 : TeakReg<0, "[page:0x0045u8]">;
def PAGE70 : TeakReg<0, "[page:0x0046u8]">;
def PAGE71 : TeakReg<0, "[page:0x0047u8]">;
def PAGE72 : TeakReg<0, "[page:0x0048u8]">;
def PAGE73 : TeakReg<0, "[page:0x0049u8]">;
def PAGE74 : TeakReg<0, "[page:0x004Au8]">;
def PAGE75 : TeakReg<0, "[page:0x004Bu8]">;
def PAGE76 : TeakReg<0, "[page:0x004Cu8]">;
def PAGE77 : TeakReg<0, "[page:0x004Du8]">;
def PAGE78 : TeakReg<0, "[page:0x004Eu8]">;
def PAGE79 : TeakReg<0, "[page:0x004Fu8]">;

// Register classes.
//
def GRRegs : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7)>;
def RegNoBRegs40 : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
def RegNoBRegs16 : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, B0H, B1H, B0L, B1L, A0L, A1L, A0H, A1H, LC, SV, EXT0, EXT1, EXT2, EXT3)>;
def RegNoBRegs16_nolh : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, LC, SV, EXT0, EXT1, EXT2, EXT3)>;
def RegNoBRegs16_noh : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, A0L, A1L, B0L, B1L, LC, SV, EXT0, EXT1, EXT2, EXT3)>;
def RegNoBRegs16_page : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, B0H, B1H, B0L, B1L, A0L, A1L, A0H, A1H, LC, SV, EXT0, EXT1, EXT2, EXT3, (sequence "PAGE%u", 0, 79))>;
def RegNoBRegs16_noh_page : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R6, R7, Y0, SP, A0L, A1L, B0L, B1L, LC, SV, EXT0, EXT1, EXT2, EXT3, (sequence "PAGE%u", 0, 79))>;
def RegR0425 : RegisterClass<"Teak", [i16], 16, (add R0, R4, R2, R5)>;
//def RegNoBRegsP040 : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
//def RegNoBRegsP016 : RegisterClass<"Teak", [i16], 16, (add R0, R1, R2, R3, R4, R5, R7, Y0, /**/ SP, B0H, B1H, B0L, B1L, A0L, A1L, A0H, A1H, LC, SV)>;
def ARegs : RegisterClass<"Teak", [i40], 32, (add A0, A1)>;
def BRegs : RegisterClass<"Teak", [i40], 32, (add B0, B1)>;
def B0Reg : RegisterClass<"Teak", [i40], 32, (add B0)>;
def B1Reg : RegisterClass<"Teak", [i40], 32, (add B1)>;
def ABRegs : RegisterClass<"Teak", [i40], 32, (add A0, A1, B0, B1)>;
let CopyCost = -1 in
{
    def ABLRegs : RegisterClass<"Teak", [i16], 16, (add A0L, A1L, B0L, B1L)>;
    def ALRegs : RegisterClass<"Teak", [i16], 16, (add A0L, A1L)>;
    def ABHRegs : RegisterClass<"Teak", [i16], 16, (add A0H, A1H, B0H, B1H)>;
    def ABERegs : RegisterClass<"Teak", [i8], 16, (add A0E, A1E, B0E, B1E)>;
}
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

def MovImm8sRegs : RegisterClass<"Teak", [i16], 16, (add A0H, A1H, EXT0, EXT1, EXT2, EXT3, R0, R1, R2, R3, R4, R5, R7, Y0, SV)>;

def PAGERegs : RegisterClass<"Teak", [i16], 16, (add (sequence "PAGE%u", 0, 79))>;
def PAGESavedRegs : RegisterClass<"Teak", [i16], 16, (add (sequence "PAGE%u", 0, 39))>;
def PAGETrashedRegs : RegisterClass<"Teak", [i16], 16, (add (sequence "PAGE%u", 40, 79))>;