# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bohr/smartquant/smartquant

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bohr/smartquant/smartquant/build

# Include any dependencies generated for this target.
include gateway/CMakeFiles/gateway.dir/depend.make

# Include the progress variables for this target.
include gateway/CMakeFiles/gateway.dir/progress.make

# Include the compile flags for this target's objects.
include gateway/CMakeFiles/gateway.dir/flags.make

gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.o: ../gateway/ctp/ctp_gateway.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/ctp/ctp_gateway.o -c /home/bohr/smartquant/smartquant/gateway/ctp/ctp_gateway.cpp

gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/ctp/ctp_gateway.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/ctp/ctp_gateway.cpp > CMakeFiles/gateway.dir/ctp/ctp_gateway.i

gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/ctp/ctp_gateway.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/ctp/ctp_gateway.cpp -o CMakeFiles/gateway.dir/ctp/ctp_gateway.s

gateway/CMakeFiles/gateway.dir/ctp/ctpmd.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/ctp/ctpmd.o: ../gateway/ctp/ctpmd.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object gateway/CMakeFiles/gateway.dir/ctp/ctpmd.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/ctp/ctpmd.o -c /home/bohr/smartquant/smartquant/gateway/ctp/ctpmd.cpp

gateway/CMakeFiles/gateway.dir/ctp/ctpmd.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/ctp/ctpmd.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/ctp/ctpmd.cpp > CMakeFiles/gateway.dir/ctp/ctpmd.i

gateway/CMakeFiles/gateway.dir/ctp/ctpmd.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/ctp/ctpmd.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/ctp/ctpmd.cpp -o CMakeFiles/gateway.dir/ctp/ctpmd.s

gateway/CMakeFiles/gateway.dir/ctp/ctptd.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/ctp/ctptd.o: ../gateway/ctp/ctptd.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object gateway/CMakeFiles/gateway.dir/ctp/ctptd.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/ctp/ctptd.o -c /home/bohr/smartquant/smartquant/gateway/ctp/ctptd.cpp

gateway/CMakeFiles/gateway.dir/ctp/ctptd.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/ctp/ctptd.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/ctp/ctptd.cpp > CMakeFiles/gateway.dir/ctp/ctptd.i

gateway/CMakeFiles/gateway.dir/ctp/ctptd.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/ctp/ctptd.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/ctp/ctptd.cpp -o CMakeFiles/gateway.dir/ctp/ctptd.s

gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.o: ../gateway/stock_level2/l2_gateway.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/stock_level2/l2_gateway.o -c /home/bohr/smartquant/smartquant/gateway/stock_level2/l2_gateway.cpp

gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/stock_level2/l2_gateway.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/stock_level2/l2_gateway.cpp > CMakeFiles/gateway.dir/stock_level2/l2_gateway.i

gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/stock_level2/l2_gateway.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/stock_level2/l2_gateway.cpp -o CMakeFiles/gateway.dir/stock_level2/l2_gateway.s

gateway/CMakeFiles/gateway.dir/stock_level2/l2md.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/stock_level2/l2md.o: ../gateway/stock_level2/l2md.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object gateway/CMakeFiles/gateway.dir/stock_level2/l2md.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/stock_level2/l2md.o -c /home/bohr/smartquant/smartquant/gateway/stock_level2/l2md.cpp

gateway/CMakeFiles/gateway.dir/stock_level2/l2md.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/stock_level2/l2md.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/stock_level2/l2md.cpp > CMakeFiles/gateway.dir/stock_level2/l2md.i

gateway/CMakeFiles/gateway.dir/stock_level2/l2md.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/stock_level2/l2md.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/stock_level2/l2md.cpp -o CMakeFiles/gateway.dir/stock_level2/l2md.s

gateway/CMakeFiles/gateway.dir/stock_level2/l2td.o: gateway/CMakeFiles/gateway.dir/flags.make
gateway/CMakeFiles/gateway.dir/stock_level2/l2td.o: ../gateway/stock_level2/l2td.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object gateway/CMakeFiles/gateway.dir/stock_level2/l2td.o"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gateway.dir/stock_level2/l2td.o -c /home/bohr/smartquant/smartquant/gateway/stock_level2/l2td.cpp

gateway/CMakeFiles/gateway.dir/stock_level2/l2td.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gateway.dir/stock_level2/l2td.i"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bohr/smartquant/smartquant/gateway/stock_level2/l2td.cpp > CMakeFiles/gateway.dir/stock_level2/l2td.i

gateway/CMakeFiles/gateway.dir/stock_level2/l2td.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gateway.dir/stock_level2/l2td.s"
	cd /home/bohr/smartquant/smartquant/build/gateway && /bin/x86_64-linux-gnu-g++-8 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bohr/smartquant/smartquant/gateway/stock_level2/l2td.cpp -o CMakeFiles/gateway.dir/stock_level2/l2td.s

# Object files for target gateway
gateway_OBJECTS = \
"CMakeFiles/gateway.dir/ctp/ctp_gateway.o" \
"CMakeFiles/gateway.dir/ctp/ctpmd.o" \
"CMakeFiles/gateway.dir/ctp/ctptd.o" \
"CMakeFiles/gateway.dir/stock_level2/l2_gateway.o" \
"CMakeFiles/gateway.dir/stock_level2/l2md.o" \
"CMakeFiles/gateway.dir/stock_level2/l2td.o"

# External object files for target gateway
gateway_EXTERNAL_OBJECTS =

gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/ctp/ctp_gateway.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/ctp/ctpmd.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/ctp/ctptd.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/stock_level2/l2_gateway.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/stock_level2/l2md.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/stock_level2/l2td.o
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/build.make
gateway/libgateway.a: gateway/CMakeFiles/gateway.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bohr/smartquant/smartquant/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX static library libgateway.a"
	cd /home/bohr/smartquant/smartquant/build/gateway && $(CMAKE_COMMAND) -P CMakeFiles/gateway.dir/cmake_clean_target.cmake
	cd /home/bohr/smartquant/smartquant/build/gateway && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gateway.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
gateway/CMakeFiles/gateway.dir/build: gateway/libgateway.a

.PHONY : gateway/CMakeFiles/gateway.dir/build

gateway/CMakeFiles/gateway.dir/clean:
	cd /home/bohr/smartquant/smartquant/build/gateway && $(CMAKE_COMMAND) -P CMakeFiles/gateway.dir/cmake_clean.cmake
.PHONY : gateway/CMakeFiles/gateway.dir/clean

gateway/CMakeFiles/gateway.dir/depend:
	cd /home/bohr/smartquant/smartquant/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bohr/smartquant/smartquant /home/bohr/smartquant/smartquant/gateway /home/bohr/smartquant/smartquant/build /home/bohr/smartquant/smartquant/build/gateway /home/bohr/smartquant/smartquant/build/gateway/CMakeFiles/gateway.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gateway/CMakeFiles/gateway.dir/depend

