cmake_minimum_required(VERSION 3.1.1)
project(kvr)

option (KVR_BUILD_TESTS "build tests" ON)

set (KVR_CXX_FLAGS_BASE "-Wall -std=c++03")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ${KVR_CXX_FLAGS_BASE}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${KVR_CXX_FLAGS_BASE}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${KVR_CXX_FLAGS_BASE}")
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})

if (OFF)
  message ("debug flags ${CMAKE_CXX_FLAGS_DEBUG}")
  message ("optim flags ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  message ("relea flags ${CMAKE_CXX_FLAGS_RELEASE}")
  message ("none  flags ${CMAKE_CXX_FLAGS}")
endif ()

set (KVR_SOURCE_DIR "src")
set (KVR_SOURCES "${KVR_SOURCE_DIR}/kvr.cpp ${KVR_SOURCE_DIR}/internal/kvr_json.cpp ${KVR_SOURCE_DIR}/internal/kvr_msgpack.cpp")
separate_arguments (KVR_SOURCES)
include_directories (${KVR_SOURCE_DIR})
add_library (kvr STATIC ${KVR_SOURCES})

if (KVR_BUILD_TESTS)
  option (KVR_UNIT_TEST_ENABLED "unit tests" ON)
  option (KVR_PERF_TEST_ENABLED "performance tests" ON)

  if (KVR_UNIT_TEST_ENABLED)
    set (KVR_CXXTEST_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test/3rdparty/cxxtest-4.3")
    set (CXXTEST_INCLUDE_DIR "${KVR_CXXTEST_INSTALL_DIR}")
    set (CXXTEST_PYTHON_TESTGEN_EXECUTABLE "${KVR_CXXTEST_INSTALL_DIR}/bin/cxxtestgen")
    find_package (CxxTest)
    if (CXXTEST_FOUND)
      #set (CXXTEST_TESTGEN_ARGS "--fog-parser --error-printer")
      separate_arguments (CXXTEST_TESTGEN_ARGS)
      enable_testing ()
      include_directories (${CXXTEST_INCLUDE_DIR})

      set (KVR_UNIT_TEST_LIST api diff_patch)
      foreach (utest ${KVR_UNIT_TEST_LIST})
        cxxtest_add_test (unit_test_${utest} ${utest}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/${utest}.h)
        target_link_libraries (unit_test_${utest} kvr)
      endforeach ()

      #cxxtest_add_test (unit_test_hello hello.cpp "${CMAKE_CURRENT_SOURCE_DIR}/test/unit/hello.h")
    endif ()
  endif ()

  if (KVR_PERF_TEST_ENABLED)
    set (KVR_PERF_TEST_LIST basic)
    foreach (ptest ${KVR_PERF_TEST_LIST})
      add_executable (perf_test_${ptest} ${CMAKE_CURRENT_SOURCE_DIR}/test/perf/${ptest}.cpp)
      set_target_properties (perf_test_${ptest} PROPERTIES COMPILE_FLAGS "-O0 -g")
      target_link_libraries (perf_test_${ptest} kvr)
    endforeach ()

    #add_executable (perfhello "${CMAKE_CURRENT_SOURCE_DIR}/test/perf/hello.cpp")
    #set_target_properties (perfhello PROPERTIES COMPILE_FLAGS "-O0 -g")

    enable_testing ()
    #need FindValgrind.cmake
    set (CMAKE_MEMORYCHECK_COMMAND valgrind)
    set (CMAKE_MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 --leak-check=full --show-reachable=yes --track-origins=yes -v")
    set (KVR_MEMCHECK_CMD "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS}")
    separate_arguments (KVR_MEMCHECK_CMD)
    
    foreach (ptest ${KVR_PERF_TEST_LIST})
      add_test (perf_test_memchk_${ptest} ${KVR_MEMCHECK_CMD} ./perf_test_${ptest})
    endforeach ()
    
    #add_test (perf_mem_hello ${KVR_MEMCHECK_CMD} ./perfhello) 
  endif ()
endif (KVR_BUILD_TESTS)
