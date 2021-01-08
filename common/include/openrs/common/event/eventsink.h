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

#include "openrs/common/event/event.h"

namespace openrs {
namespace common {
namespace event {

template <typename T>
class EventSink {
 public:
  /**
   * Handle an event.
   *
   * @param kEvent The event details.
   * @return True if event processing should continue to other event handlers,
   *  or false to stop handling this event.
   */
  virtual bool HandleEvent(const T& kEvent) = 0;
};

}  // namespace event
}  // namespace common
}  // namespace openrs
