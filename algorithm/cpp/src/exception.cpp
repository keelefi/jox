#include "exception.h"

namespace algorithm
{

algorithm_exception::algorithm_exception(const std::string& message) noexcept :
        std::logic_error(message)
{
}

cyclic_dependency_exception::cyclic_dependency_exception() noexcept :
        algorithm_exception("Cyclic dependency detected")
{
}

job_depends_on_itself_exception::job_depends_on_itself_exception(const std::string& job_name) noexcept :
        algorithm_exception("Job '" + job_name + "' depends on itself")
{
}

no_targets_exception::no_targets_exception() noexcept :
        algorithm_exception("No targets")
{
}

job_not_found_exception::job_not_found_exception(
            const std::string& job_depender,
            const std::string& job_dependee) noexcept :
        algorithm_exception("Job '" + job_depender +
                            "' references job '" + job_dependee +
                            "', but job '" + job_dependee +
                            "' does not exist")
{
}

} // naemspace algorithm

