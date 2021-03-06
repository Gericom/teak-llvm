#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <cstring>
#include "MCTargetDesc/TeakMCTargetDesc.h"
#include "TargetInfo/TeakTargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetRegistry.h"
#include "Teak.h"
#include "operand.h"
#include "parser.h"
#include "disassembler.h"

using namespace llvm;

namespace
{
    class TeakAsmParser : public MCTargetAsmParser
    {
        std::unique_ptr<Teakra::Parser> _parser;

        void convertToMapAndConstraints(unsigned Kind, const OperandVector &Operands) override;

        bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands,
            MCStreamer &Out, uint64_t &ErrorInfo, bool MatchingInlineAsm) override;

        bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

        bool ParseOperand(OperandVector &Operands, StringRef Mnemonic);
        bool ParseConditionOp(const std::string ccString, TeakCC::CondCodes& condition);
        bool ParseConditionOp(TeakCC::CondCodes& condition);
        bool ParseRegOp(OperandVector& operands);

        bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
            OperandVector &Operands) override;

        bool ParseDirective(AsmToken DirectiveID) override;

        bool equalIsAsmAssignment() override { return false; }
        bool starIsStartOfStatement() override { return false; }

    public:
        TeakAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
            const MCInstrInfo &MII, const MCTargetOptions &Options)
            : MCTargetAsmParser(Options, STI, MII)
        {
            _parser = Teakra::GenerateParser();
        }
    };

    //teak has the following kinds of operands:
    //- register    a0
    //- immediate (with possible type info and sign) 0x80u8
    //- condition   always
    //- identifier  main
    //- swap op     a0<->b1
    //- memory operand
    //-- register   [r0]
    //-- immediate  [0x8000]
    //-- page imm   [page:1u8]
    //-- r7 offset  [r7+3s7]
    //-- idk        [arrn1+ars0]

    struct TeakOperand : public MCParsedAsmOperand
    {
        enum KindTy
        {
            Token,
            Register,
            Immediate,
        } Kind;

        struct RegOp
        {
            unsigned RegNum;
        };

        struct ImmOp
        {
            const MCExpr *Val;
        };

        SMLoc StartLoc, EndLoc;
        std::string Tok;
        bool IsInt;
        bool HasIntSign;
        bool HasIntType;
        unsigned IntVal;
        union
        {            
            RegOp Reg;
            ImmOp Imm;
        };

        TeakOperand(KindTy K)
            : MCParsedAsmOperand(), Kind(K) {}

    public:
        TeakOperand(const TeakOperand &o)
            : MCParsedAsmOperand()
        {
            Kind = o.Kind;
            StartLoc = o.StartLoc;
            EndLoc = o.EndLoc;

            switch (Kind)
            {
                case Register:
                    Reg = o.Reg;
                    break;
                case Immediate:
                    Imm = o.Imm;
                    break;
                case Token:
                    Tok = o.Tok;
                    break;
            }
        }

        bool isToken() const override { return Kind == Token; }
        bool isReg() const override { return Kind == Register; }
        bool isImm() const override { return Kind == Immediate; }
        bool isMem() const override { return false; }

        bool isConstantImm() const
        {
            return isImm() && isa<MCConstantExpr>(getImm());
        }

        int64_t getConstantImm() const
        {
            const MCExpr *Val = getImm();
            return static_cast<const MCConstantExpr *>(Val)->getValue();
        }

        bool isSImm12() const
        {
            return (isConstantImm() && isInt<12>(getConstantImm()));
        }

        /// getStartLoc - Gets location of the first token of this operand
        SMLoc getStartLoc() const override { return StartLoc; }
        /// getEndLoc - Gets location of the last token of this operand
        SMLoc getEndLoc() const override { return EndLoc; }

        unsigned getReg() const override
        {
            assert(Kind == Register && "Invalid type access!");
            return Reg.RegNum;
        }

        const MCExpr *getImm() const
        {
            assert(Kind == Immediate && "Invalid type access!");
            return Imm.Val;
        }

        std::string getToken() const
        {
            assert(Kind == Token && "Invalid type access!");
            return Tok;
        }

        void print(raw_ostream &OS) const override
        {
            switch (Kind)
            {
                case Immediate:
                    OS << *getImm();
                     break;
                case Register:
                    OS << "<register x";
                    OS << getReg() << ">";
                    break;
                case Token:
                    OS << "'" << getToken() << "'";
                    break;
            }
        }

        void addExpr(MCInst &Inst, const MCExpr *Expr) const
        {
            assert(Expr && "Expr shouldn't be null!");

            if (auto *CE = dyn_cast<MCConstantExpr>(Expr))
                Inst.addOperand(MCOperand::createImm(CE->getValue()));
            else
                Inst.addOperand(MCOperand::createExpr(Expr));
        }

        // Used by the TableGen Code
        void addRegOperands(MCInst &Inst, unsigned N) const
        {
            assert(N == 1 && "Invalid number of operands!");
            Inst.addOperand(MCOperand::createReg(getReg()));
        }

        void addImmOperands(MCInst &Inst, unsigned N) const
        {
            assert(N == 1 && "Invalid number of operands!");
            addExpr(Inst, getImm());
        }

        static std::unique_ptr<TeakOperand> createToken(std::string Str, SMLoc S, bool isInt = false, bool hasIntSign = false, bool hasIntType = false, unsigned intVal = 0)
        {
            auto Op = std::make_unique<TeakOperand>(Token);
            Op->Tok = Str;
            Op->IsInt = isInt;
            Op->HasIntSign = hasIntSign;
            Op->HasIntType = hasIntType;
            Op->IntVal = intVal;
            Op->StartLoc = S;
            Op->EndLoc = S;
            return Op;
        }

        static std::unique_ptr<TeakOperand> createReg(unsigned RegNo, SMLoc S, SMLoc E)
        {
            auto Op = std::make_unique<TeakOperand>(Register);
            Op->Reg.RegNum = RegNo;
            Op->StartLoc = S;
            Op->EndLoc = E;
            return Op;
        }

        static std::unique_ptr<TeakOperand> createImm(const MCExpr *Val, SMLoc S, SMLoc E)
        {
            auto Op = std::make_unique<TeakOperand>(Immediate);
            Op->Imm.Val = Val;
            Op->StartLoc = S;
            Op->EndLoc = E;
            return Op;
        }
    };
}

