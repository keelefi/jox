#pragma once

#include "job.hh"

#include <deque>
#include <unordered_map>
#include <string>

namespace job_executor {

class job_container {
public:
    job_container();
    job_container(const job_container&) = delete;
    job_container& operator=(const job_container&) = delete;

    void job_completed(const pid_t);
    void job_startable(job*);

    void add(const std::string&, const std::string&);
    bool remove(const std::string&);

    job* get_job(const std::string&);

    void find_startable();
    bool has_startable() const;
    void start_next();

    unsigned int jobs_running() const;
    unsigned int jobs_started() const;
    unsigned int jobs_pending() const;

    bool all_jobs_completed() const;

private:
    std::unordered_map<std::string, job> m_jobs;
    std::unordered_map<pid_t, job*> m_jobs_running;
    std::deque<job*> m_startable_jobs;

    unsigned int m_jobs_completed;
};

}

