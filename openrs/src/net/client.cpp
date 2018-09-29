#include "OpenRS/net/client.h"

#include <sys/socket.h>

#include "OpenRS/net/codec/packet.h"

openrs::net::Client::Client()
    : status_(ClientStatus::kConnected)
    , socket_(0)
    , client_build_(0)
    , bytes_received_(0)
    , bytes_sent_(0)
{
    this->decoder_ = std::make_unique<codec::decoder::global::Decoder>();
    this->packet_handler_ = std::make_unique<
        openrs::net::codec::decoder::global::handlers::PacketHandler>();
    this->encoder_ = std::make_unique<codec::encoder::global::Encoder>();
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

    this->bytes_received_ += amount_read;

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

    this->packet_handler_->Handle(packet, this);
    this->buffer_input_.clear();
}

void openrs::net::Client::Write()
{
    this->buffer_output_.ClearOldData();

    this->bytes_sent_ += this->buffer_output_.size();

    this->socket_.putMessageData(this->buffer_output_);

    this->buffer_output_.clear();
}

void openrs::net::Client::Send(const openrs::net::io::Buffer& buffer)
{
    this->buffer_output_.insert(
        this->buffer_output_.cend(),
        buffer.cbegin(),
        buffer.cend());
}
