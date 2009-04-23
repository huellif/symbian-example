# ============================================================================
#  Name	 : build_help.mk
#  Part of  : hellosis
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : hellosis
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : hellosis_0xE2047FAE.hlp
hellosis_0xE2047FAE.hlp : hellosis.xml hellosis.cshlp Custom.xml
	cshlpcmp hellosis.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy hellosis_0xE2047FAE.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del hellosis_0xE2047FAE.hlp
	del hellosis_0xE2047FAE.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo hellosis_0xE2047FAE.hlp

FINAL : do_nothing
