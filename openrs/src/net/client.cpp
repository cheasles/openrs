#include "OpenRS/net/client.h"

#include <sys/socket.h>

#include "OpenRS/net/codec/packet.h"

openrs::net::Client::Client()
    : status_(ClientStatus::kConnected)
    , socket_(0)
{
    this->decoder_ = std::make_unique<codec::Decoder>();
}

openrs::net::Client::~Client()
{

}

void openrs::net::Client::Read()
{
    std::vector<uint8_t> buffer;
    buffer.resize(Client::kReadSize);

    int amount_read = this->socket_.getMessageData(&buffer);
    if (-1 == amount_read)
    {
        this->status_ = ClientStatus::kDisconnected;
        return;
    }

    buffer.resize(amount_read);
    this->buffer_input_.insert(
        this->buffer_input_.cend(),
        buffer.cbegin(),
        buffer.cend());

    codec::Packet packet;
    if (!this->decoder_->Decode(this->buffer_input_, &packet))
    {
        return;
    }
}

void openrs::net::Client::Write()
{

}
