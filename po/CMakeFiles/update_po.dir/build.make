# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_SOURCE_DIR = /home/djemos/multibootusb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/djemos/multibootusb

# Utility rule file for update_po.

# Include the progress variables for this target.
include po/CMakeFiles/update_po.dir/progress.make

update_po: po/CMakeFiles/update_po.dir/build.make
	cd /home/djemos/multibootusb/po && /usr/bin/xgettext -o /home/djemos/multibootusb/po/multibootusb.pot -L Glade /home/djemos/multibootusb/src/multibootusb.ui
	cd /home/djemos/multibootusb/po && /usr/bin/xgettext -o /home/djemos/multibootusb/po/multibootusb.pot -j /home/djemos/multibootusb/src/multibootusb.c
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/de.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/el.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/es.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/fr.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/pt.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/ro.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/ru.po /home/djemos/multibootusb/po/multibootusb.pot
	cd /home/djemos/multibootusb/po && /usr/bin/msgmerge --backup none -U /home/djemos/multibootusb/po/sv.po /home/djemos/multibootusb/po/multibootusb.pot
.PHONY : update_po

# Rule to build all files generated by this target.
po/CMakeFiles/update_po.dir/build: update_po

.PHONY : po/CMakeFiles/update_po.dir/build

po/CMakeFiles/update_po.dir/clean:
	cd /home/djemos/multibootusb/po && $(CMAKE_COMMAND) -P CMakeFiles/update_po.dir/cmake_clean.cmake
.PHONY : po/CMakeFiles/update_po.dir/clean

po/CMakeFiles/update_po.dir/depend:
	cd /home/djemos/multibootusb && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/djemos/multibootusb /home/djemos/multibootusb/po /home/djemos/multibootusb /home/djemos/multibootusb/po /home/djemos/multibootusb/po/CMakeFiles/update_po.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : po/CMakeFiles/update_po.dir/depend

