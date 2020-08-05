add_executable(job-executor
    job.cpp
    job_container.cpp
    main.cpp
    )

target_link_libraries(job-executor
    ${YAML_CPP_LIBRARIES}
    )
