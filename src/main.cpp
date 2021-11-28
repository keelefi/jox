#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <set>
#include <string>

#include <libguile.h>

#include "yaml-cpp/yaml.h"

#include "job.hh"
#include "job_container.hh"
#include "scheme.h"

struct job_config {
    std::string name;

    bool scheme;
    std::string exec;
    SCM scheme_job;

    std::set<std::string> before;
    std::set<std::string> after;
};

bool operator<(const job_config& lhs, const std::string& rhs) { return lhs.name < rhs; }
bool operator<(const std::string& lhs, const job_config& rhs) { return lhs < rhs.name; }
bool operator<(const job_config& lhs, const job_config& rhs) { return lhs.name < rhs.name; }

std::set<job_config, std::less<>> load_yaml(const std::string& filename)
{
    YAML::Node config = YAML::LoadFile(filename);

    std::set<job_config, std::less<>> job_configs;

    if (config["jobs"])
    {
        for (const auto& job : config["jobs"])
        {
            job_config job_config;
            job_config.name = job["name"].as<std::string>();
            job_config.scheme = false;
            job_config.exec = job["exec"].as<std::string>();

            auto load_sequence = [](const YAML::Node& node) -> std::vector<std::string>
            {
                if (node && node.IsSequence())
                {
                    return node.as<std::vector<std::string>>();
                }

                return {};
            };
            std::vector<std::string> vector_before = load_sequence(job["before"]);
            std::vector<std::string> vector_after = load_sequence(job["after"]);
            job_config.before = std::set<std::string>(vector_before.begin(), vector_before.end());
            job_config.after = std::set<std::string>(vector_after.begin(), vector_after.end());

            job_configs.insert(job_config); // TODO: use return value to check if it already existed
        }
    }

    return job_configs;
}

void build_jobs(jox::job_container& job_container, const std::set<job_config, std::less<>>& job_configs)
{
    // instantiate objects
    for (const auto& job_config : job_configs)
    {
        if (job_config.scheme)
        {
            job_container.add_scheme(job_config.name, job_config.scheme_job);
        }
        else
        {
            job_container.add(job_config.name, job_config.exec);
        }
    }

    // set dependencies correctly
    for (const auto& job_config : job_configs)
    {
        jox::job* job = job_container.get_job(job_config.name);

        for (const auto& job_after : job_config.after)
        {
            jox::job* job_waiting_on = job_container.get_job(job_after);
            job->add_waiting_on(job_waiting_on);
        }
        for (const auto& job_before : job_config.before)
        {
            jox::job* job_waiter = job_container.get_job(job_before);
            job->add_waiter(job_waiter);
        }
    }

    // find startable jobs
    job_container.find_startable();
}

int main_loop(jox::job_container& job_container)
{
    pid_t pgid = getpgrp();

    while (!job_container.all_jobs_completed())
    {
        while (job_container.has_startable())
        {
            job_container.start_next();
        }

        siginfo_t infop;
        waitid(P_PGID, pgid, &infop, WEXITED);  // TODO: check return code

        job_container.job_completed(infop.si_pid);
    }

    return 0;
}

void scheme_add_job_config(std::set<job_config, std::less<>>& job_configs, const std::string& job_name)
{
    if (job_configs.contains(job_name))
    {
        return;
    }

    SCM scheme_job = find_job(job_name);

    job_config job_config;
    job_config.scheme_job = scheme_job;
    job_config.name = job_read_string("job-name-proxy", scheme_job);
    job_config.scheme = true;
    job_config.after = job_read_list_strings("job-after-proxy", scheme_job);
    job_config.before = job_read_list_strings("job-before-proxy", scheme_job);

    job_configs.insert(job_config);

    for (const auto& job_after : job_config.after)
    {
         scheme_add_job_config(job_configs, job_after);
    }

    for (const auto& job_before : job_config.before)
    {
         scheme_add_job_config(job_configs, job_before);
    }
}

void main_scheme(void* data, int argc, char** argv)
{
    std::string filename = "jobs.yaml";
    if (argc > 1)
    {
        filename = argv[1];
    }

    if (!filename.ends_with(".scm"))
    {
        // TODO: we should never end up here...
    }

    scm_c_primitive_load("src/jox.scm");

    scm_c_primitive_load(filename.c_str());

    SCM scheme_targets_var = scm_c_lookup("targets");
    SCM scheme_targets = scm_variable_ref(scheme_targets_var);

    std::set<job_config, std::less<>> job_configs;

    SCM scheme_targets_cdr = scheme_targets;
    while (!scm_is_null(scheme_targets_cdr))
    {
        job_config job_config;

        SCM scheme_targets_car = scm_car(scheme_targets_cdr);

        scm_dynwind_begin(static_cast<scm_t_dynwind_flags>(0));

        char* job_name = job_read_string("job-name-proxy", scheme_targets_car);
        scheme_add_job_config(job_configs, job_name);

        scm_dynwind_end();

        scheme_targets_cdr = scm_cdr(scheme_targets_cdr);
    }

    jox::job_container job_container;
    build_jobs(job_container, job_configs);

    main_loop(job_container);

    //int targets_length = scm_length(scheme_targets);
    //for (int k = 0; k < targets_length; ++k)
    //{
    //    SCM target_k = scm_list_ref(targets_length, scm_from_int(k));

    //    // TODO: build job from target_k
    //}

    // TODO: main loop...
}

int main(int argc, char** argv)
{
    std::string filename = "jobs.yaml";
    if (argc > 1)
    {
        filename = argv[1];
    }

    if (filename.ends_with(".scm"))
    {
        scm_boot_guile(argc, argv, main_scheme, NULL);

        // Note: scm_boot_guile() never returns
    }

    auto job_configs = load_yaml(filename);

    jox::job_container job_container;
    build_jobs(job_container, job_configs);

    return main_loop(job_container);
}

