#ifndef WIPHYNLCONTROL_EXCEPTION_H_
#define WIPHYNLCONTROL_EXCEPTION_H_

#include <exception>

namespace wiphynlcontrol {

class Exception : public std::exception {
 private:
  const char* info_;

 public:
  explicit Exception(const char* info = "");

 public:
  const char* what() const noexcept { return this->info_; }

};  // Exception

}  // namespace wiphynlcontrol

#endif  // defined WIPHYNLCONTROL_EXCEPTION_H_