#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Spy
CND_DISTDIR=dist

# Include project Makefile
include example_dpp-Makefile.mk

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES=

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	cd ../../fw/ql/examples/80x86/linux/gnu/dpp && ${MAKE} -f Makefile spy

# Subprojects
.build-subprojects:
	cd ../Qf && ${MAKE}  -f Qf-Makefile.mk CONF=Spy
	cd ../Qep && ${MAKE}  -f Qep-Makefile.mk CONF=Spy
	cd ../Qspy && ${MAKE}  -f Qspy-Makefile.mk CONF=Spy

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	cd ../../fw/ql/examples/80x86/linux/gnu/dpp && ${MAKE} -f Makefile spy_clean

# Subprojects
.clean-subprojects:
	cd ../Qf && ${MAKE}  -f Qf-Makefile.mk CONF=Spy clean
	cd ../Qep && ${MAKE}  -f Qep-Makefile.mk CONF=Spy clean
	cd ../Qspy && ${MAKE}  -f Qspy-Makefile.mk CONF=Spy clean
