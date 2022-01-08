# CPP-MIPS-Assembler
* MIPS assembler implemented with C++.
* Reads in asm file and translates it into MIPS machine language

# MIPS Instruction Supported
* addiu
* addu
* and
* beq
* bne
* div
* j
* lw
* mfhi
* mflo
* mult
* or
* slt
* subu
* sw
* syscall

# MIPS Directives Supported
* .text
* .data
* .word (w1,...,wn)
* .space n

# Error Checking
* Valid instruction check "Invalid Instruction x at line: n"
* Valid label check "Could not find label x"
* Valid register check "Invalid register at line: x"
