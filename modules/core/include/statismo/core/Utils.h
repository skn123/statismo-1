/*
 * This file is part of the statismo library.
 *
 * Author: Marcel Luethi (marcel.luethi@unibas.ch)
 *
 * Copyright (c) 2011 University of Basel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __STATISMO_UTILS_H_
#define __STATISMO_UTILS_H_

#include "statismo/core/CommonTypes.h"
#include "statismo/core/Exceptions.h"
#include "statismo/core/RandUtils.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <random>

namespace statismo
{

/**
 * \brief A number of small utility functions - internal use only.
 */

namespace details
{
template <typename T>
inline T
LexicalCast(const std::string &)
{
  throw std::bad_cast();
}

template <>
inline double
LexicalCast<double>(const std::string & str)
{
  return std::stod(str);
}

template <>
inline float
LexicalCast<float>(const std::string & str)
{
  return std::stof(str);
}

template <>
inline int
LexicalCast<int>(const std::string & str)
{
  return std::stoi(str);
}

template <>
inline long
LexicalCast<long>(const std::string & str)
{
  return std::stol(str);
}

template <>
inline long long
LexicalCast<long long>(const std::string & str)
{
  return std::stoll(str);
}

template <>
inline unsigned int
LexicalCast<unsigned int>(const std::string & str)
{
  return std::stoul(str);
}

template <>
inline unsigned long
LexicalCast<unsigned long>(const std::string & str)
{
  return std::stoul(str);
}

template <>
inline unsigned long long
LexicalCast<unsigned long long>(const std::string & str)
{
  return std::stoull(str);
}
} // namespace details

namespace utils
{
inline VectorType
GenerateNormalVector(unsigned n)
{
  // we make the random generate static, to ensure that the seed is only set once, and not with
  // every call
  static std::normal_distribution<> s_dist(0, 1);
  static auto                       s_r = std::bind(s_dist, rand::RandGen());

  VectorType v = VectorType::Zero(n);
  for (unsigned i = 0; i < n; i++)
  {
    v(i) = s_r();
  }
  return v;
}

inline VectorType
ReadVectorFromTxtFile(const char * name)
{
  using ListType = std::list<statismo::ScalarType>;
  ListType      values;
  std::ifstream inFile(name, std::ios::in);
  if (inFile.good())
  {
    std::copy(std::istream_iterator<statismo::ScalarType>(inFile),
              std::istream_iterator<statismo::ScalarType>(),
              std::back_insert_iterator<ListType>(values));
  }
  else
  {
    throw StatisticalModelException((std::string("Could not read text file ") + name).c_str(), Status::BAD_INPUT_ERROR);
  }

  VectorType v = VectorType::Zero(values.size());
  unsigned   i = 0;
  for (auto it = std::cbegin(values); it != std::cend(values); ++it)
  {
    v(i) = *it;
    i++;
  }
  return v;
}

inline std::string
CreateTmpName(const std::string & extension)
{
  // https://github.com/statismo/statismo/pull/268/files
  // \note replace by std::filesystem in the future

  // imitates the path that was generated by boost::filesystem::unique_path to make sure we don't break anything
  static const char                      sk_pathChars[] = "0123456789abcdefghiklmnopqrstuvwxyz";
  static std::uniform_int_distribution<> s_randIndex(
    0, sizeof(sk_pathChars) - 2); //-1 for the \0 terminator and -1 because the index starts at 0

  std::string mask = "%%%%-%%%%-%%%%-%%%%";
  for (auto & c : mask)
  {
    if (c == '%')
    {
      c = sk_pathChars[s_randIndex(rand::RandGen())];
    }
  }

  return mask + extension;
}

inline void
RemoveFile(const std::string & str)
{
  std::remove(str.c_str());
}

inline void
ToLower(std::string & str)
{
  std::transform(std::begin(str), std::end(str), std::begin(str), [](unsigned char c) { return std::tolower(c); });
}

inline std::string
ToLowerCopy(std::string str)
{
  ToLower(str);
  return str;
}

template <typename T>
static T
LexicalCast(const std::string & str)
{
  try
  {
    return details::LexicalCast<T>(str);
  }
  catch (...)
  {
    throw std::bad_cast();
  }
}

template <char D>
static auto
Split(const std::string & in)
{
  std::istringstream       iss(in);
  std::vector<std::string> vec{ std::istream_iterator<WordDelimiter<D>>{ iss },
                                std::istream_iterator<WordDelimiter<D>>{} };
  return vec;
}
} // namespace utils

} // namespace statismo

namespace std
{
inline string
to_string(const statismo::MatrixType & t) // NOLINT
{
  ostringstream os;
  os << t;
  return os.str();
}
} // namespace std

#endif
