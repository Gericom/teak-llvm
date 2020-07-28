set(LLVM_TARGET_DEFINITIONS Teak.td)

tablegen(LLVM TeakGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TeakGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TeakGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TeakGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TeakGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TeakGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM TeakGenMCCodeEmitter.inc -gen-emitter)
add_public_tablegen_target(TeakCommonTableGen)

add_llvm_target(TeakCodeGen
  TeakSubtarget.cpp
  TeakTargetMachine.cpp
  TeakRegisterInfo.cpp
  TeakInstrInfo.cpp
  TeakFrameLowering.cpp
  TeakISelLowering.cpp
  TeakSelectionDAGInfo.cpp
  TeakISelDAGToDAG.cpp
  TeakAsmPrinter.cpp
  TeakMCInstLower.cpp
  TeakOptimizeMovImmPass.cpp
  )

add_subdirectory(AsmParser)
add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)