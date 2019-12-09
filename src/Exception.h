#ifndef WIPHYNLCONTROL_EXCEPTION_H_
#define WIPHYNLCONTROL_EXCEPTION_H_

#include <cerrno>
#include <cstring>
#include <exception>
#include <string>

namespace wiphynlcontrol {

std::string append_errno_to_str(const std::string &info);

class Exception : public std::exception {
 private:
  const std::string info_;

 public:
  explicit Exception(const std::string &info = "");

 public:
  const char *what() const noexcept;
  static const std::string err_to_str(const int &code);

};  // Exception

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_EXCEPTION_H_