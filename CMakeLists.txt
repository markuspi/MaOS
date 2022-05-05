cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

message("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")

set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
enable_language(ASM_NASM)

project(MaOS C CXX)

add_subdirectory(libc)
add_subdirectory(kernel)

add_custom_target(qemu COMMAND qemu-system-i386 -kernel "$<TARGET_FILE:kernel>" DEPENDS kernel)
add_custom_target(qemu-gdb COMMAND qemu-system-i386 -kernel "$<TARGET_FILE:kernel>" -s -S DEPENDS kernel)

add_custom_command(OUTPUT maos.build.iso
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/iso" "${CMAKE_BINARY_DIR}/iso"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:kernel>" "${CMAKE_BINARY_DIR}/iso/boot"
        COMMAND grub-mkrescue --output maos.build.iso "${CMAKE_BINARY_DIR}/iso" 2> mkrescue.log
        MAIN_DEPENDENCY kernel
        DEPENDS "${CMAKE_SOURCE_DIR}/iso/boot/grub/grub.cfg")
# We do this naming-dance so that the clion run configuration can find the build product
add_custom_target(maos.iso DEPENDS maos.build.iso COMMAND cp maos.build.iso maos.iso)
add_custom_target(qemu-iso COMMAND qemu-system-i386 -cdrom maos.iso DEPENDS maos.build.iso)
