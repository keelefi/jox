#include "job.hh"

#include "job_container.hh"

#include <sys/types.h>
#include <unistd.h>

namespace job_executor {

job::job(
    const std::string& name,
    const std::string& exec,
    job_container& job_container
    ) :
        m_name(name),
        m_exec(exec),
        m_waiting_on(),
        m_waiters(),
        m_running(false),
        m_completed(false),
        m_job_container(job_container)
{
}

std::pair<pid_t, job*> job::start()
{
    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        // TODO: throw exception
    }
    if (child_pid == 0)
    {
        // Note: exec() returns only on error
        execl("/bin/sh", "sh", "-c", m_exec.c_str(), NULL);
        // TODO: check error
    }

    m_running = true;

    return std::make_pair(child_pid, this);
}

void job::completed()
{
    m_running = false;
    m_completed = true;

    for (const auto& waiter : m_waiters)
    {
        waiter->dependency_completed(this);
    }
}

void job::dependency_completed(job* dependency)
{
    m_waiting_on.erase(dependency);

    if (m_waiting_on.empty())
    {
        m_job_container.job_startable(this);
    }
}

void job::add_waiter(job* waiter)
{
    auto result = m_waiters.insert(waiter);
    if (result.second)
    {
        waiter->add_waiting_on(this);
    }
}

void job::add_waiting_on(job* waiting_on)
{
    auto result = m_waiting_on.insert(waiting_on);
    if (result.second)
    {
        waiting_on->add_waiter(this);
    }
}

bool job::is_startable() const
{
    return m_waiting_on.empty() && !m_running && !m_completed;
}

bool job::is_running() const
{
    return m_running;
}

bool job::has_completed() const
{
    return m_completed;
}

}

