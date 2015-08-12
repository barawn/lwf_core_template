# lwf_core_template

Template for a Code Composer Studio lwframework project. lwframework projects only work on MSP430s currently, in CPU (not CPUX) mode, using the TI C Compiler (not the MSP430 GCC compiler).

Note: for any lwframework project you need to go to project properties, "MSP430 Compiler->Advanced Options->Runtime Model Options" and select "Reserve a register for the user. (--global register) [r4]".

## Creating a lwframework project from the template

1. Import the lwf_core_template project from Git
2. Create your own blank project.
3. Link the "core" folder in the lwf_core_template project into your own project. (New->Folder, then Advanced, then Link to an..).
4. Create a components.cpp, components.h, and main.cpp file. You can open the ".template" files from the lwf_core_template project and copy those over.
5. Add the lwf_core_template project and the current project to the include locations. (in Properties, go to MSP430 Compiler->Include Options, then Add, Workspace, and select the lwf_core_template project. Then do the same with your own project).
6. While there, go to MSP430 Compiler->Advanced Options->Runtime Model Options and select "Reserve a register for the user. (--global register) [r4]".

## See the Wiki!

The wiki on GitHub ( https://github.com/barawn/lwf_core_template/wiki ) has more information on lwframework.
