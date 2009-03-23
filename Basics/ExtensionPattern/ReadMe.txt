Extension pattern example
--------------------------
This project contains 6 components.
The following 4 are built from the group directory
OriginalDll : an original implementation of a trivial DLL that contains a class that stores 2 numbers
OriginalClient : client code that links against OriginalDll.lib and demonstrates the functionality
ExtensionDll : an extension DLL that adds 2 functions to the class in OriginalDll to add & multiply the 2 numbers together
ExtensionClient : client code that links agains OriginalDll.lib and ExtensionDll.lib and demonstrates the new functionality
The following 2 are built from the group_v2 directory
OriginalDll_v2 : the same DLL as OriginalDll, but with some new functions added by the supplier
ExtensionDll_v2 : the same DLL as ExtensionDll.  Identical code.

Introduction
------------
OriginalDll has 6 functions in its DEF files, but has its header file modified to add 2 more public IMPORT_Cs.
(these are the "AddNumbers" and "MultiplyNumbers" functions).
There is also a 3rd export added, the "DoMultiplyNumbers" function.  This function is private, so cannot be called
by clients of OriginalDll.  The DoMultiplyNumbers function is necessary because it needs access to a const, KMagicMultiplyer,
which is defined in originaldll.dll and therefore can't be implemented elsewhere.  This is an example of an intrusive function.

The implementations of AddNumbers and MultiplyNumbers are in a different DLL - ExtensionDll
However this function is not implemented in OriginalDll.  This implementation of this function is in
the ExtensionDll project, and therefore the export appears in the ExtensionDll DEF files.

It is possible to use the 6 original exports in OriginalDll by simply linking against OriginalDll.lib
(this is demo'd in OriginalClient.exe).
However, in order to use the additional functions, the client must link to both OriginalDll.lib AND ExtensionDll.lib.
The function can be used just as normal, but is routed through the extension dll.  This is demo'd in ExtensionClient.exe

The OriginalDll_v2 and ExtensionDll_v2 projects demonstrate what happens when a supplier releases a new version of
their Dll with new ordinal functions.  OriginalDll_v2 adds some new supplier functions, which means that the extension
functions have to move in the DEF file.  This is taken care of by the extensionDll, which is rebuilt.  Once this is done,
the client code will work exactly as before, without needing a rebuild.  Therefore the extension to the functionality is
achieved without causing any BC breaks.


Building
--------
To build OriginalDll, ExtensionDll, OriginalClient & ExtensionClient:
- go to \ExtensionPattern\group
- type "bldmake bldfiles"
- type "abld build"

To build the OriginalDll_v2 and ExtensionDll_v2:
- first build the originals as above
- go to \ExtensionPattern\group_v2
- type "bldmake bldfiles"
- type "abld makefile"
- type "abld reallyclean"
- type "abld build"

(the cleaning step is necessary as this project will replace OriginalDll and ExtensionDll so need to be sure that
the previous versions are removed first)


Files supplied
--------------
ReadMe.txt - this file
eabi\EXTENSIONDLLU.DEF -  ARM DEF file for ExtensionDll
eabi\ORIGINALDLLU.DEF - ARM DEF file for originalDll
eabi\ORIGINALDLL_V2U.DEF - ARM DEF file for OriginalDll_v2 (containing new functions)
bwins\EXTENSIONDLLU.DEF - WINS DEF file for ExtensionDll
bwins\ORIGINALDLLU.DEF - WINS DEF file for originalDll
bwins\ORIGINALDLL_V2U.DEF - WINS DEF file for OriginalDll_v2 (containing new functions)
group\bld.inf - Makefile for OriginalDll, ExtensionDll, OriginalClient & ExtensionClient
group\ExtensionClient.mmp - Project definition for ExtensionClient
group\ExtensionDll.mmp - Project definition for ExtensionDll
group\OriginalClient.mmp - Project definition for OriginalClient
group\OriginalDll.mmp - Project definition for OriginalDll
group_V2\bld.inf - Makefile for OriginalDll_v2 and ExtensionDll_v2
group_V2\ExtensionDll_v2.mmp - Project definition for ExtensionDll_v2
group_V2\OriginalDll_v2.mmp - Project definition for OriginalDll_v2
include\NumberStore.h - header file for CNumberStore class
include\NumberStore_v2.h - header file for version 2 of CNumberStore class
src_extension\NumberStoreExtensionImplementation.cpp - Source for ExtensionDll
src_extensionclient\ExtensionClient.cpp - Source for ExtensionDll client console app
src_extension_v2\NumberStoreExtensionImplementation_v2.cpp - Source for ExtensionDll_v2
src_original\NumberStoreOriginalImplementation.cpp - Source for OriginalDll
src_originalclient\OriginalClient.cpp - Source for OriginalDll client console app
src_original_v2\NumberStoreOriginalImplementation_v2.cpp - Source for OriginalDll_v2

