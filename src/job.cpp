#include "job.hh"

#include "job_container.hh"

#include <sys/types.h>
#include <unistd.h>

namespace jox {

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
        m_scheme(false),
        m_job_container(job_container)
{
}

job::job(
    const std::string& name,
    SCM scheme_job,
    job_container& job_container
    ) :
        m_name(name),
        m_exec(),
        m_scheme_job(scheme_job),
        m_waiting_on(),
        m_waiters(),
        m_running(false),
        m_completed(false),
        m_scheme(true),
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
        if (m_scheme)
        {
            SCM job_script_proxy_func_var = scm_c_lookup("job-script-proxy");
            SCM job_script_proxy_func = scm_variable_ref(job_script_proxy_func_var);
            SCM job_script = scm_call_1(job_script_proxy_func, m_scheme_job);

            // Note: The result can be retrieved later by calling promise again. Calling the promise again will not
            // have guile execute it again, but retrieve the last result.
            SCM result = scm_force(job_script);

            SCM set_job_result_proxy_func_var = scm_c_lookup("set-job-result-proxy!");
            SCM set_job_result_proxy_func = scm_variable_ref(set_job_result_proxy_func_var);
            scm_call_2(set_job_result_proxy_func, m_scheme_job, result);

            if (scm_is_integer(result))
            {
                exit(scm_to_int(result));
            }

            // TODO: create error

            exit(EXIT_FAILURE);
        }
        else
        {
            // Note: exec() returns only on error
            execl("/bin/sh", "sh", "-c", m_exec.c_str(), NULL);
            // TODO: check error
        }
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

