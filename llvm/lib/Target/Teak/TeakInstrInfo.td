include "TeakInstrFormats.td"
include "TeakOperators.td"

//def : Pattern<(i32 (load_sym tglobaladdr:$addr)),  [(MOVi32 $addr)]>;

// def MOVi32 : InstTeak<(outs BRegs:$dst), (ins i32imm:$src), "",
//                      [(set i32:$dst, (movei32 imm:$src))]> {
//   let isPseudo = 1;
// }

// Immediate operands with a shared generic asm render method.
class ImmAsmOperand<int Low, int High> : AsmOperandClass {
  let RenderMethod = "addImmOperands";
  let PredicateMethod = "isImmediate<" # Low # "," # High # ">";
  let DiagnosticString = "operand must be an immediate in the range [" # Low # "," # High # "]";
}

def Imm0_31AsmOperand: ImmAsmOperand<0,31> { let Name = "Imm0_31"; }
def imm0_31 : Operand<i40>, ImmLeaf<i40, [{
  return Imm >= 0 && Imm <= 31;
}]> {
  let ParserMatchClass = Imm0_31AsmOperand;
}

def Imm6sAsmOperand: ImmAsmOperand<-32,31> { let Name = "Imm6s"; }
def Imm6s : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= -32 && Imm <= 31;
}]> {
  let ParserMatchClass = Imm6sAsmOperand;
}

def Imm7sAsmOperand: ImmAsmOperand<-64,63> { let Name = "Imm7s"; }
def Imm7s : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= -64 && Imm <= 63;
}]> {
  let ParserMatchClass = Imm7sAsmOperand;
}

def Imm8uAsmOperand: ImmAsmOperand<0,255> { let Name = "Imm8u"; }
def Imm8u : Operand<i40>, ImmLeaf<i40, [{
  return Imm >= 0 && Imm <= 255;
}]> {
  let ParserMatchClass = Imm8uAsmOperand;
}

def Imm8u_16AsmOperand: ImmAsmOperand<0,255> { let Name = "Imm8u_16"; }
def Imm8u_16 : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= 0 && Imm <= 255;
}]> {
  let ParserMatchClass = Imm8u_16AsmOperand;
}

def Imm0_65535AsmOperand: ImmAsmOperand<0,65535> { let Name = "Imm0_65535"; }
def imm0_65535 : Operand<i40>, ImmLeaf<i40, [{
  return Imm >= 0 && Imm < 65536;
}]> {
  let ParserMatchClass = Imm0_65535AsmOperand;
}

def Imm0_65535AsmOperand_16: ImmAsmOperand<0,65535> { let Name = "Imm0_65535_16"; }
def imm0_65535_16 : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= 0 && Imm < 65536;
}]> {
  let ParserMatchClass = Imm0_65535AsmOperand_16;
}

def ImmNeg32768_32767AsmOperand: ImmAsmOperand<-32768,32767> { let Name = "ImmNeg32768_32767"; }
def immNeg32768_32767 : Operand<i40>, ImmLeaf<i40, [{
  return Imm >= -32768 && Imm <= 32767;
}]> {
  let ParserMatchClass = ImmNeg32768_32767AsmOperand;
}

def ImmNeg32768_32767AsmOperand_16: ImmAsmOperand<-32768,32767> { let Name = "ImmNeg32768_32767_16"; }
def immNeg32768_32767_16 : Operand<i16>, ImmLeaf<i16, [{
  return Imm >= -32768 && Imm <= 32767;
}]> {
  let ParserMatchClass = ImmNeg32768_32767AsmOperand_16;
}

def Imm0_262143AsmOperand: ImmAsmOperand<0,262143> { let Name = "Imm0_262143"; }
def imm0_262143 : Operand<i32>, ImmLeaf<i32, [{
  return Imm >= 0 && Imm < 262143;
}]> {
  let ParserMatchClass = Imm0_262143AsmOperand;
}

def imm16_not_XFORM : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((uint16_t)~N->getZExtValue(), SDLoc(N), MVT::i16);
}]>;

//def ImmNot16AsmOperand_16: ImmAsmOperand<0,65535> { let Name = "Imm0_65535_16"; }
def immNot16_16 : Operand<i16>, ImmLeaf<i16, [{
  return 1;//Imm >= 0 && Imm < 65536;
}], imm16_not_XFORM>;

//def ImmNot16AsmOperand_16: ImmAsmOperand<0,65535> { let Name = "Imm0_65535_16"; }
def immNot16 : Operand<i40>, ImmLeaf<i40, [{
  return ((~Imm) & 0xFFFFFF0000) == 0;// (~Imm) >= 0 && (~Imm) < 65536;
}], imm16_not_XFORM>;

def load_postinc : PatFrag<(ops node:$base),
                          (ld node:$base), [{
  const LoadSDNode* ld = cast<LoadSDNode>(N);
	ISD::MemIndexedMode idxMode = ld->getAddressingMode();
  if (idxMode != ISD::POST_INC && idxMode != ISD::POST_DEC)
      return false;
  int offs = cast<ConstantSDNode>(ld->getOffset())->getSExtValue();
  return (idxMode == ISD::POST_INC && offs == 1) || (idxMode == ISD::POST_DEC && offs == -1);
}]>{
    let IsLoad = 1;
    let IsUnindexed = 0;
    let IsNonExtLoad = 1;
}

