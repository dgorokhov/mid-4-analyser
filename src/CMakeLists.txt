cmake_minimum_required(VERSION 3.30)

# Прокидываем глобальную include-папку для всех внутренних библиотек
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# иблиотека опций командной строки
add_library(cmd_options STATIC
    cmd_options.cpp
)
# Ликуем с Boost, чтобы cmd_options видела <boost/program_options.hpp>
target_link_libraries(cmd_options PUBLIC Boost::program_options)

# библиотека ядра анализатора и его метрик
add_library(analyzer_core STATIC
    analyzer.cpp
    function.cpp
)

# библиотека аккумуляторов
add_library(metric_accumulator STATIC
    metric_accumulator.cpp
)

# Линкуем аккумуляторы с ядром
target_link_libraries(metric_accumulator PUBLIC analyzer_core)


# ЕДИНЫЙ ТАРГЕТ ТЕСТОВ
set(target metric_test)

add_executable(${target}
    tests.cpp
)

target_link_libraries(${target}
    PRIVATE
        GTest::GTest
        GTest::Main
        analyzer_core
        metric_accumulator
)

# Копирование тестовых файлов Python в бинарную директорию сборки
file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/metric_impl/tests/files/*.py")
if(NOT test_files)
    file(GLOB test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/files/*.py")
endif()

foreach(file IN LISTS test_files)
    get_filename_component(file_name ${file} NAME)
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/src/metric_impl/tests/files/${file_name}" COPYONLY)
    configure_file(${file} "${CMAKE_CURRENT_BINARY_DIR}/tests/files/${file_name}" COPYONLY)
endforeach()

add_test(NAME ${target} COMMAND ${target})
