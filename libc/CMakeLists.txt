project(libk C)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.c)

add_library(k STATIC ${SRCS})
target_include_directories(k PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/../kernel/include)
target_compile_options(k PRIVATE "-ffreestanding")
target_compile_definitions(k PRIVATE __is_libk)
