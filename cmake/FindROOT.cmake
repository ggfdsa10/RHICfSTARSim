# Find the ROOT installation
find_path(ROOT_INCLUDE_DIR NAMES TROOT.h PATHS ${ROOTSYS}/include)
find_library(ROOT_LIBRARIES NAMES Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc Minuit2 Spectrum PATHS ${ROOTSYS}/lib)

# Handle the standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ROOT DEFAULT_MSG ROOT_LIBRARIES ROOT_INCLUDE_DIR)

# Mark variables as advanced
mark_as_advanced(ROOT_INCLUDE_DIR ROOT_LIBRARIES)

include("${ROOT_INCLUDE_DIR}/../cmake/modules/RootNewMacros.cmake")

#set(ROOT_DEFINITIONS "")
#set(ROOT_CXX_FLAGS " -pipe -m32 -fsigned-char -pthread -std=c++11")
#set(ROOT_C_FLAGS " -pipe -m32 -pthread")
#set(ROOT_fortran_FLAGS " -m32 -std=legacy")
#set(ROOT_EXE_LINKER_FLAGS " -rdynamic")

execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --ldflags OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ROOT_LD_FLAGS}")
set(ROOT_LIBRARIES ${ROOT_LIBS})
