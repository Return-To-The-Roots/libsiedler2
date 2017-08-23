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
    set(ClangFormat_SRC_EXTENSIONS "*.cpp;*.h;*.hpp;*.c;*.tpp" CACHE STRING "Extensions to consider for add_ClangFormat_folder")
    mark_as_advanced(ClangFormat_SRC_EXTENSIONS)
    function(add_ClangFormat_files _files)
        foreach(curFile ${ARGN})
            set(ClangFormat_FILES ${ClangFormat_FILES} "${curFile}" CACHE INTERNAL "")
        endforeach()
    endfunction()
    function(add_ClangFormat_folder _folder _recursive)
        set(_tmpGLOB "")
        string(REPLACE " " ";" _exts "${ClangFormat_SRC_EXTENSIONS}")
        foreach(_ext ${_exts})
            list(APPEND _tmpGLOB "${_folder}/${_ext}")
        endforeach()
        if(_recursive)
            file(GLOB_RECURSE _NewClangFormatFiles ${_tmpGLOB})
        else()
            file(GLOB _NewClangFormatFiles ${_tmpGLOB})
        endif()
        if(_NewClangFormatFiles)
            add_ClangFormat_files(${_NewClangFormatFiles})
        endif()
    endfunction()
    function(print_ClangFormat_files)
        message(STATUS "Files for clang-format: ${ClangFormat_FILES}")
    endfunction()
    function(add_ClangFormat_target _style)
        set(_sources "")
        foreach (_source ${ClangFormat_FILES})
            if (NOT TARGET ${_source})
                get_filename_component(_source_file ${_source} NAME)
                get_source_file_property(_clang_loc "${_source}" LOCATION)
 
                set(_format_file clangFormat/${_source_file}.format)
 
                add_custom_command(OUTPUT ${_format_file}
                        DEPENDS ${_source}
                        COMMENT "Clang-Format ${_source}"
                        COMMAND ${ClangFormat_BINARY} -style=${_style} -i ${_clang_loc}
                        COMMAND ${CMAKE_COMMAND} -E touch ${_format_file})
 
                list(APPEND _sources ${_format_file})
            endif ()
        endforeach ()
 
        if (_sources)
            add_custom_target(clangFormat
                    SOURCES ${_sources}
                    COMMENT "Clang-Format for target ${_target}")
        endif ()
    endfunction()
endif()
