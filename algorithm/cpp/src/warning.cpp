#include "warning.h"

#include <stdexcept>

namespace algorithm
{

WARNING_TYPE warning_type_decode(const std::string& warning_type)
{
    if (warning_type == "JOB NOT REQUIRED")
    {
        return JOB_NOT_REQUIRED;
    }

    throw std::runtime_error("Invalid warning type string " + warning_type);
}

warning::warning(const WARNING_TYPE warning, const std::string& message) :
        m_warning(warning),
        m_message(message)
{
}

bool warning::operator==(const warning& other) const
{
    return (m_warning == other.m_warning) &&
            (m_message == other.m_message);
}

std::string warning::get_warning() const
{
    switch (m_warning)
    {
    case JOB_NOT_REQUIRED:
        return "JOB NOT REQUIRED";
    }

    throw std::runtime_error("Invalid warning type enumeration");
}

std::string warning::get_message() const
{
    return m_message;
}

} // namespace algorithm

