project(kernel)

set(ARCH_DIR ${PROJECT_SOURCE_DIR}/arch/x86)
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.asm ${PROJECT_SOURCE_DIR}/*.c)

add_executable(kernel ${SRCS})
target_compile_options(kernel PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding>)
target_include_directories(kernel PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(kernel PRIVATE k)
target_link_options(kernel PRIVATE -nostdlib -T ${ARCH_DIR}/linker.ld)

add_custom_target(qemu qemu-system-i386 -kernel "$<TARGET_FILE:kernel>" DEPENDS kernel)
add_custom_target(qemu-gdb qemu-system-i386 -kernel "$<TARGET_FILE:kernel>" -s -S DEPENDS kernel)
