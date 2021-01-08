/**
 * This file is part of OpenRS.
 *
 * OpenRS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenRS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with OpenRS. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <iostream>
#include <string>

namespace openrs {

namespace common {

class Log {
 public:
  enum class LogLevel { kDebug, kInfo, kWarning, kError };

 private:
  LogLevel level_;
  static LogLevel minimum_level_;

 public:
  Log() : level_(LogLevel::kDebug) {}

  Log(const LogLevel& level) : level_(level) {
    operator<<("[" + Log::GetLabel(level) + "] ");
  }

  ~Log() {
    if (this->level_ >= Log::minimum_level_) {
      if (this->level_ == LogLevel::kError)
        std::cerr << std::endl;
      else
        std::cout << std::endl;
    }
  }

  template <typename Type>
  Log& operator<<(const Type& message) {
    if (this->level_ >= Log::minimum_level_) {
      if (this->level_ == LogLevel::kError)
        std::cerr << message;
      else
        std::cout << message;
    }

    return *this;
  }

  static inline void set_level(const LogLevel& level) {
    Log::minimum_level_ = level;
  }

  static inline std::string GetLabel(const LogLevel& level) {
    switch (level) {
      case LogLevel::kDebug:
        return "DEBUG";
      case LogLevel::kInfo:
        return "INFO ";
      case LogLevel::kWarning:
        return "WARN ";
      case LogLevel::kError:
        return "ERROR";
    }

    return "";
  }
};

}  // namespace common

}  // namespace openrs
