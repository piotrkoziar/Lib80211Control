#ifndef WIPHYNLCONTROL_EXCEPTION_H_
#define WIPHYNLCONTROL_EXCEPTION_H_

#include <exception>
#include <string>
#include <cerrno>
#include <cstring>


namespace wiphynlcontrol {

std::string append_errno_to_str(const std::string &info, const int &errno_code);

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