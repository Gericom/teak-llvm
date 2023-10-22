#include "../TargetInfo/TeakTargetInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "TeakMCTargetDesc.h"
#include "InstPrinter/TeakInstPrinter.h"
#include "TeakMCAsmInfo.h"
#include "TeakELFStreamer.h"
//\#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "TeakGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TeakGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TeakGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTeakMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTeakMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTeakMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTeakMCRegisterInfo(X, 0);
  return X;
}

static MCSubtargetInfo *createTeakMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU,
                                                 StringRef FS) {
  return createTeakMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCAsmInfo *createTeakMCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TT,
                                     const MCTargetOptions &Options) {
  return new TeakMCAsmInfo(TT);
}

// static MCCodeGenInfo *createTeakMCCodeGenInfo(const Triple &TT, Reloc::Model RM,
//                                              CodeModel::Model CM,
//                                              CodeGenOpt::Level OL) {
//   MCCodeGenInfo *X = new MCCodeGenInfo();
//   if (RM == Reloc::Default) {
//     RM = Reloc::Static;
//   }
//   if (CM == CodeModel::Default) {
//     CM = CodeModel::Small;
//   }
//   if (CM != CodeModel::Small && CM != CodeModel::Large) {
//     report_fatal_error("Target only supports CodeModel Small or Large");
//   }

//   X->initMCCodeGenInfo(RM, CM, OL);
//   return X;
// }

static MCInstPrinter* createTeakMCInstPrinter(const Triple &TT, unsigned SyntaxVariant,
    const MCAsmInfo &MAI, const MCInstrInfo &MII, const MCRegisterInfo &MRI)
{
    return new TeakInstPrinter(MAI, MII, MRI);
}

static MCStreamer* createTeakMCStreamer(const Triple &T, MCContext &Context,
    std::unique_ptr<MCAsmBackend> &&MAB, std::unique_ptr<MCObjectWriter> &&OW,
    std::unique_ptr<MCCodeEmitter> &&Emitter, bool RelaxAll)
{
    return createTeakELFStreamer(Context, std::move(MAB), std::move(OW), std::move(Emitter), RelaxAll);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTeakTargetMC()
{
    // Register the MC asm info.
    RegisterMCAsmInfoFn X(getTheTeakTarget(), createTeakMCAsmInfo);

    // Register the MC codegen info.
    //TargetRegistry::RegisterMCCodeGenInfo(getTheTeakTarget(), createTeakMCCodeGenInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(getTheTeakTarget(), createTeakMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(getTheTeakTarget(), createTeakMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(getTheTeakTarget(), createTeakMCSubtargetInfo);

    // Register the MCInstPrinter
    TargetRegistry::RegisterMCInstPrinter(getTheTeakTarget(), createTeakMCInstPrinter);

    // Register the ASM Backend.
    TargetRegistry::RegisterMCAsmBackend(getTheTeakTarget(), createTeakAsmBackend);

    // Register the MCCodeEmitter
    TargetRegistry::RegisterMCCodeEmitter(getTheTeakTarget(), createTeakMCCodeEmitter);

    TargetRegistry::RegisterELFStreamer(getTheTeakTarget(), createTeakMCStreamer);
}
