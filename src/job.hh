#pragma once

#include <set>
#include <string>

namespace jox {

class job_container;

class job {
public:
    job(const std::string&,
        const std::string&,
        job_container&);

    std::pair<pid_t, job*> start();
    void completed();

    void dependency_completed(job*);

    const std::string& get_name() const { return m_name; };
    const std::string& get_exec() const { return m_exec; };

    void add_waiter(job*);
    void add_waiting_on(job*);

    bool is_startable() const;
    bool is_running() const;
    bool has_completed() const;

private:
    std::string m_name;
    std::string m_exec;

    std::set<job*> m_waiting_on;
    std::set<job*> m_waiters;

    bool m_running;
    bool m_completed;

    job_container& m_job_container;
};

}

