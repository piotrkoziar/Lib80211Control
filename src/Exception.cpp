#include "Exception.h"

namespace wiphynlcontrol {

Exception::Exception(const std::string &info) : info_(info) {}

const char *Exception::what() const noexcept { return info_.c_str(); }

const std::string Exception::err_to_str(const int &code) {
  std::string str_code = std::to_string(code);
  std::string msg = ": ";
  switch (code) {
    case -1:
      msg.append("Operation not permitted");
      break;
    case -22:
      msg.append("Invalid argument");
      break;
    case -19:
      msg.append("No such device");
      break;
    default:
      msg.append("Unspecified failure");
      break;
  }
  std::string ret = str_code + msg;
  return ret;
}

}  // namespace wiphynlcontrol