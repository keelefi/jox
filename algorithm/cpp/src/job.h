#pragma once

#include <string>
#include <vector>

namespace algorithm
{

class job
{
public:
    job(const std::string&, const std::vector<std::string>&, const std::vector<std::string>&);

    // TODO: default comparison is available in C++20
    //bool operator==(const job&) const = default;
    bool operator==(const job&) const;

    const std::string get_name() const;

    void add_after(const std::string&);
    void add_before(const std::string&);

    const std::vector<std::string>& get_after() const;
    const std::vector<std::string>& get_before() const;

private:
    std::string m_name;
    std::vector<std::string> m_after;
    std::vector<std::string> m_before;
};

} // namespace algorithm

