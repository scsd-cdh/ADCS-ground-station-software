add_executable(test_sgp4 test_sgp4.cpp)
target_link_libraries(test_sgp4 PRIVATE adcs_core)
add_test(NAME test_sgp4 COMMAND test_sgp4)

# Reference numbers exported from MATLAB (matlab/tests/export_cpp_fixture.m)
add_executable(test_windows test_windows.cpp)
target_link_libraries(test_windows PRIVATE adcs_windows)
target_compile_definitions(test_windows PRIVATE ADCS_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_test(NAME test_windows COMMAND test_windows)
