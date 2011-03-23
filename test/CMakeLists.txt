
find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-limit=yes --leak-resolution=low")

add_executable(TestOne TestOne.cpp)
add_executable(01_module_loader 01_module_loader.cpp)
add_executable(02_module_linker 02_module_linker.cpp)
add_executable(03_module_handler 03_module_handler.cpp)

target_link_libraries(TestOne shine ${GLIB2_LIBRARIES})
target_link_libraries(01_module_loader shine ${GLIB2_LIBRARIES})
target_link_libraries(02_module_linker shine ${GLIB2_LIBRARIES})
target_link_libraries(03_module_handler shine ${GLIB2_LIBRARIES})

add_test(TestOne TestOne)

add_test(01_module_loader 01_module_loader)
add_test(02_module_linker 02_module_linker)
add_test(03_module_handler 03_module_handler)

set(TEST_FILE_EXTRA mod1.c)

foreach(TEST_EXTRA ${TEST_FILE_EXTRA})
    ADD_CUSTOM_COMMAND(
        TARGET TestOne
        POST_BUILD
        COMMAND clang
        ARGS -c -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_EXTRA}
        COMMENT "Creating LLVM bitcode extra module (${TEST_EXTRA})..."
    )
endforeach()

#ADD_CUSTOM_COMMAND(
#    TARGET TestOne
#    POST_BUILD
#    COMMAND clang
#    ARGS -c -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/mod1.c -o ${CMAKE_CURRENT_BINARY_DIR}/mod1.bc
#    COMMENT "Creating LLVM bitcode test modules..."
#)

#ADD_CUSTOM_COMMAND(
#    TARGET TestOne
#    POST_BUILD
#    COMMAND clang
#    ARGS -c -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/mod2.c -o ${CMAKE_CURRENT_BINARY_DIR}/mod2.bc
#    COMMENT "Creating LLVM bitcode test modules..."
#)