std::string formatString(const std::string& format, ...)
{
    va_list args;
    va_start (args, format);
    size_t len = std::vsnprintf(NULL, 0, format.c_str(), args);
    va_end (args);
    std::vector<char> vec(len + 1);
    va_start (args, format);
    std::vsnprintf(&vec[0], len + 1, format.c_str(), args);
    va_end (args);
    return &vec[0];
}

void TeakAsmParser::convertToMapAndConstraints(unsigned Kind, const OperandVector &Operands)
{
    dbgs() << "convertToMapAndConstraints " << Kind << "\n";
    // assert(Kind < CVT_NUM_SIGNATURES && "Invalid signature!");
    // unsigned NumMCOperands = 0;
    // const uint8_t *Converter = ConversionTable[Kind];
    // for (const uint8_t *p = Converter; *p; p+= 2) {
    //     switch (*p) {
    //     default: llvm_unreachable("invalid conversion entry!");
    //     case CVT_Reg:
    //     Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
    //     Operands[*(p + 1)]->setConstraint("r");
    //     ++NumMCOperands;
    //     break;
    //     case CVT_Tied:
    //     ++NumMCOperands;
    //     break;
    //     case CVT_95_Reg:
    //     Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
    //     Operands[*(p + 1)]->setConstraint("r");
    //     NumMCOperands += 1;
    //     break;
    //     case CVT_95_addImmOperands:
    //     Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
    //     Operands[*(p + 1)]->setConstraint("m");
    //     NumMCOperands += 1;
    //     break;
    //     case CVT_imm_95_0:
    //     Operands[*(p + 1)]->setMCOperandNum(NumMCOperands);
    //     Operands[*(p + 1)]->setConstraint("");
    //     ++NumMCOperands;
    //     break;
    //     }
    // }
}

