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

#include <memory>
#include <vector>

#include "openrs/common/event/eventsink.h"

namespace openrs {
namespace common {
namespace event {

template <typename T>
class EventSource {
 private:
  std::vector<std::weak_ptr<EventSink<T>>> sinks_;

 protected:
  void EmitEvent(const T& kEvent) const {
    for (const auto& kSink : this->sinks_) {
      if (auto sink_instance = kSink.lock()) {
        if (sink_instance->HandleEvent(kEvent)) {
          break;
        }
      }
    }
  };

 public:
  inline void add_sink(const std::weak_ptr<EventSink<T>>& kSink) {
    this->sinks_.emplace_back(kSink);
  }

  inline const auto& sinks() const { return this->sinks_; }
};

}  // namespace event
}  // namespace common
}  // namespace openrs
