# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.13.4/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.13.4/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kramerelwell/Projects/201B/allolib_playground

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/build/Ass1/Release

# Utility rule file for Ass1_run.

# Include the progress variables for this target.
include CMakeFiles/Ass1_run.dir/progress.make

CMakeFiles/Ass1_run: ../../../bin/Ass1
	cd /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/bin && ./Ass1

Ass1_run: CMakeFiles/Ass1_run
Ass1_run: CMakeFiles/Ass1_run.dir/build.make

.PHONY : Ass1_run

# Rule to build all files generated by this target.
CMakeFiles/Ass1_run.dir/build: Ass1_run

.PHONY : CMakeFiles/Ass1_run.dir/build

CMakeFiles/Ass1_run.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Ass1_run.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Ass1_run.dir/clean

CMakeFiles/Ass1_run.dir/depend:
	cd /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/build/Ass1/Release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kramerelwell/Projects/201B/allolib_playground /Users/kramerelwell/Projects/201B/allolib_playground /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/build/Ass1/Release /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/build/Ass1/Release /Users/kramerelwell/Projects/201B/allolib_playground/kramerelwell/build/Ass1/Release/CMakeFiles/Ass1_run.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Ass1_run.dir/depend