bool TeakAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands,
    MCStreamer &Out, uint64_t &ErrorInfo, bool MatchingInlineAsm)
{
    MCInst Inst;
    Inst.setLoc(IDLoc);
    std::vector<std::string> tokens;
    //dbgs() << "Tokens:\n";
    if(((TeakOperand&)*Operands[0]).getToken() == "call")
    {
        std::string cc = ((TeakOperand&)*Operands[2]).getToken();
        TeakCC::CondCodes condition;
        if(!ParseConditionOp(cc, condition))
            return Error(IDLoc, "Invalid condition code!");
        Inst.setOpcode(Teak::CALL_imm18);
        Inst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(((TeakOperand&)*Operands[1]).getToken(), MCSymbolRefExpr::VK_None, getContext())));
        Inst.addOperand(MCOperand::createImm((int)condition));
        Out.EmitInstruction(Inst, getSTI());
        return false;
    }
    else if(((TeakOperand&)*Operands[0]).getToken() == "callr")
    {
        std::string cc = ((TeakOperand&)*Operands[2]).getToken();
        TeakCC::CondCodes condition;
        if(!ParseConditionOp(cc, condition))
            return Error(IDLoc, "Invalid condition code!");
        Inst.setOpcode(Teak::CALLR_rel7);
        Inst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(((TeakOperand&)*Operands[1]).getToken(), MCSymbolRefExpr::VK_None, getContext())));
        Inst.addOperand(MCOperand::createImm((int)condition));
        Out.EmitInstruction(Inst, getSTI());
        return false;
    }
    else if(((TeakOperand&)*Operands[0]).getToken() == "br")
    {
        std::string cc = ((TeakOperand&)*Operands[2]).getToken();
        TeakCC::CondCodes condition;
        if(!ParseConditionOp(cc, condition))
            return Error(IDLoc, "Invalid condition code!");
        Inst.setOpcode(Teak::BR_imm18);
        Inst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(((TeakOperand&)*Operands[1]).getToken(), MCSymbolRefExpr::VK_None, getContext())));
        Inst.addOperand(MCOperand::createImm((int)condition));
        Out.EmitInstruction(Inst, getSTI());
        return false;
    }
    else if(((TeakOperand&)*Operands[0]).getToken() == "brr")
    {
        std::string cc = ((TeakOperand&)*Operands[2]).getToken();
        TeakCC::CondCodes condition;
        if(!ParseConditionOp(cc, condition))
            return Error(IDLoc, "Invalid condition code!");
        Inst.setOpcode(Teak::BRRCond_rel7);
        Inst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(((TeakOperand&)*Operands[1]).getToken(), MCSymbolRefExpr::VK_None, getContext())));
        Inst.addOperand(MCOperand::createImm((int)condition));
        Out.EmitInstruction(Inst, getSTI());
        return false;
    }
    else if(((TeakOperand&)*Operands[0]).getToken() == "bkrep")
    {
        std::string regString = ((TeakOperand&)*Operands[1]).getToken();

        Inst.setOpcode(Teak::BKREP_reg16);

        int reg = -1;
        if (regString == "a0l")
            reg = Teak::A0L;
        else if (regString == "a1l")
            reg = Teak::A1L;
        else if (regString == "b0l")
            reg = Teak::B0L;
        else if (regString == "b1l")
            reg = Teak::B1L;
        else if (regString == "a0h")
            reg = Teak::A0H;
        else if (regString == "a1h")
            reg = Teak::A1H;
        else if (regString == "b0h")
            reg = Teak::B0H;
        else if (regString == "b1h")
            reg = Teak::B1H;
        else if (regString == "r0")
            reg = Teak::R0;
        else if (regString == "r1")
            reg = Teak::R1;
        else if (regString == "r2")
            reg = Teak::R2;
        else if (regString == "r3")
            reg = Teak::R3;
        else if (regString == "r4")
            reg = Teak::R4;
        else if (regString == "r5")
            reg = Teak::R5;
        else if (regString == "r6")
            reg = Teak::R6;
        else if (regString == "r7")
            reg = Teak::R7;
        else if (regString == "y0")
            reg = Teak::Y0;
        else if (regString == "sv")
            reg = Teak::SV;
        else if (regString == "lc")
            reg = Teak::LC;

        if (reg == -1)
            return Error(IDLoc, "Invalid register for bkrep");

        Inst.addOperand(MCOperand::createReg(reg));
        Inst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(((TeakOperand&)*Operands[2]).getToken(), MCSymbolRefExpr::VK_None, getContext())));
        Out.EmitInstruction(Inst, getSTI());
        return false;
    }
    for (auto &op : Operands)
    {
        TeakOperand& teakOp = static_cast<TeakOperand&>(*op);
        if(teakOp.IsInt && !teakOp.HasIntSign && !teakOp.HasIntType)
           tokens.push_back("0x0000");
        else
            tokens.push_back(teakOp.getToken());
        //dbgs() << teakOp.getToken() << "\n";
    }
    auto parseResult = _parser->Parse(tokens);
    //dbgs() << "stat: " << parseResult.status << " op: " << parseResult.opcode << "\n";
    if(parseResult.status == Teakra::Parser::Opcode::Invalid)
        return Error(IDLoc, "Instruction could not be parsed");
    if(parseResult.status == Teakra::Parser::Opcode::Valid)
    {
        //reparse without integer replacements
        tokens.clear();
        for (auto &op : Operands)
        {
            TeakOperand& teakOp = static_cast<TeakOperand&>(*op);
            tokens.push_back(teakOp.getToken());
        }
        parseResult = _parser->Parse(tokens);
        Inst.setOpcode(Teak::RawAsmOp);
        Inst.addOperand(MCOperand::createImm(parseResult.opcode));
    }
    else
    {
        //find imm
        unsigned val = 0;
        for (auto &op : Operands)
        {
            TeakOperand& teakOp = static_cast<TeakOperand&>(*op);
            if(teakOp.IsInt && !teakOp.HasIntSign && !teakOp.HasIntType)
            {
                val = teakOp.IntVal;
                break;
            }
        }
        Inst.setOpcode(Teak::RawAsmOpExtended);
        Inst.addOperand(MCOperand::createImm(parseResult.opcode));
        Inst.addOperand(MCOperand::createImm(val));
    }
    Out.EmitInstruction(Inst, getSTI());
    return false;
}


