# Finds the clang-format executable and
# sets ClangFormat_FOUND and ClangFormat_Version
#
# Defines the following macros:
#   add_ClangFormat_files(files)
#       Add files to the list of files to be formatted.
#   add_ClangFormat_folder(folder recursive)
#       Add all files matching the glob in ClangFormat_SRC_EXTENSIONS to the
#       list of files to be formatted.
#       If recursive is true, subfolders will be added too.
#   print_ClangFormat_files()
#       Print the current list of files as a STATUS message.
#   add_ClangFormat_target(style)
#       Adds a new target "clangFormat" that will format all files in the list
#       Allowed values for style are e.g.: LLVM, Google, Chromium, Mozilla, Webkit
#       and file to use a .clang-format file in one of the srcs parent directory

find_program(ClangFormat_BINARY
    NAMES
    clang-format-5.0
    clang-format-4.0
    clang-format-3.9
    clang-format-3.8
    clang-format-3.7
    clang-format-3.6
    clang-format
    PATHS ${ClangTools_PATH} /usr/local/bin /usr/bin ENV CLANG_TOOLS_PATH
)
if(ClangFormat_BINARY)
    execute_process(COMMAND ${ClangFormat_BINARY} --version OUTPUT_VARIABLE _out)
    string(REGEX REPLACE ".*clang-format version ([^ \n]+).*" "\\1" ClangFormat_VERSION_STRING "${_out}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat
    FOUND_VAR ClangFormat_FOUND
    REQUIRED_VARS ClangFormat_BINARY
    VERSION_VAR ClangFormat_VERSION_STRING)

if(ClangFormat_FOUND)
    set(ClangFormat_FILES "" CACHE INTERNAL "")
    set(ClangFormat_SRC_EXTENSIONS "*.cpp *.h *.hpp *.c *.tpp" CACHE STRING "Extensions to consider for add_ClangFormat_folder")
    mark_as_advanced(ClangFormat_SRC_EXTENSIONS)
    macro(add_ClangFormat_files _files)
        foreach(curFile ${ARGN})
            set(ClangFormat_FILES ${ClangFormat_FILES} "${curFile}" CACHE INTERNAL "")
        endforeach()
    endmacro()
    macro(add_ClangFormat_folder _folder _recursive)
        if(_recursive)
            file(GLOB_RECURSE _NewClangFormatFiles ${_folder} ${ClangFormat_SRC_EXTENSIONS})
        else()
            file(GLOB _NewClangFormatFiles ${_folder} ${ClangFormat_SRC_EXTENSIONS})
        endif()
        add_ClangFormat_files(${_NewClangFormatFiles})
    endmacro()
    macro(print_ClangFormat_files)
        message(STATUS "Files for clang-format: ${ClangFormat_FILES}")
    endmacro()
    macro(add_ClangFormat_target _style)
        add_custom_target(clangFormat
            COMMAND ${ClangFormat_BINARY}
            -style=${_style}
            -i
            ${ClangFormat_FILES}
        )
    endmacro()
endif()
