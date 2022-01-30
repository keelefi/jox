#pragma once

#include <string>

namespace algorithm
{

enum WARNING_TYPE
{
    JOB_NOT_REQUIRED,
};

WARNING_TYPE warning_type_decode(const std::string&);

class warning
{
public:
    warning(const WARNING_TYPE, const std::string&);

    bool operator==(const warning&) const;

    std::string get_warning() const;
    std::string get_message() const;
private:
    WARNING_TYPE m_warning;
    std::string m_message;
};

} // namespace algorithm

