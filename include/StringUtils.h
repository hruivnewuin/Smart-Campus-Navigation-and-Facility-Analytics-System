#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include <string>
#include <vector>

namespace StringUtils {
//remove leading/trailing whitespace
inline std::string trim(const std::string& s) {
  const char* ws = " \t\r\n\f\v";
  std::string::size_type b = s.find_first_not_of(ws);
  if (b == std::string::npos) return "";
  std::string::size_type e = s.find_last_not_of(ws);
  return s.substr(b, e - b + 1);
}
//convert ascii letters to lowercase
inline std::string toLowerAscii(const std::string& s) {
  std::string r = s;
  for (std::string::size_type i = 0; i < r.size(); ++i)
    if (r[i] >= 'A' && r[i] <= 'Z') r[i] = static_cast<char>(r[i] - 'A' + 'a');
  return r;
}
//case sensitive equality of two already trimmed strings
inline bool equalsIgnoreCase(const std::string& a, const std::string& b) {
  return toLowerAscii(a) == toLowerAscii(b);
}
//split by '|' and return vectors using commas
inline std::vector<std::string> splitFields(const std::string& line) {
  std::vector<std::string> out;
  std::string cur;
  for (std::string::size_type i = 0; i < line.size(); ++i) {
    if (line[i] == '|') { out.push_back(trim(cur)); cur.clear(); }
    else cur += line[i];
  }
  out.push_back(trim(cur));
  return out;
}
//true if the line is blank or a comment
inline bool isBlankOrComment(const std::string& line) {
  std::string t = trim(line);
  return t.empty() || t[0] == '#';
}
}

#endif
