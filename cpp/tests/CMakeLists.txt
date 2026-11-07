add_executable(test_sgp4 test_sgp4.cpp)
target_link_libraries(test_sgp4 PRIVATE adcs_core)
add_test(NAME test_sgp4 COMMAND test_sgp4)

# CelesTrak pull policy, cache fallback and propagation (fake HTTP and clock, no network)
add_executable(test_contact_state test_contact_state.cpp)
target_link_libraries(test_contact_state PRIVATE adcs_contact)
add_test(NAME test_contact_state COMMAND test_contact_state)
