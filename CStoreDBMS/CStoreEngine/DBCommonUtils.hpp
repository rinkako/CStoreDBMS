#ifndef ___CSTORE_DBCOMMONUTILS
#define ___CSTORE_DBCOMMONUTILS
#include <string>
#include <vector>
#include "DBObject.h"
#include "DBMarcos.h"

namespace CSCommonUtil {
  //函数作用： 消去字符串前后的空白
  //参数列表：
  //       str 要处理的字符串
  //返 回 值： N/A
  template<typename _Ty = std::string>
  std::string CStrTrim(const std::string& str) {
    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos) {
      return str;
    }
    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != std::string::npos) {
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
  std::string CStrReplace(const std::string& str, const std::string& src, const std::string& dest) {
    std::string ret;
    std::string::size_type pos_begin = 0;
    std::string::size_type pos = str.find(src);
    while (pos != std::string::npos) {
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
  std::vector<std::string> CStrSplit(const std::string& str, std::string sep = ",") {
    std::vector<std::string> retVec;
    if (str.empty()) {
      return retVec;
    }
    std::string tmp;
    std::string::size_type pos_begin = str.find_first_not_of(sep);
    std::string::size_type comma_pos = 0;
    while (pos_begin != std::string::npos) {
      comma_pos = str.find(sep, pos_begin);
      if (comma_pos != std::string::npos) {
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

  class StringBuilder : CStore::DBObject {
  public:
    //函数作用： 构造函数
    //参数列表： N/A
    //返 回 值： N/A
    StringBuilder() : CStore::DBObject("StringBuilder", this) {
      this->_str = "";
    }

    //函数作用： 带初始字符串的构造函数
    //参数列表：
    //       str 初始化字符串
    //返 回 值： N/A
    StringBuilder(const std::string& str) : CStore::DBObject("StringBuilder", this) {
      this->_str = str;
    }

    //函数作用： 带初始字符串的构造函数
    //参数列表：
    //       str 初始化字符串
    //返 回 值： N/A
    StringBuilder(const char* str) : CStore::DBObject("StringBuilder", this) {
      this->_str = std::string(str);
    }

    //函数作用： 获取当前字符串长度
    //参数列表： N/A
    //返 回 值： 字符串长度整数
    inline int Length() {
      return this->_str.length();
    }

    //函数作用： 清空字符串构造器
    //参数列表： N/A
    //返 回 值： N/A
    inline void Clear() {
      this->_str.clear();
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(const std::string& appender) {
      this->_str.append(appender);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(char appender) {
      this->_str.push_back(appender);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(const char* appender) {
      this->_str.append(appender);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(int appender) {
      char numbuf[32];
      std::sprintf(numbuf, "%d", appender);
      this->_str.append(numbuf);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(double appender) {
      char numbuf[64];
      std::sprintf(numbuf, "%lf", appender);
      this->_str.append(numbuf);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append(float appender) {
      char numbuf[64];
      std::sprintf(numbuf, "%f", appender);
      this->_str.append(numbuf);
      return *this;
    }

    //函数作用： 追加内容
    //参数列表：
    //  appender 要追加的东西
    //返 回 值： N/A
    StringBuilder& Append (CStore::DBObject& appender) {
      this->_str.append(appender.ToString().c_str());
      return *this;
    }

    //函数作用： 重写字符串化方法
    //参数列表： N/A
    //返 回 值： 该字符串构造器对应的字符串实例
    virtual std::string ToString() {
      return this->_str;
    }

    //函数作用： 重写对比两个对象是否相等，直接比较对应的字符串是否相等
    //参数列表：
    //    RefObj 要比较的对象引用
    //返 回 值： 是否相等
    virtual bool Equals(StringBuilder& RefObj) {
      return this->_str == RefObj.ToString();
    }

  private:
    // 字符串容器
    std::string _str;

    // 阻止拷贝构造
    DISALLOW_COPY_AND_ASSIGN(StringBuilder);
  }; /* StringBuilder */

} /* CStoreUtil */

#endif /* ___CSTORE_DBCOMMONUTILS */