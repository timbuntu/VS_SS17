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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/e698662c/Sensor.o \
	${OBJECTDIR}/_ext/e698662c/Server.o \
	${OBJECTDIR}/main.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/_ext/be8c287e/Test.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread
CXXFLAGS=-pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vs_p1

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vs_p1: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vs_p1 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/e698662c/Sensor.o: /home/tim/NetBeansProjects/VS_P1/Sensor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e698662c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e698662c/Sensor.o /home/tim/NetBeansProjects/VS_P1/Sensor.cpp

${OBJECTDIR}/_ext/e698662c/Server.o: /home/tim/NetBeansProjects/VS_P1/Server.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e698662c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e698662c/Server.o /home/tim/NetBeansProjects/VS_P1/Server.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/_ext/be8c287e/Test.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/_ext/be8c287e/Test.o: /home/tim/NetBeansProjects/VS_P1/tests/Test.cpp 
	${MKDIR} -p ${TESTDIR}/_ext/be8c287e
	${RM} "$@.d"
	$(COMPILE.cc) -g -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${TESTDIR}/_ext/be8c287e/Test.o /home/tim/NetBeansProjects/VS_P1/tests/Test.cpp


${OBJECTDIR}/_ext/e698662c/Sensor_nomain.o: ${OBJECTDIR}/_ext/e698662c/Sensor.o /home/tim/NetBeansProjects/VS_P1/Sensor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e698662c
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/e698662c/Sensor.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e698662c/Sensor_nomain.o /home/tim/NetBeansProjects/VS_P1/Sensor.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/e698662c/Sensor.o ${OBJECTDIR}/_ext/e698662c/Sensor_nomain.o;\
	fi

${OBJECTDIR}/_ext/e698662c/Server_nomain.o: ${OBJECTDIR}/_ext/e698662c/Server.o /home/tim/NetBeansProjects/VS_P1/Server.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e698662c
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/e698662c/Server.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e698662c/Server_nomain.o /home/tim/NetBeansProjects/VS_P1/Server.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/e698662c/Server.o ${OBJECTDIR}/_ext/e698662c/Server_nomain.o;\
	fi

${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -std=c++11 -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main_nomain.o main.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main.o ${OBJECTDIR}/main_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vs_p1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
