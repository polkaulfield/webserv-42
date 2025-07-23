#ifndef UPLOADEDFILE_HPP
#define UPLOADEDFILE_HPP

#include <string>
#include <vector>
#include <iostream>

class ServerResponse;
class ClientRequest;

struct UploadedFile {
  std::string _filename;
  std::string _fieldName;
  std::string _contentType;
  std::vector<char> _content;
  size_t _size;

  UploadedFile() : _size(0) {}
};

#endif
