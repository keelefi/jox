add_executable(jox
    job.cpp
    job_container.cpp
    main.cpp
    )

target_link_libraries(jox
    ${YAML_CPP_LIBRARIES}
    )
