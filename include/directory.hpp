#include <string>

class Directory {
  private:
    std::string _data;
    std::string _path;
  public:
  Directory();
  Directory(const std::string& path);
  ~Directory();
  std::string& getHtml();
};