bool TeakAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
{
    const AsmToken &Tok = getParser().getTok();
    StartLoc = Tok.getLoc();
    EndLoc = Tok.getEndLoc();
    RegNo = 0;
    StringRef Name = getLexer().getTok().getIdentifier();

    dbgs() << "ParseRegister: " << Name << "\n";

    // if (!MatchRegisterName(Name)) {
    //     getParser().Lex(); // Eat identifier token.
    //     return false;
    // }

    return Error(StartLoc, "invalid register name");
}

bool TeakAsmParser::ParseOperand(OperandVector &Operands, StringRef Mnemonic)
{

    return false;
}

bool TeakAsmParser::ParseConditionOp(const std::string ccString, TeakCC::CondCodes& condition)
{
    for(int i = TeakCC::True; i <= TeakCC::Iu1; i++)
    {
        if(ccString == TeakCCondCodeToString((TeakCC::CondCodes)i))
        {
            condition = (TeakCC::CondCodes)i;
            return true;
        }
    }
    return false;
}

bool TeakAsmParser::ParseConditionOp(TeakCC::CondCodes& condition)
{
    if(!getLexer().is(AsmToken::Identifier))
        return true;

    const AsmToken& token = getParser().getTok();
    StringRef condString = token.getString();
    for(int i = TeakCC::True; i <= TeakCC::Iu1; i++)
    {
        if(condString.equals(TeakCCondCodeToString((TeakCC::CondCodes)i)))
        {
            condition = (TeakCC::CondCodes)i;
            return false;
        }
    }
    return true;
}

bool TeakAsmParser::ParseRegOp(OperandVector& operands)
{
    if(!getLexer().is(AsmToken::Identifier))
        return true;

    const AsmToken& token = getParser().getTok();
    StringRef regString = token.getString();
    //THIS IS NOT COMPLETE!!!
    for(int i = (int)RegName::a0; i < (int)RegName::undefine; i++)
    {
        if(regString.equals(Teakra::Disassembler::DsmReg((RegName)i)))
        {
            operands.push_back(TeakOperand::createToken(regString, token.getLoc()));
            return false;
        }
    }
    return true;
}

