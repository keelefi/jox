add_executable(jox
    job.cpp
    job_container.cpp
    main.cpp
    scheme.cpp
    )

target_include_directories(jox PRIVATE
    ${GUILE_INCLUDE_DIRS}
    )

target_link_libraries(jox
    ${YAML_CPP_LIBRARIES}
    ${GUILE_LINK_LIBRARIES}
    )
