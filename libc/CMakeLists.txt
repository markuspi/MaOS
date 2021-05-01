project(libk C CXX)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp)

add_library(k STATIC ${SRCS})
target_include_directories(k PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/../kernel/include)
target_compile_options(k PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding> $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding -fno-exceptions -fno-rtti>)
target_compile_definitions(k PRIVATE __is_libk)
