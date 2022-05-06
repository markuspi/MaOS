project(libmath C CXX)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/*.asm ${PROJECT_SOURCE_DIR}/*.c ${PROJECT_SOURCE_DIR}/*.cpp)

add_library(math STATIC ${SRCS})

target_include_directories(math PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(math PRIVATE $<$<COMPILE_LANGUAGE:C>:-ffreestanding> $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding -fno-exceptions -fno-rtti>)