def load_postdec : PatFrag<(ops node:$base),
                          (ld node:$base), [{
  const LoadSDNode* ld = cast<LoadSDNode>(N);
	ISD::MemIndexedMode idxMode = ld->getAddressingMode();
  if (idxMode != ISD::POST_INC && idxMode != ISD::POST_DEC)
      return false;
  int offs = cast<ConstantSDNode>(ld->getOffset())->getSExtValue();
  return (idxMode == ISD::POST_INC && offs == -1) || (idxMode == ISD::POST_DEC && offs == 1);
}]>{
    let IsLoad = 1;
    let IsUnindexed = 0;
    let IsNonExtLoad = 1;
}

def pred : PredicateOperand<OtherVT, (ops i32imm, i32imm), (ops (i32 0), (i32 zero_reg))>
{
    let PrintMethod = "printCondCode";
}

// // sext_inreg
// def : Pat<(sext_inreg i16:$src, i8),
//           (SEXT (i8 (EXTRACT_SUBREG i16:$src, sub_lo)))>;

// let Defs = [ICC], isCommutable = 1 in
// def ADDV_imm16_memrn : InstTeakImm16<
//     (outs ),
//     (ins immNeg32768_32767_16:$val, GRRegs:$addr),
//     "addv $val, [$addr]",
//     [(store (add (load GRRegs:$addr), immNeg32768_32767_16:$val), GRRegs:$addr)]>;


