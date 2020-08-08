//===- Teak.cpp ------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Teak is a Harvard-architecture 8-bit micrcontroller designed for small
// baremetal programs. All Teak-family processors have 32 8-bit registers.
// The tiniest Teak has 32 byte RAM and 1 KiB program memory, and the largest
// one supports up to 2^24 data address space and 2^22 code address space.
//
// Since it is a baremetal programming, there's usually no loader to load
// ELF files on Teaks. You are expected to link your program against address
// 0 and pull out a .text section from the result using objcopy, so that you
// can write the linked code to on-chip flush memory. You can do that with
// the following commands:
//
//   ld.lld -Ttext=0 -o foo foo.o
//   objcopy -O binary --only-section=.text foo output.bin
//
// Note that the current Teak support is very preliminary so you can't
// link any useful program yet, though.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;

namespace lld {
namespace elf {

namespace {
class Teak final : public TargetInfo {
public:
  Teak();
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocateOne(uint8_t *loc, RelType type, uint64_t val) const override;
  int64_t getImplicitAddend(const uint8_t* buf, RelType type) const override;
};
} // namespace

Teak::Teak() { noneRel = R_TEAK_NONE; }

RelExpr Teak::getRelExpr(RelType type, const Symbol &s,
                        const uint8_t *loc) const {
  return R_ABS;
}

void Teak::relocateOne(uint8_t *loc, RelType type, uint64_t val) const
{
	switch (type)
	{
		case R_TEAK_CALL_IMM18:
			write16le(loc, (read16le(loc) & ~0x30) | (((val >> 17) & 3) << 4));
			write16le(loc + 2, (val >> 1) & 0xFFFF);
			break;
		case R_TEAK_PTR_IMM16:
			write16le(loc + 2, (val >> 1) & 0xFFFF);
			break;
		case R_TEAK_BKREP_REG:
			val -= 2;
			write16le(loc, (read16le(loc) & ~0x60) | (((val >> 17) & 3) << 5));
			write16le(loc + 2, (val >> 1) & 0xFFFF);
			break;
		case R_TEAK_BKREP_R6:
			val -= 2;
			write16le(loc, (read16le(loc) & ~3) | ((val >> 17) & 3));
			write16le(loc + 2, (val >> 1) & 0xFFFF);
			break;
		default:
			error(getErrorLocation(loc) + "unrecognized relocation " + toString(type));
	}
}

int64_t Teak::getImplicitAddend(const uint8_t* buf, RelType type) const
{
	switch (type)
	{
		case R_TEAK_CALL_IMM18:
			return (((read16le(buf) >> 4) & 3) << 17) | (read16le(buf + 2) << 1);
		case R_TEAK_PTR_IMM16:
			return read16le(buf + 2) << 1;
		case R_TEAK_BKREP_REG:
			return (((read16le(buf) >> 5) & 3) << 17) | (read16le(buf + 2) << 1) + 2;
		case R_TEAK_BKREP_R6:
			return ((read16le(buf) & 3) << 17) | (read16le(buf + 2) << 1) + 2;
		default:
			error("unrecognized relocation " + toString(type));
	}
	return 0;
}

TargetInfo *getTeakTargetInfo() {
  static Teak target;
  return &target;
}

} // namespace elf
} // namespace lld 