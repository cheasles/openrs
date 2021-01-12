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
#include <openrs/common/event/event.h>
#include <openrs/common/event/eventsink.h>
#include <openrs/common/event/eventsource.h>

#include <memory>

#include "gtest/gtest.h"

/**
 * A simple test event with an int parameter.
 */
class EventTest : public openrs::common::event::Event<int> {
 public:
  EventTest(int test) : openrs::common::event::Event<int>(test) {}
};

/**
 * Handles the simple event and sets a member variable to its' value.
 */
class EventTestSink : public openrs::common::event::EventSink<EventTest> {
 public:
  /**
   * The value of the int in the last handled event.
   */
  int last_event_value;

  /**
   * Handles a test event.
   *
   * @param kEvent The event details.
   * @return True if the event was processed by this sink and shouldn't be
   *  passed to the next registered sink.
   */
  inline bool HandleEvent(const EventTest& kEvent) override {
    this->last_event_value = std::get<0>(kEvent);
    return true;
  }
};

/**
 * Emits a test event.
 */
class EventTestSource : public openrs::common::event::EventSource<EventTest> {
 public:
  /**
   * Trigger a test event.
   *
   * @param kTest The int value to trigger with.
   */
  inline void TriggerTest(const int& kTest) { this->EmitEvent({kTest}); }
};

/**
 * Test that emitting an event works correctly.
 */
TEST(Event, EmitEvent) {
  EventTestSource source;
  auto sink = std::make_shared<EventTestSink>();

  ASSERT_NO_THROW(source.add_sink(sink));
  ASSERT_NO_THROW(source.TriggerTest(5));
  EXPECT_EQ(5, sink->last_event_value);
}

/**
 * Test that deleting a sink before triggering an event doesn't crash.
 */
TEST(Event, EmitEvent_SinkExpired) {
  EventTestSource source;
  {
    auto sink = std::make_shared<EventTestSink>();
    ASSERT_NO_THROW(source.add_sink(sink));
  }

  ASSERT_NO_THROW(source.TriggerTest(5));
}