bool TeakAsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
    SMLoc NameLoc, OperandVector &Operands)
{
    //dbgs() << "ParseInstruction: " << Name << "\n";
    Operands.push_back(TeakOperand::createToken(Name, NameLoc));
    while (!getLexer().is(AsmToken::EndOfStatement))
    {
        const AsmToken& Tok = getParser().getTok();
        SMLoc loc = Tok.getLoc();
        if(Tok.is(AsmToken::Comma))
        {
            getParser().Lex();
            continue;
        }
        if(Tok.is(AsmToken::LBrac))
        {
            //memory op
            std::string op = "[";
            getParser().Lex();
            while(!getLexer().is(AsmToken::RBrac))
            {
                if(getLexer().is(AsmToken::EndOfStatement))
                    return Error(loc, "Missing ending bracket!");
                op += getParser().getTok().getString();
                getParser().Lex();
            }
            op += "]";
            Operands.push_back(TeakOperand::createToken(op, loc));
            getParser().Lex();
            continue;
        }
        StringRef Name2 = Tok.getString();        
        //dbgs() << Name2 << "\n";
        if(Tok.is(AsmToken::Plus) || Tok.is(AsmToken::Minus) || Tok.is(AsmToken::Integer))
        {
            std::string op;
            bool hasSign = false;
            int mul = 1;
            if(Tok.is(AsmToken::Plus) || Tok.is(AsmToken::Minus))
            {
                hasSign = true;
                op = Tok.is(AsmToken::Plus) ? "+" : "-";
                if(Tok.is(AsmToken::Minus))
                    mul = -1;
                getParser().Lex();
            }
            else
                op = "";
            const AsmToken& Tok3 = getParser().getTok();
            if(!Tok3.is(AsmToken::Integer))
                return Error(loc, "Expected integer!");
            unsigned val = Tok3.getIntVal();
            getParser().Lex();
            const AsmToken &Tok2 = getParser().getTok();
            if(Tok2.getString().equals("u8") || Tok2.getString().equals("s8"))
            {
                Operands.push_back(TeakOperand::createToken(op + formatString("0x%04x", val) + Tok2.getString().str(), loc, true, hasSign, true, val * mul));
                getParser().Lex();
            }
            else
                Operands.push_back(TeakOperand::createToken(op + formatString("0x%04x", val), loc, true, hasSign, false, val * mul));
        }
        else if (Tok.is(AsmToken::Identifier))
        {
            std::string reg = Tok.getString();
            getParser().Lex();
            const AsmToken &Tok2 = getParser().getTok();
            if (reg == "p" && Tok2.is(AsmToken::Star))
            {
                Operands.push_back(TeakOperand::createToken("p*", loc));
                getParser().Lex();
            }
            else
                Operands.push_back(TeakOperand::createToken(reg, loc));
        }
        else
        {        
            Operands.push_back(TeakOperand::createToken(Name2, Tok.getLoc()));
            getParser().Lex();
        }
    }
    // Consume the EndOfStatement.
    getParser().Lex();
    return false;//Error(NameLoc, "unexpected token");
    // The first operand could be either register or actually an operator.
    // unsigned RegNo = MatchRegisterName(Name);

    // if (RegNo != 0) {
    //     SMLoc E = SMLoc::getFromPointer(NameLoc.getPointer() - 1);
    //     Operands.push_back(BPFOperand::createReg(RegNo, NameLoc, E));
    // } else if (BPFOperand::isValidIdAtStart (Name))
    //     Operands.push_back(BPFOperand::createToken(Name, NameLoc));
    // else
    //     return Error(NameLoc, "invalid register/token name");

    // while (!getLexer().is(AsmToken::EndOfStatement)) {
    //     // Attempt to parse token as operator
    //     if (parseOperandAsOperator(Operands) == MatchOperand_Success)
    //     continue;

    //     // Attempt to parse token as register
    //     if (parseRegister(Operands) == MatchOperand_Success)
    //     continue;

    //     // Attempt to parse token as an immediate
    //     if (parseImmediate(Operands) != MatchOperand_Success) {
    //     SMLoc Loc = getLexer().getLoc();
    //     return Error(Loc, "unexpected token");
    //     }
    // }

    // if (getLexer().isNot(AsmToken::EndOfStatement)) {
    //     SMLoc Loc = getLexer().getLoc();

    //     getParser().eatToEndOfStatement();

    //     return Error(Loc, "unexpected token");
    // }

    // // Consume the EndOfStatement.
    // getParser().Lex();
    // return false;
}

bool TeakAsmParser::ParseDirective(AsmToken DirectiveID)
{ 
    return true;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTeakAsmParser()
{
    RegisterMCAsmParser<TeakAsmParser> X(getTheTeakTarget());
}