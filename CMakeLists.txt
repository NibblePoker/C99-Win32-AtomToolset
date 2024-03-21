# Configuring CMake
message("--> Configuring CMake...")

# Configuring CMake > Checking minimum version
message("---> Checking CMake's version...")
message("----> Require 3.16")
message("----> Using ${CMAKE_VERSION}")
cmake_minimum_required(VERSION 3.16)

# Configuring CMake > Selecting compiler
message("---> Checking OS type...")
if (UNIX)
    fatal_error("----> Found UNIX system, please use a Windows system to compile this project !")
else ()
    message("----> Found non-UNIX system, letting CMake autodetect compiler")
endif (UNIX)
message("----> Compiler: ${CMAKE_C_COMPILER}")

# Configuring CMake > Preparing C revision and project itself...
message("---> Setting language to \"C\" and revision to \"C99\" without extensions")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

# Setting up the CMake project itself
project("NibblePoker's Utility" C)

# Dependencies
message("--> Processing dependencies...")

# Adding FetchContent to grab my library out on GitHub
message("---> Including FetchContent...")
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)


message("--> Checking for platform-specific arguments for the compiler")
if (CYGWIN)
    # Does not work, God I hate Cygwin...
    # At least you don't get a multi-MiB file because dead code removal is hard, aww...
    # See: https://cygwin.com/faq/faq.html#faq.programming.static-linking
    #message("---> Detected Cygwin, adding \"-static\" to \"CMAKE_C_FLAGS\"")
    #set(CMAKE_C_FLAGS "-static ${CMAKE_C_FLAGS}")
    message("---> Detected Cygwin.")
    message("---> Good luck, you're on your own there bud, I'm not touching that shit...")
elseif (MSVC)
    message("---> Detected MSVC compiler, adding custom flags to compiler and linker...")

    # /TC - Specifies all source files are C.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Gy /Oy /TC")

    # /Od -> Disables optimization.
    # /Ot -> Favors fast code.
    # /Os -> Favors small code.
    set(CMAKE_C_FLAGS_DEBUG "/Od ${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE "/Ot ${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os ${CMAKE_C_FLAGS_MINSIZEREL}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ot ${CMAKE_C_FLAGS_RELWITHDEBINFO}")

    # /ALIGN:x     -> Align final executable's section to multiples of x bytes.
    # /FILEALIGN:x -> Forces section alignment to multiples of x bytes.
    # /OPT:REF     -> Eliminates functions and data that are never referenced.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ALIGN:128 /FILEALIGN:16 /OPT:REF")

    # /CGTHREADS:x -> Makes 'cl.exe' use x thread(s).
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /CGTHREADS:8")

    # /GUARD:CF       -> Enables Control Flow Guard protection. (Prevents UPX compression !)
    # /DYNAMICBASE:NO -> Disables ASLR.  (Not included due to ARM/ARM64 builds)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /GUARD:CF")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /GUARD:CF /DYNAMICBASE:NO")

    # /RELEASE -> Sets the Checksum in the header of final executable.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /RELEASE")

    # https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170
    # Oh my !
else ()
    message("---> No actions taken")
endif ()


# Adding our source files
message("--> Preparing targets...")

file(GLOB_RECURSE src_np_c99_goodies CONFIGURE_DEPENDS "libs/C99-Utility-Libraries/src/*.h" "libs/C99-Utility-Libraries/src/*.c")
file(GLOB_RECURSE src_np_atom_toolset CONFIGURE_DEPENDS "src/*.h" "src/*.c")

message("---> Defining 'AtomUtil'...")
add_executable(AtomUtil ${src_np_c99_goodies} ${src_np_atom_toolset} "rsc/rc/app.rc")
set_target_properties(AtomUtil PROPERTIES OUTPUT_NAME "AtomUtil")

target_compile_definitions(AtomUtil PRIVATE -D_UNICODE -DUNICODE) # Enabling unicode variants for Win32Api calls
set_target_properties(AtomUtil PROPERTIES LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wWinMainCRTStartup /SUBSYSTEM:CONSOLE")

target_compile_definitions(AtomUtil PUBLIC NP_GOODIES_ARGUMENTS_USE_WCHAR) # Must be the same as pre-FetchContent def.
target_compile_definitions(AtomUtil PUBLIC NP_DEBUG_LOGGING)  # Enables debugging and optional error logging for debug builds.
target_include_directories(AtomUtil PUBLIC "libs/C99-Utility-Libraries/src")

# Defining constants to let the help text printer work properly.
target_compile_definitions(AtomUtil PUBLIC NP_WIN32)
target_compile_definitions(AtomUtil PUBLIC NP_ARGS_WCHAR)
