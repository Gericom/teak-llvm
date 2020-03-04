set(CMAKE_CXX_STANDARD 17)
add_llvm_component_library(LLVMTeakAsmParser
  TeakAsmParser.cpp
  disassembler.cpp
  parser.cpp
  )
