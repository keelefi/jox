#include "job_container.hh"

namespace jox {

job_container::job_container() :
        m_jobs(),
        m_jobs_running(),
        m_startable_jobs(),
        m_jobs_completed(0)
{
}

void job_container::job_completed(const pid_t child_pid)
{
    auto completed_job = m_jobs_running.find(child_pid);
    if (completed_job == m_jobs_running.end())
    {
        // error
    }

    completed_job->second->completed();

    m_jobs_running.erase(completed_job);

    m_jobs_completed++;
}

void job_container::job_startable(job* startable_job)
{
    m_startable_jobs.push_back(startable_job);
}

void job_container::add(const std::string& name, const std::string& exec)
{
    m_jobs.emplace(std::string(name), job(name, exec, *this));
}

bool job_container::remove(const std::string&)
{
    // TODO
    return false;
}

job* job_container::get_job(const std::string& job_name)
{
    auto job = m_jobs.find(job_name);
    if (job != m_jobs.end())
    {
        return &job->second;
    }
    return NULL; // TODO: instead, throw exception
}

void job_container::find_startable()
{
    for (auto& iter : m_jobs)
    {
        auto job = &iter.second;
        if (job->is_startable())
        {
            m_startable_jobs.push_back(job);
        }
    }
}

bool job_container::has_startable() const
{
    return !m_startable_jobs.empty();
}

void job_container::start_next()
{
    auto new_job = m_startable_jobs.front();
    m_startable_jobs.pop_front();

    m_jobs_running.emplace(new_job->start());
}

unsigned int job_container::jobs_running() const
{
    return m_jobs_running.size();
}

unsigned int job_container::jobs_started() const
{
    return m_jobs_completed + jobs_running();
}

unsigned int job_container::jobs_pending() const
{
    return m_jobs.size() - jobs_started();
}

bool job_container::all_jobs_completed() const
{
    return m_jobs.size() == m_jobs_completed;
}

}

