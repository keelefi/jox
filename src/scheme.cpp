#include "scheme.h"

SCM find_job(const std::string& job_name)
{
    SCM scheme_job_var = scm_c_lookup(job_name.c_str());
    SCM scheme_job = scm_variable_ref(scheme_job_var);

    return scheme_job;
}

char* job_read_string(const std::string& function, SCM& job)
{
    SCM job_parameter_func_var = scm_c_lookup(function.c_str());
    SCM job_parameter_func = scm_variable_ref(job_parameter_func_var);
    SCM job_parameter = scm_call_1(job_parameter_func, job);

    char* result = scm_to_locale_string(job_parameter);
    scm_dynwind_free(result);

    return result;
}

SCM job_read_scm(const std::string& function, SCM& job)
{
    SCM job_parameter_func_var = scm_c_lookup(function.c_str());
    SCM job_parameter_func = scm_variable_ref(job_parameter_func_var);
    SCM job_parameter = scm_call_1(job_parameter_func, job);

    return job_parameter;
}

std::set<std::string> job_read_list_strings(const std::string& function, SCM& job)
{
    SCM job_parameter_func_var = scm_c_lookup(function.c_str());
    SCM job_parameter_func = scm_variable_ref(job_parameter_func_var);
    SCM job_parameter = scm_call_1(job_parameter_func, job);

    std::set<std::string> result;

    SCM list_cdr = job_parameter;
    while (!scm_is_null(list_cdr))
    {
        SCM list_car = scm_car(list_cdr);

        scm_dynwind_begin(static_cast<scm_t_dynwind_flags>(0));

        char* current_string = scm_to_locale_string(list_car);
        scm_dynwind_free(current_string);

        result.insert(current_string);

        scm_dynwind_end();

        list_cdr = scm_cdr(list_cdr);
    }

    return result;
}

