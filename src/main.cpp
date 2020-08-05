#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <set>
#include <string>

#include "yaml-cpp/yaml.h"

#include "job.hh"
#include "job_container.hh"

struct job_config {
    bool operator<(const job_config& rhs) const { return name < rhs.name; }
    std::string name;
    std::string exec;
    std::set<std::string> before;
    std::set<std::string> after;
};

std::set<job_config> load_yaml(const std::string& filename)
{
    YAML::Node config = YAML::LoadFile(filename);

    std::set<job_config> job_configs;

    if (config["jobs"])
    {
        for (const auto& job : config["jobs"])
        {
            job_config job_config;
            job_config.name = job["name"].as<std::string>();
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

void build_jobs(job_executor::job_container& job_container, const std::set<job_config>& job_configs)
{
    // instantiate objects
    for (const auto& job_config : job_configs)
    {
        job_container.add(job_config.name, job_config.exec);
    }

    // set dependencies correctly
    for (const auto& job_config : job_configs)
    {
        job_executor::job* job = job_container.get_job(job_config.name);

        for (const auto& job_after : job_config.after)
        {
            job_executor::job* job_waiting_on = job_container.get_job(job_after);
            job->add_waiting_on(job_waiting_on);
        }
        for (const auto& job_before : job_config.before)
        {
            job_executor::job* job_waiter = job_container.get_job(job_before);
            job->add_waiter(job_waiter);
        }
    }

    // find startable jobs
    job_container.find_startable();
}

int main_loop(job_executor::job_container& job_container)
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

int main(int argc, char** argv)
{
    std::string filename = "jobs.yaml";
    if (argc > 1)
    {
        filename = argv[1];
    }
    auto job_configs = load_yaml(filename);

    job_executor::job_container job_container;
    build_jobs(job_container, job_configs);

    return main_loop(job_container);
}

