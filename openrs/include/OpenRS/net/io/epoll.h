#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <map>
#include <memory>
#include <stdexcept>

#include "OpenRS/net/io/channel.h"

namespace openrs {

namespace net {

namespace io {

template <int MAX_EVENTS = 2000>
class EPoll {
 private:
  int file_descriptor_;
  epoll_event epoll_events_[MAX_EVENTS];
  std::map<int, std::shared_ptr<Channel>> channel_map_;

 public:
  EPoll(const int& kMaxEvents) {
    if (kMaxEvents > MAX_EVENTS) {
      throw std::runtime_error("Too many poll events requested.");
    }

    this->file_descriptor_ = ::epoll_create(kMaxEvents);
    if (this->file_descriptor_ <= 0) {
      throw std::runtime_error("Could not create poll event");
    }
  }

  ~EPoll() {
    if (this->file_descriptor_ > 0) {
      ::close(this->file_descriptor_);
    }

    this->channel_map_.clear();
  }

  bool AddPollEvent(const int& kWaitEvent, const int& kSocketFileDescriptor) {
    epoll_event event;
    event.events = kWaitEvent;

    if (0 == ::epoll_ctl(this->file_descriptor_, EPOLL_CTL_ADD,
                         kSocketFileDescriptor, &event)) {
      return true;
    }

    return false;
  }

  bool AddPollEvent(const std::shared_ptr<Channel>& channel,
                    const int& kWaitEvent, const int& kSocketFileDescriptor) {
    this->channel_map_[kSocketFileDescriptor] = channel;

    epoll_event event;
    event.data.ptr = &this->channel_map_[kSocketFileDescriptor];
    event.events = kWaitEvent;

    if (0 == ::epoll_ctl(this->file_descriptor_, EPOLL_CTL_ADD,
                         kSocketFileDescriptor, &event)) {
      return true;
    }

    return false;
  }

  bool UpdatePollEvent(const int& kWaitEvent,
                       const int& kSocketFileDescriptor) {
    auto channel = this->channel_map_.find(kSocketFileDescriptor);
    if (this->channel_map_.cend() == channel) {
      return false;
    }

    epoll_event event;
    event.data.ptr = &this->channel_map_[kSocketFileDescriptor];
    event.events = kWaitEvent;

    if (0 == ::epoll_ctl(this->file_descriptor_, EPOLL_CTL_MOD,
                         kSocketFileDescriptor, &event)) {
      return true;
    }

    return false;
  }

  int Wait(std::vector<std::shared_ptr<Channel>>* channel_events) {
    if (!channel_events) {
      return 0;
    }

    const int kNumberEvents = ::epoll_wait(this->file_descriptor_,
                                           this->epoll_events_, MAX_EVENTS, 0);
    if (kNumberEvents <= 0) {
      return 0;
    }

    channel_events->reserve(kNumberEvents);
    for (int i = 0; i < kNumberEvents; ++i) {
      auto channel = *static_cast<std::shared_ptr<Channel>*>(
          this->epoll_events_[i].data.ptr);
      channel->set_descriptor(this->epoll_events_[i].data.fd);
      channel->set_event(this->epoll_events_[i].events);
      channel_events->emplace_back(channel);
    }

    return kNumberEvents;
  }

  void RemovePollEvent(const int& kSocketFileDescriptor) {
    this->channel_map_.erase(kSocketFileDescriptor);

    if (0 != ::epoll_ctl(this->file_descriptor_, EPOLL_CTL_DEL,
                         kSocketFileDescriptor, nullptr)) {
      throw std::runtime_error("Could not remove poll event");
    }
  }

  inline int get_file_descriptor() const { return this->file_descriptor_; }
};

}  // namespace io

}  // namespace net

}  // namespace openrs
