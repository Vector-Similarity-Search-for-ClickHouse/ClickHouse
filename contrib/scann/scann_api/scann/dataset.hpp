#pragma once

#include <array>
#include <vector>

// row_major_arr

template <typename T, size_t N>
class ConstDataSetWrapper {
 public:
  ConstDataSetWrapper(const std::vector<T>& data, std::array<int, N> shape)
      : data_(data), shape_(shape) {
    // check data_.size() == shape.prod() ? ;
  }

  const std::array<int, N>& Shape() const { return shape_; }

  const T* Data() const { return data_.data(); }

  size_t Size() const { return data_.size(); }

 private:
  const std::vector<T>& data_;
  const std::array<int, N> shape_;
};