let Defs = [ICC] in
{
    let isCommutable = 1, mayStore = 1, mayLoad = 1 in
    def ADDV_imm16_memrn : InstTeakImm16<(outs), (ins immNeg32768_32767_16:$val, GRRegs:$addr), "addv $val, [$addr]", [(store (add (i16 (load GRRegs:$addr)), immNeg32768_32767_16:$val), GRRegs:$addr)]>;

    let mayStore = 1, mayLoad = 1 in
    def SUBV_imm16_memrn : InstTeakImm16<(outs), (ins immNeg32768_32767_16:$val, GRRegs:$addr), "subv $val, [$addr]", [(store (sub (i16 (load GRRegs:$addr)), immNeg32768_32767_16:$val), GRRegs:$addr)]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def ADDV_imm16_RegNoBRegs16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins immNeg32768_32767_16:$val, RegNoBRegs16:$a), "addv $val, $a", [(set RegNoBRegs16:$dst, (add RegNoBRegs16:$a, immNeg32768_32767_16:$val))]>;

    let Constraints = "$dst = $a" in
    def SUBV_imm16_RegNoBRegs16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins immNeg32768_32767_16:$val, RegNoBRegs16:$a), "subv $val, $a", [(set RegNoBRegs16:$dst, (sub RegNoBRegs16:$a, immNeg32768_32767_16:$val))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def INC_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$a, pred:$p), "inc $a, $p", [(set ARegs:$dst, (add ARegs:$a, 1))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def DEC_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$a, pred:$p), "dec $a, $p", [(set ARegs:$dst, (add ARegs:$a, -1))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def RND_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$a, pred:$p), "rnd $a, $p", [(set ARegs:$dst, (add ARegs:$a, 0x8000))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def ADD_imm8u_a : InstTeak<(outs ARegs:$dst), (ins Imm8u:$val, ARegs:$a), "add ${val}u8, $a", [(set ARegs:$dst, (add ARegs:$a, Imm8u:$val))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def ADD_imm16_a : InstTeakImm16<(outs ARegs:$dst), (ins immNeg32768_32767:$val, ARegs:$a), "add $val, $a", [(set ARegs:$dst, (add ARegs:$a, immNeg32768_32767:$val))]>;

    let Constraints = "$dst = $a", isCommutable = 1, mayLoad = 1 in
    def ADD_memimm16_a : InstTeakImm16<(outs ARegs:$dst), (ins Operand<i16>:$addr, ARegs:$a), "add [$addr], $a", [(set ARegs:$dst, (add ARegs:$a, (sextloadi16 imm:$addr)))]>;

    let Constraints = "$dst = $a", isCommutable = 1, mayLoad = 1 in
    def ADD_memrn_a : InstTeak<(outs ARegs:$dst), (ins GRRegs:$addr, ARegs:$a), "add [$addr], $a", [(set ARegs:$dst, (add ARegs:$a, (sextloadi16 GRRegs:$addr)))]>;

    let Constraints = "$dst = $a", mayLoad = 1 in
    def ADDL_memrn_a : InstTeak<(outs ARegs:$dst), (ins GRRegs:$addr, ARegs:$a), "addl [$addr], $a", [(set ARegs:$dst, (add ARegs:$a, (zextloadi16 GRRegs:$addr)))]>;

    let Constraints = "$dst = $a" in
    def ADD_regnobp016_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$nob, ARegs:$a), "add $nob, $a", [(set ARegs:$dst, (add ARegs:$a, (sext RegNoBRegs16:$nob)))]>;

    let Constraints = "$dst = $a" in
    def ADDL_regnob16_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$nob, ARegs:$a), "addl $nob, $a", [(set ARegs:$dst, (add ARegs:$a, (zext RegNoBRegs16:$nob)))]>;

    let Constraints = "$dst = $a", isCommutable = 1 in
    def ADD_ab_ab : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$b, ABRegs:$a), "add $b, $a", [(set ABRegs:$dst, (add ABRegs:$a, ABRegs:$b))]>;

    //this does not give the expected result, it is actually
    //a = a & (0xFF00 | val)
    // let Defs = [ICC], Constraints = "$dst = $a" in
    // def AND_imm8_a : InstTeak<
    //     (outs ARegs:$dst),
    //     (ins Imm8u:$val, ARegs:$a),
    //     "and $val, $a",
    //     [(set ARegs:$dst, (and ARegs:$a, Imm8u:$val))]>;

    let mayStore = 1, mayLoad = 1 in
    def RST_imm16_memrn : InstTeakImm16<(outs), (ins immNot16_16:$val, GRRegs:$addr), "rst $val, [$addr]", [(store (TeakAnd (i16 (load GRRegs:$addr)), immNot16_16:$val), GRRegs:$addr)]>;

    let mayStore = 1, mayLoad = 1 in
    def SET_imm16_memrn : InstTeakImm16<(outs), (ins i16imm:$val, GRRegs:$addr), "set $val, [$addr]", [(store (TeakOr (i16 (load GRRegs:$addr)), imm:$val), GRRegs:$addr)]>;

    let mayStore = 1, mayLoad = 1 in
    def CHNG_imm16_memrn : InstTeakImm16<(outs), (ins i16imm:$val, GRRegs:$addr), "chng $val, [$addr]", [(store (TeakXor (i16 (load GRRegs:$addr)), imm:$val), GRRegs:$addr)]>;

    let Constraints = "$dst = $a" in
    {
        def AND_imm16_a : InstTeakImm16<(outs ARegs:$dst), (ins imm0_65535:$val, ARegs:$a), "and $val, $a", [(set ARegs:$dst, (TeakAnd ARegs:$a, imm0_65535:$val))]>;
        def AND_regnobp016_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$b, ARegs:$a), "and $b, $a", [(set ARegs:$dst, (TeakAnd ARegs:$a, (zext RegNoBRegs16:$b)))]>;

        def OR_imm8u_a : InstTeak<(outs ARegs:$dst), (ins Imm8u:$val, ARegs:$a), "or ${val}u8, $a", [(set ARegs:$dst, (TeakOr ARegs:$a, Imm8u:$val))]>;
        def OR_imm16_a : InstTeakImm16<(outs ARegs:$dst), (ins imm0_65535:$val, ARegs:$a), "or $val, $a", [(set ARegs:$dst, (TeakOr ARegs:$a, imm0_65535:$val))]>;
        def OR_regnobp016_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$b, ARegs:$a), "or $b, $a", [(set ARegs:$dst, (TeakOr ARegs:$a, (zext RegNoBRegs16:$b)))]>;
        //def OR_regnobp016_al : InstTeak<(outs ALRegs:$dst), (ins RegNoBRegs16:$b, ALRegs:$a), "or $b, $a", [(set ALRegs:$dst, (TeakOr ALRegs:$a, RegNoBRegs16:$b))]>;

        let mayLoad = 1 in
        {
            def AND_memimm16_a : InstTeakImm16<(outs ARegs:$dst), (ins Operand<i16>:$addr, ARegs:$a), "and [$addr], $a", [(set ARegs:$dst, (TeakAnd ARegs:$a, (zextloadi16 imm:$addr)))]>;
            def AND_memrn_a : InstTeak<(outs ARegs:$dst), (ins GRRegs:$addr, ARegs:$a), "and [$addr], $a", [(set ARegs:$dst, (TeakAnd ARegs:$a, (zextloadi16 GRRegs:$addr)))]>;

            def OR_memimm16_a : InstTeakImm16<(outs ARegs:$dst), (ins Operand<i16>:$addr, ARegs:$a), "or [$addr], $a", [(set ARegs:$dst, (TeakOr ARegs:$a, (zextloadi16 imm:$addr)))]>;
            def OR_memrn_a : InstTeak<(outs ARegs:$dst), (ins GRRegs:$addr, ARegs:$a), "or [$addr], $a", [(set ARegs:$dst, (TeakOr ARegs:$a, (zextloadi16 GRRegs:$addr)))]>;

            def XOR_memimm16_a : InstTeakImm16<(outs ARegs:$dst), (ins Operand<i16>:$addr, ARegs:$a), "xor [$addr], $a", [(set ARegs:$dst, (TeakXor ARegs:$a, (zextloadi16 imm:$addr)))]>;
            def XOR_memrn_a : InstTeak<(outs ARegs:$dst), (ins GRRegs:$addr, ARegs:$a), "xor [$addr], $a", [(set ARegs:$dst, (TeakXor ARegs:$a, (zextloadi16 GRRegs:$addr)))]>;
        }

        def RST_imm16_regnob16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins immNot16_16:$val, RegNoBRegs16:$a), "rst $val, $a", [(set RegNoBRegs16:$dst, (TeakAnd RegNoBRegs16:$a, immNot16_16:$val))]>;
        def RST_imm16_abl : InstTeakImm16<(outs ABRegs:$dst), (ins immNot16:$val, ABRegs:$a), "rst $val, ${a}l", [(set ABRegs:$dst, (TeakAnd ABRegs:$a, immNot16:$val))]>;
        def RST_imm16_sttmod : InstTeakImm16<(outs SttModRegs:$dst), (ins imm0_65535_16:$val, SttModRegs:$a), "rst $val, $a", []>;

        def SET_imm16_regnob16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins i16imm:$val, RegNoBRegs16:$a), "set $val, $a", [(set RegNoBRegs16:$dst, (TeakOr RegNoBRegs16:$a, imm:$val))]>;
        def SET_imm16_abl : InstTeakImm16<(outs ABRegs:$dst), (ins imm0_65535:$val, ABRegs:$a), "set $val, ${a}l", [(set ABRegs:$dst, (TeakOr ABRegs:$a, imm0_65535:$val))]>;
        def SET_imm16_sttmod : InstTeakImm16<(outs SttModRegs:$dst), (ins imm0_65535_16:$val, SttModRegs:$a), "set $val, $a", []>;

        def CHNG_imm16_regnob16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins i16imm:$val, RegNoBRegs16:$a), "chng $val, $a", [(set RegNoBRegs16:$dst, (TeakXor RegNoBRegs16:$a, imm:$val))]>;
        def CHNG_imm16_abl : InstTeakImm16<(outs ABRegs:$dst), (ins imm0_65535:$val, ABRegs:$a), "chng $val, ${a}l", [(set ABRegs:$dst, (TeakXor ABRegs:$a, imm0_65535:$val))]>;
        def CHNG_imm16_sttmod : InstTeakImm16<(outs SttModRegs:$dst), (ins imm0_65535_16:$val, SttModRegs:$a), "chng $val, $a", []>;

        let mayLoad = 1 in
        def OR_r7offset7s_a : InstTeak<(outs ARegs:$dst), (ins memsrc:$offset, ARegs:$a), "or $offset, $a", []>;

        let mayLoad = 1 in
        def OR_r7offset16_a : InstTeakImm16<(outs ARegs:$dst), (ins memsrc:$offset, ARegs:$a), "or $offset, $a", []>;

        //def SUBV_imm16_RegNoBRegs16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins immNeg32768_32767_16:$val, RegNoBRegs16:$a), "subv $val, $a", [(set RegNoBRegs16:$dst, (sub RegNoBRegs16:$a, immNeg32768_32767_16:$val))]>;
        def SUB_imm8u_a : InstTeak<(outs ARegs:$dst), (ins Imm8u:$val, ARegs:$a), "sub ${val}u8, $a", [(set ARegs:$dst, (sub ARegs:$a, Imm8u:$val))]>;
        def SUB_imm16_a : InstTeakImm16<(outs ARegs:$dst), (ins immNeg32768_32767:$val, ARegs:$a), "sub $val, $a", [(set ARegs:$dst, (sub ARegs:$a, immNeg32768_32767:$val))]>;
        def SUB_regnobp016_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$nob, ARegs:$a), "sub $nob, $a", [(set ARegs:$dst, (sub ARegs:$a, (sext RegNoBRegs16:$nob)))]>;
        def SUBL_regnob16_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$nob, ARegs:$a), "subl $nob, $a", [(set ARegs:$dst, (sub ARegs:$a, (zext RegNoBRegs16:$nob)))]>;
        def SUB_ab_ab : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$b, ABRegs:$a), "sub $b, $a", [(set ABRegs:$dst, (sub ABRegs:$a, ABRegs:$b))]>;

        def NEG_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$a, pred:$p), "neg $a, $p", [(set ARegs:$dst, (ineg ARegs:$a))]>;
        def NOT_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$a, pred:$p), "not $a, $p", [(set ARegs:$dst, (not ARegs:$a))]>;

        def XOR_imm8u_a : InstTeak<(outs ARegs:$dst), (ins Imm8u:$val, ARegs:$a), "xor ${val}u8, $a", [(set ARegs:$dst, (TeakXor ARegs:$a, Imm8u:$val))]>;
        def XOR_imm16_a : InstTeakImm16<(outs ARegs:$dst), (ins imm0_65535:$val, ARegs:$a), "xor $val, $a", [(set ARegs:$dst, (TeakXor ARegs:$a, imm0_65535:$val))]>;
        def XOR_regnobp016_a : InstTeak<(outs ARegs:$dst), (ins RegNoBRegs16:$b, ARegs:$a), "xor $b, $a", [(set ARegs:$dst, (TeakXor ARegs:$a, (zext RegNoBRegs16:$b)))]>;
        // def XOR_regnobp016_al : InstTeak<(outs ALRegs:$dst), (ins RegNoBRegs16:$b, ALRegs:$a), "xor $b, $a", [(set ALRegs:$dst, (TeakXor ALRegs:$a, RegNoBRegs16:$b))]>;
        def XOR_a_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$b, ARegs:$a), "xor $b, $a", [(set ARegs:$dst, (TeakXor ARegs:$a, ARegs:$b))]>;
    }

    def AND_ab_ab_a : InstTeak<(outs ARegs:$dst), (ins ABRegs:$b, ABRegs:$a), "and $b, $a, $dst", [(set ARegs:$dst, (TeakAnd ABRegs:$a, ABRegs:$b))]>;
    def OR_ab_ab_a  : InstTeak<(outs ARegs:$dst), (ins ABRegs:$b, ABRegs:$a), "or $b, $a, $dst",  [(set ARegs:$dst, (TeakOr  ABRegs:$a, ABRegs:$b))]>;

    let isMoveImm = 1 in
    {        
        def CLR_ab : InstTeak<(outs ABRegs:$dst), (ins pred:$p), "clr $dst, $p", [(set ABRegs:$dst, 0)]>;
        def CLRR_ab : InstTeak<(outs ABRegs:$dst), (ins pred:$p), "clrr $dst, $p", [(set ABRegs:$dst, 0x8000)]>;
        def MOV_imm16_regnob16 : InstTeakImm16<(outs RegNoBRegs16:$dst), (ins i16imm:$val), "mov $val, $dst", [(set RegNoBRegs16:$dst, imm:$val)]>;
        def MOV_imm16neg_ab : InstTeakImm16<(outs ABRegs:$dst), (ins immNeg32768_32767:$val), "mov $val, $dst", [(set ABRegs:$dst, immNeg32768_32767:$val)]>;
        def MOV_imm16_ab : InstTeakImm16<(outs ABRegs:$dst), (ins imm0_65535:$val), "mov $val, ${dst}l", [(set ABRegs:$dst, imm0_65535:$val)]>;
        def MOV_imm16hi_ab : InstTeakImm16<(outs ABRegs:$dst), (ins imm0_65535:$val), "mov $val, ${dst}h", []>;
    }

    let isMoveReg = 1 in
    {
        let Constraints = "$dst != $src" in
        def COPY_a : InstTeak<(outs ARegs:$dst), (ins ARegs:$src, pred:$p), "copy $dst, $p", []>;
        def MOV_ab_ab : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$src), "mov $src, $dst", [/*(set ABRegs:$dst, ABRegs:$src)*/]>;
        def MOV_regnobp016_regnob16 : InstTeak<(outs RegNoBRegs16:$dst), (ins RegNoBRegs16:$src), "mov $src, $dst", [/*(set RegNoBRegs16:$dst, RegNoBRegs16:$src)*/]>;
        def MOV_p0_regnob16 : InstTeak<(outs RegNoBRegs16:$dst), (ins P0Regs:$src), "mov $src, $dst", [/*(set RegNoBRegs16:$dst, RegNoBRegs16:$src)*/]>;
        def MOV_regnobp016_ab : InstTeak<(outs ABRegs:$dst), (ins RegNoBRegs16:$src), "mov $src, $dst", [(set ABRegs:$dst, (sext RegNoBRegs16:$src))]>;
        def MOV_p0_ab : InstTeak<(outs ABRegs:$dst), (ins P0Regs:$src), "mov $src, $dst", [/*(set ABRegs:$dst, (sext P0Regs:$src))*/]>;
        def MOV_regnobp016_abl : InstTeak<(outs ABRegs:$dst), (ins RegNoBRegs16:$src), "mov $src, ${dst}l", [(set ABRegs:$dst, (zext RegNoBRegs16:$src))]>;
    }

    def SHFI_arith_ab_ab : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$src, Imm6s:$shift), "shfi $src, $dst, $shift", [(set ABRegs:$dst, (TeakShiftArith ABRegs:$src, Imm6s:$shift))]>;
    def SHFI_logic_ab_ab : TeakPseudoInst<(outs ABRegs:$dst), (ins ABRegs:$src, Imm6s:$shift), "shfi $src, $dst, $shift", [(set ABRegs:$dst, (TeakShiftLogic ABRegs:$src, Imm6s:$shift))]>;
    def SHFC_arith_ab_ab_sv : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$src, SVReg:$shift, pred:$p), "shfc $src, $dst, $p", [(set ABRegs:$dst, (TeakShiftArith ABRegs:$src, SVReg:$shift))]>;
    def SHFC_logic_ab_ab_sv : TeakPseudoInst<(outs ABRegs:$dst), (ins ABRegs:$src, SVReg:$shift, pred:$p), "shfc $src, $dst, $p", [(set ABRegs:$dst, (TeakShiftLogic ABRegs:$src, SVReg:$shift))]>;

    def MPY_y0_regnob16 : InstTeak<(outs P0Regs:$p), (ins Y0Regs:$y, RegNoBRegs16:$x), "mpy $y, $x", []>;

    let Defs = [ICC, P0] in
    def MPY_y0_regnob16_RegNoBRegs16 : TeakPseudoInst<(outs RegNoBRegs16:$dst), (ins Y0Regs:$y, RegNoBRegs16:$x), "mpy $y, $x", [(set RegNoBRegs16:$dst, (mul RegNoBRegs16:$x, Y0Regs:$y))]>;

    let Constraints = "$asrc = $bdst, $bsrc = $adst" in
    def SWAP_ab : InstTeak<(outs BRegs:$adst, ARegs:$bdst), (ins ARegs:$asrc, BRegs:$bsrc), "swap ($asrc, $bsrc)", []>;
}

