#pragma once

#include <cstddef>

namespace scann {

class IWriter {
  public:
    virtual void write(const char * from, size_t n) = 0;
    virtual ~IWriter() = default;
};

class IReader {
  public:
    virtual void read(char * to, size_t n) = 0; 
    virtual ~IReader() = default;
};

}  // namespace scann
