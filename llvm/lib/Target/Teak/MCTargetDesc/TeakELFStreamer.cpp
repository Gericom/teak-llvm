#include "TeakELFStreamer.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

TeakELFStreamer::TeakELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> MAB,
    std::unique_ptr<MCObjectWriter> OW, std::unique_ptr<MCCodeEmitter> Emitter)
    : MCELFStreamer(Context, std::move(MAB), std::move(OW), std::move(Emitter))
{

}

void TeakELFStreamer::EmitIntValue(uint64_t Value, unsigned Size)
{
    if (Size == 4)
    {
        char buf[4];
        buf[0] = uint8_t((Value >> 16) & 0xFF);
        buf[1] = uint8_t((Value >> 24) & 0xFF);
        buf[2] = uint8_t(Value & 0xFF);
        buf[3] = uint8_t((Value >> 8) & 0xFF);
        EmitBytes(StringRef(buf, Size));
        return;
    }
    MCELFStreamer::EmitIntValue(Value, Size);
}

MCELFStreamer* llvm::createTeakELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> MAB,
    std::unique_ptr<MCObjectWriter> OW, std::unique_ptr<MCCodeEmitter> Emitter, bool RelaxAll)
{
    return new TeakELFStreamer(Context, std::move(MAB), std::move(OW), std::move(Emitter));
} 