// def PseudoSRA : InstTeak<(outs ABRegs:$dst), (ins ABRegs:$src, Operand<i32>:$shift), "",
//                      [(set i40:$dst, ( imm:$src))]> {
//   let isPseudo = 1;
// }

//def : Pat<(i16 (add RegNoBRegs16:$a, RegNoBRegs16:$b)), (EXTRACT_SUBREG (ADD_regnobp016_a RegNoBRegs16:$a, (MOV_regnobp016_ab RegNoBRegs16:$b)), sub_16bit)>;

def LO16 : SDNodeXForm<imm,
[{
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0xFFFF, SDLoc(N), MVT::i40);
}]>;

def HI16 : SDNodeXForm<imm,
[{
    return CurDAG->getTargetConstant((unsigned)N->getZExtValue() >> 16, SDLoc(N), MVT::i40);
}]>;

// Arbitrary immediates.
def : Pat<(i40 imm:$val), (SET_imm16_abl (LO16 imm:$val), (MOV_imm16hi_ab (HI16 imm:$val)))>;

let mayStore = 1 in
def MOV_al2_memimm16 : InstTeakImm16<(outs), (ins ALRegs:$a, Operand<i16>:$dstAddr), "mov ${a}, [$dstAddr]", [(store ALRegs:$a, imm:$dstAddr)]>;

