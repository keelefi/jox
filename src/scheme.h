#pragma once

#include <set>
#include <string>

#include <libguile.h>

SCM find_job(const std::string& job_name);

char* job_read_string(const std::string& function, SCM& job);

SCM job_read_scm(const std::string& function, SCM& job);

std::set<std::string> job_read_list_strings(const std::string& function, SCM& job);
