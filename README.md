# lwf_core_template

Template for a Code Composer Studio lwframework project. lwframework projects only work on MSP430s currently, in CPU (not CPUX) mode, using the TI C Compiler (not the MSP430 GCC compiler).

Note: for any lwframework project you need to go to project properties, "MSP430 Compiler->Advanced Options->Runtime Model Options" and select "Reserve a register for the user. (--global register) [r4]".
