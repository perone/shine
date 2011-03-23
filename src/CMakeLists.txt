set(SHINE_SRC
    moduleloader.cpp
    modulelinker.cpp
    modulehandler.cpp
    astnode.cpp
    shine.cpp
)

add_library(shine SHARED ${SHINE_SRC})
set_target_properties(shine PROPERTIES VERSION 0.0.1 SOVERSION 1)

target_link_libraries(shine ${REQ_LLVM_LIBRARIES} ${EXTRA_LIBRARIES})
set_target_properties(shine PROPERTIES LINKER_LANGUAGE CXX)

INSTALL(TARGETS shine
        DESTINATION lib)
