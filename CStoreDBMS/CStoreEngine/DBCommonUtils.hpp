#ifndef ___CSTORE_DBCOMMONUTILS
#define ___CSTORE_DBCOMMONUTILS
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace CSCommonUtil {
  //函数作用： 消去字符串前后的空白
  //参数列表：
  //       str 要处理的字符串
  //返 回 值： N/A
  template<typename _Ty = std::string>
  string CStrTrim(const string& str) {
    string::size_type pos = str.find_first_not_of(' ');
    if (pos == string::npos) {
      return str;
    }
    string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != string::npos) {
      return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
  }

  //函数作用： 替换字符串中的内容
  //参数列表：
  //       str 要处理的字符串
  //       src 要替换的原的子串
  //      dest 要替换成的目标子串
  //返 回 值： 分割出的子串数量
  template<typename _Ty = std::string>
  string CStrReplace(const string& str, const string& src, const string& dest) {
    string ret;
    string::size_type pos_begin = 0;
    string::size_type pos = str.find(src);
    while (pos != string::npos) {
      ret.append(str.data() + pos_begin, pos - pos_begin);
      ret += dest;
      pos_begin = pos + 1;
      pos = str.find(src, pos_begin);
    }
    if (pos_begin < str.length()) {
      ret.append(str.begin() + pos_begin, str.end());
    }
    return ret;
  }

  //函数作用： 将字符串分割成子串向量
  //参数列表：
  //       str 要处理的字符串
  //       sep 分隔符
  //返 回 值： 子串向量
  template<typename _Ty = std::string>
  vector<string> CStrSplit(const string& str, string sep = ",") {
    vector<string> retVec;
    if (str.empty()) {
      return retVec;
    }
    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;
    while (pos_begin != string::npos) {
      comma_pos = str.find(sep, pos_begin);
      if (comma_pos != string::npos) {
        tmp = str.substr(pos_begin, comma_pos - pos_begin);
        pos_begin = comma_pos + sep.length();
      } else {
        tmp = str.substr(pos_begin);
        pos_begin = comma_pos;
      }
      if (!tmp.empty()) {
        retVec.push_back(tmp);
        tmp.clear();
      }
    }
    return retVec;
  }

} /* namespace: CStoreUtil */

#endif /* ___CSTORE_DBCOMMONUTILS */