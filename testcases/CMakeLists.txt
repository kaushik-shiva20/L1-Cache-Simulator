add_executable(testcase0 testcase0.cc)
target_link_libraries(testcase0 sim_cache)
add_custom_command(TARGET testcase0 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase0>/traces)

add_executable(testcase1 testcase1.cc)
target_link_libraries(testcase1 sim_cache)
add_custom_command(TARGET testcase1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase1>/traces)

add_executable(testcase2 testcase2.cc)
target_link_libraries(testcase2 sim_cache)
add_custom_command(TARGET testcase2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase2>/traces)

add_executable(testcase3 testcase3.cc)
target_link_libraries(testcase3 sim_cache)
add_custom_command(TARGET testcase3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase3>/traces)

add_executable(testcase4 testcase4.cc)
target_link_libraries(testcase4 sim_cache)
add_custom_command(TARGET testcase4 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase4>/traces)

add_executable(testcase5 testcase5.cc)
target_link_libraries(testcase5 sim_cache)
add_custom_command(TARGET testcase5 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcase5>/traces)

add_executable(testcaseReport testcaseReport.cc)
target_link_libraries(testcaseReport sim_cache)
add_custom_command(TARGET testcaseReport POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/traces"
        $<TARGET_FILE_DIR:testcaseReport>/traces)