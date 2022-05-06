project(kernel)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.asm ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(kernel ${SRCS} include/kernel/types.h)
target_compile_options(kernel PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding -std=gnu99> $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding -fno-exceptions -fno-rtti>)
target_include_directories(kernel PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(kernel PRIVATE k math)
target_link_options(kernel PRIVATE -nostdlib -T ${PROJECT_SOURCE_DIR}/linker.ld)
