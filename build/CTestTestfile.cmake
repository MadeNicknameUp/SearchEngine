# CMake generated Testfile for 
# Source directory: /home/xrd/Projects/SkillboxProjects/SearchEngine
# Build directory: /home/xrd/Projects/SkillboxProjects/SearchEngine/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(AllTests "/home/xrd/Projects/SkillboxProjects/SearchEngine/build/tests")
set_tests_properties(AllTests PROPERTIES  _BACKTRACE_TRIPLES "/home/xrd/Projects/SkillboxProjects/SearchEngine/CMakeLists.txt;51;add_test;/home/xrd/Projects/SkillboxProjects/SearchEngine/CMakeLists.txt;0;")
subdirs("extern/json")
subdirs("extern/googletest")