let mayStore = 1 in
def MOV_al_memimm16 : InstTeakImm16<(outs), (ins ARegs:$a, Operand<i16>:$dstAddr), "mov ${a}l, [$dstAddr]", [(truncstorei16 ARegs:$a, imm:$dstAddr)]>;

// let mayStore = 1 in
// def MOV_al_memimm16 : InstTeak<
//     (outs),
//     (ins ALRegs:$a, Operand<i16>:$dstAddr),
//     "mov $a, [$dstAddr]",
//     [(store ALRegs:$a, tglobaladdr:$dstAddr)]>;

let mayLoad = 1, Defs = [ICC] in
def MOV_memimm16_a : InstTeakImm16<(outs ARegs:$dst), (ins Operand<i16>:$srcAddr), "mov [$srcAddr], $dst", [(set ARegs:$dst, (sextloadi16 imm:$srcAddr))]>;

let mayStore = 1 in
def MOV_regnob16_memrn : InstTeak<(outs), (ins RegNoBRegs16:$a, GRRegs:$dstAddr), "mov $a, [$dstAddr]", [(store RegNoBRegs16:$a, GRRegs:$dstAddr)]>;

let mayStore = 1 in
def MOV_abl_memrn : InstTeak<(outs), (ins ABRegs:$a, GRRegs:$dstAddr), "mov ${a}l, [$dstAddr]", [(truncstorei16 ABRegs:$a, GRRegs:$dstAddr)]>;

