#pragma once

#include "llvm/MC/MCELFStreamer.h"
#include <memory>

namespace llvm
{
    class MCAsmBackend;
    class MCCodeEmitter;
    class MCContext;
    class MCSubtargetInfo;
    class MCObjectWriter;

    class TeakELFStreamer : public MCELFStreamer
    {
    public:
        TeakELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> MAB,
            std::unique_ptr<MCObjectWriter> OW, std::unique_ptr<MCCodeEmitter> Emitter);
        
        void EmitIntValue(uint64_t Value, unsigned Size) override;
    };

    MCELFStreamer* createTeakELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> MAB,
        std::unique_ptr<MCObjectWriter> OW, std::unique_ptr<MCCodeEmitter> Emitter, bool RelaxAll);
}