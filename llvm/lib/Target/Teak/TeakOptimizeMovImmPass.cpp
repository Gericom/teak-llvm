#include "Teak.h"
#include "TeakInstrInfo.h"
#include "TeakMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
using namespace llvm;

namespace
{
    class TeakOptimizeMovImmPass : public MachineFunctionPass 
    {
    public:
        static char sId;

        TeakOptimizeMovImmPass() : MachineFunctionPass(sId) { }

        bool runOnMachineFunction(MachineFunction& mf) override;

        MachineFunctionProperties getRequiredProperties() const override
        {
            return MachineFunctionProperties()
              .set(MachineFunctionProperties::Property::NoVRegs);
        }

        StringRef getPassName() const override { return "optimise teak move immediate pass"; }
    };

    char TeakOptimizeMovImmPass::sId = 0;
}

bool TeakOptimizeMovImmPass::runOnMachineFunction(MachineFunction& mf)
{
    if (skipFunction(mf.getFunction()))
        return false;

    const TargetInstrInfo* tii = mf.getSubtarget().getInstrInfo();

    std::vector<MachineInstr*> movImms;
    std::vector<MachineInstr*> addvSubvReg;

    for (auto& mbb : mf)
    {
        for (auto& mi : mbb)
        {
            if (mi.getOpcode() == Teak::MOV_imm16_regnob16 ||
                mi.getOpcode() == Teak::MOV_imm16_ab ||
                mi.getOpcode() == Teak::MOV_imm16_abh)
            {
                movImms.push_back(&mi);
            }
            else if (mi.getOpcode() == Teak::ADDV_imm16_RegNoBRegs16 || mi.getOpcode() == Teak::SUBV_imm16_RegNoBRegs16)
                addvSubvReg.push_back(&mi);
        }
    }

    bool changed = false;

    for (auto mi : movImms)
    {
        if (!mi->getOperand(1).isImm())
            continue;
        unsigned dstReg = mi->getOperand(0).getReg();
        if (mi->getOpcode() == Teak::MOV_imm16_ab)
            dstReg = teakGetAbLReg(dstReg);
        else if (mi->getOpcode() == Teak::MOV_imm16_abh)
            dstReg = teakGetAbHReg(dstReg);
        int64_t imm = mi->getOperand(1).getImm();
        if ((dstReg == Teak::R0 || dstReg == Teak::R1 ||
            dstReg == Teak::R2 || dstReg == Teak::R3 ||
            dstReg == Teak::R4 || dstReg == Teak::R5 ||
            dstReg == Teak::R7 || dstReg == Teak::Y0 ||
            dstReg == Teak::SV || dstReg == Teak::A0H ||
            dstReg == Teak::A1H || dstReg == Teak::EXT0 || 
            dstReg == Teak::EXT1 || dstReg == Teak::EXT2 ||
            dstReg == Teak::EXT3) && imm >= -128 && imm <= 127)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MOV_imm8s), dstReg)
                .addImm(imm);
            mi->eraseFromParent();
            changed = true;
        }
        else if ((dstReg == Teak::A0L || dstReg == Teak::A1L) && imm >= 0 && imm <= 255)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MOV_imm8u), dstReg)
                .addImm(imm);
            mi->eraseFromParent();
            changed = true;
        }        
    }

    for (auto mi : addvSubvReg)
    {
        if (!mi->getOperand(1).isImm())
            continue;
        unsigned dstReg = mi->getOperand(0).getReg();
        if (!TeakMCRegisterClasses[Teak::GRRegsRegClassID].contains(dstReg))
            continue;

        int16_t imm = (int16_t)mi->getOperand(1).getImm();
        if (mi->getOpcode() == Teak::SUBV_imm16_RegNoBRegs16)
            imm = -imm;

        if (imm == 1)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MODR_inc1), dstReg)
                .addReg(dstReg);
            mi->eraseFromParent();
            changed = true;
        }
        else if (imm == -1)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MODR_dec1), dstReg)
                .addReg(dstReg);
            mi->eraseFromParent();
            changed = true;
        }
        else if (imm == 2)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MODR_inc2), dstReg)
                .addReg(dstReg);
            mi->eraseFromParent();
            changed = true;
        }
        else if (imm == -2)
        {
            BuildMI(*mi->getParent(), *mi, mi->getDebugLoc(), tii->get(Teak::MODR_dec2), dstReg)
                .addReg(dstReg);
            mi->eraseFromParent();
            changed = true;
        }
    }

    return changed;
}

FunctionPass* llvm::createTeakOptimizeMovImmPass()
{
    return new TeakOptimizeMovImmPass();
}