let Defs = [ICC], mayLoad = 1 in
def MOV_memrn_regnob16 : InstTeak<(outs RegNoBRegs16_nolh:$dst), (ins GRRegs:$srcAddr), "mov [$srcAddr], $dst", [(set RegNoBRegs16_nolh:$dst, (load GRRegs:$srcAddr))]>;

// let Defs = [ICC], mayLoad = 1, Constraints = "$srcAddr = $srcAddrOut" in
// def MOV_memrn_regnob16_postinc : InstTeak<(outs RegNoBRegs16_nolh:$dst, GRRegs:$srcAddrOut), (ins GRRegs:$srcAddr), "mov [${srcAddr}++], $dst", [(set RegNoBRegs16_nolh:$dst, GRRegs:$srcAddrOut, (load_postinc GRRegs:$srcAddr))]>;

let Defs = [ICC], mayLoad = 1, Constraints = "$srcAddr = $srcAddrOut" in
def MOV_memrn_regnob16_postdec : InstTeak<(outs RegNoBRegs16_nolh:$dst, GRRegs:$srcAddrOut), (ins GRRegs:$srcAddr), "mov [${srcAddr}--], $dst", [(set RegNoBRegs16_nolh:$dst, GRRegs:$srcAddrOut, (load_postdec GRRegs:$srcAddr))]>;

let Defs = [ICC], mayLoad = 1 in
def MOV_memrn_ab : InstTeak<(outs ABRegs:$dst), (ins GRRegs:$srcAddr), "mov [$srcAddr], $dst", [(set ABRegs:$dst, (sextloadi16 GRRegs:$srcAddr))]>;

let Defs = [ICC], mayLoad = 1 in
def MOV_memrn_ab1 : InstTeak<(outs ABRegs:$dst), (ins GRRegs:$srcAddr), "mov [$srcAddr], ${dst}l", [(set ABRegs:$dst, (zextloadi16 GRRegs:$srcAddr))]>;

// let mayStore = 1 in
// def MOV_a_r7offset16 : InstTeak<
//     (outs),
//     (ins ARegs:$a, memsrc:$offset),
//     "mov ${a}l, $offset",
//     [(truncstorei16 ARegs:$a, addr:$offset)]>;

let mayStore = 1 in
def MOV_al_r7offset7s : InstTeak<(outs), (ins ALRegs:$a, memsrc:$offset), "mov $a, $offset", []>;

let mayStore = 1 in
def MOV_al_r7offset16 : InstTeakImm16<(outs), (ins ALRegs:$a, memsrc:$offset), "mov $a, $offset", []>;

let mayStore = 1 in
def STORE_REG_TO_STACK_PSEUDO_16 : TeakPseudoInst<(outs), (ins RegNoBRegs16:$a, memsrc:$offset), "STORE_REG_TO_STACK_PSEUDO_16", [(store RegNoBRegs16:$a, addr:$offset)]>;

let mayStore = 1 in
def STORE_REG_TO_STACK_PSEUDO_TRUNC16 : TeakPseudoInst<(outs), (ins ABRegs:$a, memsrc:$offset), "STORE_REG_TO_STACK_PSEUDO_TRUNC16", [(truncstorei16 ABRegs:$a, addr:$offset)]>;

let mayStore = 1 in
def STORE_REG_TO_STACK_PSEUDO_32 : TeakPseudoInst<(outs), (ins ABRegs:$a, memsrc:$offset), "STORE_REG_TO_STACK_PSEUDO_32", [(truncstorei32 ABRegs:$a, addr:$offset)]>;

