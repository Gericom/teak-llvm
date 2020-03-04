//===-- TeakInstrFormats.td - Teak Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
class InstTeak<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<32> Inst;

  let Namespace = "Teak";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  let Size = 2;

  // Always execute - no condition flags for this example
  //let Inst{31-28} = 0b1110;
}

class InstTeakImm16<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTeak<outs, ins, asmstr, pattern> {
  let Size = 4;
}

// Teak pseudo instructions format
class TeakPseudoInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTeak<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
}

// class ALUInst<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
//     : InstTeak<outs, ins, asmstr, pattern> {
//   let Inst{27-26} = 0b00;
//   let Inst{24-21} = opcode;
// }

// class MulInst<bits<7> opcode, dag outs, dag ins, string asmstr,
//               list<dag> pattern> : InstTeak<outs, ins, asmstr, pattern> {
//   let Inst{27-21} = opcode;
//   let Inst{20}    = 0;  // We never update flags
//   let Inst{7-4}   = 0b1001;
// }

// class BranchInst<bits<4> opcode, dag outs, dag ins,
//                  string asmstr, list<dag> pattern>
//     : InstTeak<outs, ins, asmstr, pattern> {
//   let Inst{27-24} = opcode;
// }