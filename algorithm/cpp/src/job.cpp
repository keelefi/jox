#include "job.h"

namespace algorithm
{

job::job(const std::string& name, const std::vector<std::string>& after, const std::vector<std::string>& before) :
        m_name(name),
        m_after(after),
        m_before(before)
{
}

bool job::operator==(const job& other) const
{
    return (m_name == other.m_name) &&
            (m_after == other.m_after) &&
            (m_before == other.m_before);
}

const std::string job::get_name() const
{
    return m_name;
}

void job::add_after(const std::string& job_after)
{
    m_after.push_back(job_after);
}

void job::add_before(const std::string& job_before)
{
    m_before.push_back(job_before);
}

const std::vector<std::string>& job::get_after() const
{
    return m_after;
}

const std::vector<std::string>& job::get_before() const
{
    return m_before;
}

} // namespace algorithm

