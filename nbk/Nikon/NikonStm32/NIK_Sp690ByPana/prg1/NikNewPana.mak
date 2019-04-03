#================================================================
# DebugFactory Builder make file
# 
# Author   : Panasonic Corporation
# 
# FileName :NikNewPana.mak
#================================================================
 
#================================================================
#       According to DebugFactory Builder rules 
#
# Editing can be performed from IDE for the following macro definitions with 
# DebugFactory Builder.  
# Do not edit the make file directly. 
# The edited data will be discarded by the IDE setting. 
#PROJECT    : target 
#SOURCEFILE : source file 
#TOOLDIR    : tool storage folder 
#CC         : compiler name
#ASM        : assembler name
#LINK       : linker name 
#CINC       : include file path (for compiler) 
#AINC       : include file path (for assembler) 
#LIBPATH    : library file path 
#LIBFILES   : library name 
#CFLAGS     : compiler options 
#CFLAGS     : compiler options for assembler file output 
#AFLAGS     : assembler options 
#LFLAGS     : linker library options 
#OBJECTOUTPUTDIR : object file (*.rf) output folder 
#
       
#================================================================
 
#================================================================
# Target setting
#================================================================
PROJECT	= NikNewPana.ex
 
#================================================================
# User macro definition
#================================================================
#========================DFUSRDEFMACRO_S==============================
#========================DFUSRDEFMACRO_E==============================
 
#================================================================
# Source file setting
#================================================================
SOURCEFILE	= ..\src\Calc.c \
		..\src\ClkSyn.c \
		..\src\ComuProc.c \
		..\src\init.c \
		..\src\int.c \
		..\src\main.c \
		..\src\ST101cS.ASM \
		..\src\timer.c \
		..\src\uart.c \
 
#================================================================
# Intermediate file setting
#================================================================
OBJFILES	= ..\obj\Calc.rf \
		..\obj\ClkSyn.rf \
		..\obj\ComuProc.rf \
		..\obj\init.rf \
		..\obj\int.rf \
		..\obj\main.rf \
		..\obj\ST101cS.rf \
		..\obj\timer.rf \
		..\obj\uart.rf \
 
#================================================================
# Tool setting
#================================================================
CC	= CC101E.EXE
ASM	= AS101E.EXE
LINK	= LD101E.EXE
 
#================================================================
# C source include file path setting
#================================================================
CINC	= -ID:\PROGRA~1\PANASO~1\DEBUGF~1\MN101\BUILTI~1\1_7R1\CC101E\INCLUDE
 
#================================================================
# Assembler include path setting
#================================================================
AINC	= 
 
#================================================================
# Library path setting
#================================================================
LIBPATH	= -LD:\PROGRA~1\PANASO~1\DEBUGF~1\MN101\BUILTI~1\1_7R1\CC101E\LIB \
		-LD:\PROGRA~1\PANASO~1\DEBUGF~1\MN101\BUILTI~1\1_7R1\CC101E\LIB \
		-LD:\PROGRA~1\PANASO~1\DEBUGF~1\MN101\BUILTI~1\1_7R1\CC101E\LIB \
 
#================================================================
# Library setting
#================================================================
LIBFILES	= -lCC101E.LIB \
		-lCC101EVD.LIB \
		-lCC101EW.LIB \
		-lpanax_ex_mon101e.l
 
#================================================================
# Compiler option setting
#================================================================
CFLAGS	=-e  -g -fchar-bfield -fc9x-comment -fenable-asm -c -D__USE_MONITOR_EXLIB $(CINC)
 
#================================================================
# Local option setting
#================================================================
 
#================================================================
# Assembler option setting
#================================================================
AFLAGS	= -e  -g -l -Xl -D__USE_MONITOR_EXLIB $(AINC)
CAFLAGS	= -e -g -l -Xl -D__USE_MONITOR_EXLIB $(AINC) -Ot -ZC -Zv12
 
#================================================================
# Linker option setting
#================================================================
LFLAGS	= -e  -m -g
 
#================================================================
# Object file (*.RF) output folder setting
#================================================================
OBJECTOUTPUTDIR	= ..\obj\\
 
#================================================================
# Target (EX)
#================================================================
$(PROJECT): $(OBJFILES)
 
#================================================================
# Pre-link user-defined processing
#================================================================
#========================DFUSRBDEF_S==============================
#========================DFUSRBDEF_E==============================
 
#================================================================
# Target (EX)
#================================================================
	$(LINK) @&&|
		-o.\$(PROJECT)
		$(LFLAGS)
		$(LIBPATH)
		-T@CODE=4000
		-T@DATA=0
		$(OBJECTOUTPUTDIR)ST101cS.rf
		$(OBJECTOUTPUTDIR)Calc.rf
		$(OBJECTOUTPUTDIR)ClkSyn.rf
		$(OBJECTOUTPUTDIR)ComuProc.rf
		$(OBJECTOUTPUTDIR)init.rf
		$(OBJECTOUTPUTDIR)int.rf
		$(OBJECTOUTPUTDIR)main.rf
		$(OBJECTOUTPUTDIR)timer.rf
		$(OBJECTOUTPUTDIR)uart.rf
		$(LIBFILES)
|
 
#================================================================
# Post-make user-defined processing
#================================================================
#========================DFUSRADEF_S==============================
#========================DFUSRADEF_E==============================
 
#================================================================
# Execution command
#================================================================
$(OBJECTOUTPUTDIR)Calc.rf: ..\src\Calc.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)Calc.rf ..\src\Calc.c
$(OBJECTOUTPUTDIR)ClkSyn.rf: ..\src\ClkSyn.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)ClkSyn.rf ..\src\ClkSyn.c
$(OBJECTOUTPUTDIR)ComuProc.rf: ..\src\ComuProc.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)ComuProc.rf ..\src\ComuProc.c
$(OBJECTOUTPUTDIR)init.rf: ..\src\init.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)init.rf ..\src\init.c
$(OBJECTOUTPUTDIR)int.rf: ..\src\int.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)int.rf ..\src\int.c
$(OBJECTOUTPUTDIR)main.rf: ..\src\main.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)main.rf ..\src\main.c
$(OBJECTOUTPUTDIR)ST101cS.rf: ..\src\ST101cS.ASM
	$(ASM) $(AFLAGS) -o $(OBJECTOUTPUTDIR)$(@F) ..\src\ST101cS.ASM
$(OBJECTOUTPUTDIR)timer.rf: ..\src\timer.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)timer.rf ..\src\timer.c
$(OBJECTOUTPUTDIR)uart.rf: ..\src\uart.c
	$(CC) $(CFLAGS) -o $(OBJECTOUTPUTDIR)uart.rf ..\src\uart.c