let mayLoad = 1 in
def LOAD_REG_FROM_STACK_PSEUDO_16 : TeakPseudoInst<(outs RegNoBRegs16:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_16", [(set RegNoBRegs16:$a, (load addr:$offset))]>;

let mayLoad = 1 in
def LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40 : TeakPseudoInst<(outs ABRegs:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_16_SEXT40", [(set ABRegs:$a, (sextloadi16 addr:$offset))]>;

let mayLoad = 1 in
def LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40 : TeakPseudoInst<(outs ABRegs:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_16_ZEXT40", [(set ABRegs:$a, (zextloadi16 addr:$offset))]>;

let mayLoad = 1 in
def LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40 : TeakPseudoInst<(outs ABRegs:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40", [(set ABRegs:$a, (extloadi32 addr:$offset))]>;

// let mayLoad = 1, Defs = [ICC] in
// def LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40 : TeakPseudoInst<(outs ARegs:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40", [(set ARegs:$a, (extloadi32 addr:$offset))]>;

//let mayLoad = 1, Defs = [ICC] in
//def LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40 : TeakPseudoInst<(outs ARegs:$a), (ins memsrc:$offset), "LOAD_REG_FROM_STACK_PSEUDO_32_SEXT40", [(set ARegs:$a, (load addr:$offset))]>;

//def : Pat<(extloadi32 memsrc:$offset), (LOAD_REG_FROM_STACK_PSEUDO_32_ZEXT40 addr:$offset)>;

// let mayStore = 1 in
// def STORE_REG_TO_STACK_PSEUDO_32 : TeakPseudoInst<
//     (outs),
//     (ins Operand<i16>:$a, memsrc:$offset),
//     "STORE_REG_TO_STACK_PSEUDO_32",
//     [(store i16:$a, addr:$offset)]>;

let mayLoad = 1, Defs = [ICC] in
def MOV_r7offset7s_a : InstTeak<(outs ARegs:$a), (ins memsrc:$offset), "mov $offset, ${a}", []>;

let mayLoad = 1, Defs = [ICC] in
def MOV_r7offset16_a : InstTeakImm16<(outs ARegs:$a), (ins memsrc:$offset), "mov $offset, ${a}", []>;


def : Pat<(truncstorei16 ARegs:$src, tglobaladdr:$dstAddr), (MOV_al_memimm16 (EXTRACT_SUBREG $src, sub_16bit), i16:$dstAddr)>;
def : Pat<(truncstorei16 ABRegs:$src, GRRegs:$dstAddr), (MOV_regnob16_memrn (EXTRACT_SUBREG $src, sub_16bit), GRRegs:$dstAddr)>;

def : Pat<(i16 (trunc ABRegs:$src)), (EXTRACT_SUBREG ABRegs:$src, sub_16bit)>;
def : Pat<(i40 (anyext ABLRegs:$src)), (INSERT_SUBREG (i40 (IMPLICIT_DEF)), ABLRegs:$src, sub_16bit)>;

let Defs = [SP], Uses = [SP] in
{
    let mayStore = 1 in
    {
        def PUSH_regnob16 : InstTeak<(outs), (ins RegNoBRegs16:$reg), "push $reg", []>;
        //def PUSH_px : InstTeak<(outs), (ins PRegs:$reg), "push $reg", []>;
        def PUSH_ararpsttmod : InstTeak<(outs), (ins ArArpSttMod:$reg), "push $reg", []>;
    }
    let Defs = [SP, ICC], mayLoad = 1 in
    {
        def POP_regnob16 : InstTeak<(outs RegNoBRegs16_nolh:$reg), (ins), "pop $reg", []>;
        //def POP_px : InstTeak<(outs PRegs:$reg), (ins), "pop $reg", []>;
        def POP_ararpsttmod : InstTeak<(outs ArArpSttMod:$reg), (ins), "pop $reg", []>;
    }
}

let isCompare = 1, Defs = [ICC] in
{
    def CMP_imm8u_a             : InstTeak     <(outs), (ins Imm8u:$val,             ARegs:$a),        "cmp ${val}u8, $a", [(TeakCmpICC ARegs:$a,               Imm8u:$val)]>;
    def CMP_imm16_a             : InstTeakImm16<(outs), (ins immNeg32768_32767:$val, ARegs:$a),        "cmp $val, $a",     [(TeakCmpICC ARegs:$a,               immNeg32768_32767:$val)]>;
    def CMPV_imm16_RegNoBRegs16 : InstTeakImm16<(outs), (ins immNeg32768_32767:$val, RegNoBRegs16:$a), "cmpv $val, $a",    [(TeakCmpICC (sext RegNoBRegs16:$a), immNeg32768_32767:$val)]>;
    
    let mayLoad = 1 in
    {
        def CMP_memimm16_a  : InstTeakImm16<(outs), (ins Operand<i16>:$addr, ARegs:$a), "cmp [$addr], $a",  [(TeakCmpICC ARegs:$a, (sextloadi16 imm:$addr))]>;
        def CMP_memrn_a  : InstTeak<(outs), (ins GRRegs:$addr, ARegs:$a), "cmp [$addr], $a",  [(TeakCmpICC ARegs:$a, (sextloadi16 GRRegs:$addr))]>;
        def CMPU_memrn_a : InstTeak<(outs), (ins GRRegs:$addr, ARegs:$a), "cmpu [$addr], $a", [(TeakCmpICC ARegs:$a, (zextloadi16 GRRegs:$addr))]>;
    }

    def CMP_regnobp016_a : InstTeak<(outs), (ins RegNoBRegs16:$b,  ARegs:$a),  "cmp $b, $a",  [(TeakCmpICC ARegs:$a,  (sext RegNoBRegs16:$b))]>;
    def CMPU_regnob016_a : InstTeak<(outs), (ins RegNoBRegs16:$b,  ARegs:$a),  "cmpu $b, $a", [(TeakCmpICC ARegs:$a,  (zext RegNoBRegs16:$b))]>;
    def CMP_ab_ab        : InstTeak<(outs), (ins ABRegs:$b,        ABRegs:$a), "cmp $b, $a",  [(TeakCmpICC ABRegs:$a, ABRegs:$b)]>;
}

let Uses = [ICC], usesCustomInserter = 1 in
{
    def SELECT_CC_Int_ICC : TeakPseudoInst<(outs ABRegs:$dst), (ins ABRegs:$T, ABRegs:$F, Operand<i40>:$Cond), "; SELECT_CC_Int_ICC PSEUDO!",
        [(set i40:$dst, (TeakSelectICC i40:$T, i40:$F, imm:$Cond))]>;

    def SELECT_CC_Int_ICC_i16 : TeakPseudoInst<(outs GRRegs:$dst), (ins RegNoBRegs16:$T, RegNoBRegs16:$F, Operand<i40>:$Cond), "; SELECT_CC_Int_ICC PSEUDO!",
        [(set GRRegs:$dst, (TeakSelectICC RegNoBRegs16:$T, RegNoBRegs16:$F, imm:$Cond))]>;
}

let Uses = [ICC], isBranch = 1, isTerminator = 1 in
def BRCond_imm18 : InstTeakImm16<(outs), (ins Operand<OtherVT>:$addr, CCOp:$cond), "br $addr, $cond", [(TeakBRICC bb:$addr, imm:$cond)]>;

let isBarrier = 1, isBranch = 1, isTerminator = 1 in
def BR_imm18 : InstTeakImm16<(outs), (ins Operand<OtherVT>:$addr), "br $addr, always", [(br bb:$addr)]>;

let isCall = 1, Defs = [SP], Uses = [SP] in 
def CALL_imm : InstTeakImm16<(outs), (ins Operand<i40>:$addr, pred:$p), "call $addr, $p", [(teak_call tglobaladdr:$addr)]>;

let isTerminator = 1, isReturn = 1, isBarrier = 1, Defs = [SP], Uses = [SP], mayLoad = 1 in
def RET : InstTeak<(outs), (ins pred:$p), "ret $p",  [(TeakRetFlag)]>;

let Defs = [SP], Uses = [SP] in
{
    def ADJCALLSTACKDOWN : TeakPseudoInst<(outs), (ins i16imm:$amt, i16imm:$amt2), "# ADJCALLSTACKDOWN $amt, $amt2", [(callseq_start timm:$amt, timm:$amt2)]>;
    def ADJCALLSTACKUP   : TeakPseudoInst<(outs), (ins i16imm:$amt1, i16imm:$amt2), "# ADJCALLSTACKUP $amt1", [(callseq_end timm:$amt1, timm:$amt2)]>;
}

def RawAsmOp : InstTeak<(outs), (ins i16imm:$opcode), ".word $opcode", []>;
def RawAsmOpExtended : InstTeakImm16<(outs), (ins i16imm:$opcode, i16imm:$ext), ".word $opcode, $ext", []>;

def : Pat<(i40 (add ARegs:$a, (sextloadi16 (TeakWrapper tglobaladdr:$src)))), (ADD_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakAnd ARegs:$a, (zextloadi16 (TeakWrapper tglobaladdr:$src)))), (AND_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakAnd ARegs:$a, (extloadi16 (TeakWrapper tglobaladdr:$src)))), (AND_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakOr ARegs:$a, (zextloadi16 (TeakWrapper tglobaladdr:$src)))), (OR_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakOr ARegs:$a, (extloadi16 (TeakWrapper tglobaladdr:$src)))), (OR_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakXor ARegs:$a, (zextloadi16 (TeakWrapper tglobaladdr:$src)))), (XOR_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(i40 (TeakXor ARegs:$a, (extloadi16 (TeakWrapper tglobaladdr:$src)))), (XOR_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(TeakCmpICC ARegs:$a, (i40 (sextloadi16 (TeakWrapper tglobaladdr:$src)))), (CMP_memimm16_a tglobaladdr:$src, ARegs:$a)>;
def : Pat<(store ALRegs:$src, (i16 (TeakWrapper tglobaladdr:$dst))), (MOV_al2_memimm16 ALRegs:$src, tglobaladdr:$dst)>;
def : Pat<(truncstorei16 ARegs:$src, (i16 (TeakWrapper tglobaladdr:$dst))), (MOV_al_memimm16 ARegs:$src, tglobaladdr:$dst)>;
def : Pat<(i40 (sextloadi16 (TeakWrapper tglobaladdr:$src))), (MOV_memimm16_a tglobaladdr:$src)>;
def : Pat<(i40 (extloadi16 (TeakWrapper tglobaladdr:$src))), (MOV_memimm16_a tglobaladdr:$src)>;
def : Pat<(i16 (TeakWrapper tglobaladdr:$dst)), (MOV_imm16_regnob16 tglobaladdr:$dst)>;
// anyextload
def : Pat<(extloadi16 tglobaladdr:$srcAddr), (MOV_memimm16_a tglobaladdr:$srcAddr)>;
def : Pat<(extloadi16 imm:$srcAddr), (MOV_memimm16_a imm:$srcAddr)>;
def : Pat<(extloadi16 GRRegs:$srcAddr), (MOV_memrn_ab1 GRRegs:$srcAddr)>;