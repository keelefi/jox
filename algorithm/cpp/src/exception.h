#pragma once

#include <stdexcept>
#include <string>

namespace algorithm
{

class algorithm_exception: public std::logic_error
{
public:
    algorithm_exception(const std::string&) noexcept;
};

class cyclic_dependency_exception: public algorithm_exception
{
public:
    cyclic_dependency_exception() noexcept;
};

class job_depends_on_itself_exception: public algorithm_exception
{
public:
    job_depends_on_itself_exception(const std::string&) noexcept;
};

class no_targets_exception: public algorithm_exception
{
public:
     no_targets_exception() noexcept;
};

class job_not_found_exception: public algorithm_exception
{
public:
    job_not_found_exception(const std::string&, const std::string&) noexcept;
};

} // namespace algorithm

