# ============================================================================
#  Name	 : build_help.mk
#  Part of  : Direct
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : Direct
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : Direct_0xE4698408.hlp
Direct_0xE4698408.hlp : Direct.xml Direct.cshlp Custom.xml
	cshlpcmp Direct.cshlp
ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
	md $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
	copy Direct_0xE4698408.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del Direct_0xE4698408.hlp
	del Direct_0xE4698408.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo Direct_0xE4698408.hlp

FINAL : do_nothing
