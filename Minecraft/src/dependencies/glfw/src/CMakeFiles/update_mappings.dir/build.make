# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/cmake/1429/bin/cmake

# The command to remove a file.
RM = /snap/cmake/1429/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/krystian/Minecraft

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/krystian/Minecraft

# Utility rule file for update_mappings.

# Include any custom commands dependencies for this target.
include Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/compiler_depend.make

# Include the progress variables for this target.
include Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/progress.make

Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/krystian/Minecraft/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Updating gamepad mappings from upstream repository"
	cd /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/src && /snap/cmake/1429/bin/cmake -P /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/CMake/GenerateMappings.cmake mappings.h.in mappings.h

Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/codegen:
.PHONY : Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/codegen

update_mappings: Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings
update_mappings: Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/build.make
.PHONY : update_mappings

# Rule to build all files generated by this target.
Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/build: update_mappings
.PHONY : Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/build

Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/clean:
	cd /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/src && $(CMAKE_COMMAND) -P CMakeFiles/update_mappings.dir/cmake_clean.cmake
.PHONY : Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/clean

Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/depend:
	cd /home/krystian/Minecraft && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/krystian/Minecraft /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/src /home/krystian/Minecraft /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/src /home/krystian/Minecraft/Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : Minecraft/src/dependencies/glfw/src/CMakeFiles/update_mappings.dir/depend

