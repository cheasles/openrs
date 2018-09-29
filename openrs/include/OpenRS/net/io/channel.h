#pragma once

#include <functional>

namespace openrs
{

namespace net
{

namespace io
{

class Channel
{
protected:
    int descriptor_;
    int event_;

public:
    Channel()
        : descriptor_(0)
        , event_(0)
    { }

    virtual bool HandleEvent() { return true; };

    inline int descriptor() const
    {
        return this->descriptor_;
    }

    inline int event() const
    {
        return this->event_;
    }

    inline void set_descriptor(const int& descriptor)
    {
        this->descriptor_ = descriptor;
    }

    inline void set_event(const int& event)
    {
        this->event_ = event;
    }
};

class CallbackChannel
    : public Channel
{
public:
    typedef std::function<void(const std::shared_ptr<CallbackChannel>&)> callback_type;

protected:
    callback_type callback_;

public:
    inline callback_type callback() const
    {
        return this->callback_;
    }

    inline void set_callback(callback_type callback)
    {
        this->callback_ = callback;
    }
};

class LoginChannel
    : public CallbackChannel
{ };

class ClientChannel
    : public CallbackChannel
{ };

}  // namespace io

}  // namespace net

}  // namespace openrs
