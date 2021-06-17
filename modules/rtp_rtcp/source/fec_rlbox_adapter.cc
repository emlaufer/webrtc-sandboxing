
#include "modules/rtp_rtcp/source/fec_rlbox_adapter.h"

#include "modules/rtp_rtcp/source/fec_rlbox_wrapper.h"

namespace webrtc {

rlbox_sandbox_fec* FecRlboxAdapter::mSandbox;

FecRlboxAdapter::FecRlboxAdapter() {
  // TODO
  if (!mSandbox) {
    mSandbox  = new rlbox_sandbox_fec();
    mSandbox->create_sandbox();
  }
}

FecRlboxAdapter::FecRlboxAdapter(
                         uint32_t ssrc,
                         uint32_t protected_media_ssrc, bool ulp) {
  if (!mSandbox) {
    mSandbox  = new rlbox_sandbox_fec();
    mSandbox->create_sandbox();
  }

  impl = mSandbox->invoke_sandbox_function(CreateCtor, ssrc, protected_media_ssrc, ulp);
}

FecRlboxAdapter::~FecRlboxAdapter() {
  // TODO
}

std::unique_ptr<FecRlboxAdapter> FecRlboxAdapter::CreateUlpfec(uint32_t ssrc) {
  // create an adapter and simply delegate to sandbox
  FecRlboxAdapter* adapter = new FecRlboxAdapter();
  adapter->impl = mSandbox->invoke_sandbox_function(CreateUlpfec_c, ssrc);
  return std::unique_ptr<FecRlboxAdapter>(adapter);
}

std::unique_ptr<FecRlboxAdapter> FecRlboxAdapter::CreateFlexfec(
      uint32_t ssrc,
      uint32_t protected_media_ssrc) {
  // create an adapter and simply delegate to sandbox
  FecRlboxAdapter* adapter = new FecRlboxAdapter();
  adapter->impl = mSandbox->invoke_sandbox_function(CreateFlexfec_c, ssrc, protected_media_ssrc);
  return std::unique_ptr<FecRlboxAdapter>(adapter);
}

int FecRlboxAdapter::EncodeFec(const PacketList& media_packets,
                uint8_t protection_factor,
                int num_important_packets,
                bool use_unequal_protection,
                FecMaskType fec_mask_type,
                std::list<Packet*>* fec_packets) {
  // HACK: current idea...we just copy data buffers over into sandbox
  //  memory... wrapper reconstructs packets. May lose subtyping info here
  auto media_packet_data = mSandbox->malloc_in_sandbox<DataWrapper>(media_packets.size());
  size_t i = 0;
  for (auto packet_iter = media_packets.begin(); packet_iter != media_packets.end(); packet_iter++) {
    size_t data_size = (*packet_iter)->data.size();
    auto media_packet_buffer = mSandbox->malloc_in_sandbox<uint8_t>(data_size);
    if ((*packet_iter)->data.data() == nullptr) {
      //printf("oof");
    }
    if (!media_packet_buffer) {
      //printf("oof2 %zu\n", data_size);
    }
    rlbox::memcpy(*mSandbox, media_packet_buffer, (*packet_iter)->data.data(), data_size);
    media_packet_data[i].data = media_packet_buffer;
    media_packet_data[i].size = data_size;
    i++;
  }
  // copy the media packets into the sandbox

  auto fec_packets_tainted = mSandbox->malloc_in_sandbox<std::list<Packet*>*>();
  int ret = mSandbox->invoke_sandbox_function(EncodeFec_c, impl, media_packet_data, media_packets.size(), protection_factor, num_important_packets, use_unequal_protection, fec_mask_type, fec_packets_tainted).UNSAFE_unverified();

  // todo(evan): totally unsafe. really what we need to do is copy the fec packets into application memory, or sandbox could mess with them
  *fec_packets = **fec_packets_tainted.UNSAFE_unverified();

  /*for (int i =) {
    size_t data_size = (*packet_iter)->data.size();
    auto media_packet_buffer = mSandbox->malloc_in_sandbox<uint8_t>(data_size);
    if ((*packet_iter)->data.data() == nullptr) {
      //printf("oof");
    }
    if (!media_packet_buffer) {
      //printf("oof2 %zu", data_size);
    }
    rlbox::memcpy(mSandbox, media_packet_buffer, (*packet_iter)->data.data(), data_size);
    media_packet_data[i].data = media_packet_buffer;
    media_packet_data[i].size = data_size;
    i++;
  }*/

  return ret;
}

void FecRlboxAdapter::DecodeFec(const ReceivedPacket& received_packet,
                 RecoveredPacketList* recovered_packets) {
  //printf("entering decode\n");
  // copy recv packet into sandbox...
  // copy args to sandbox
  // copy received packet
  auto received_packet_tainted = mSandbox->malloc_in_sandbox<ReceivedPacketWrapper>();
  size_t data_size = received_packet.pkt->data.size();
  auto media_packet_buffer = mSandbox->malloc_in_sandbox<uint8_t>(data_size);
  rlbox::memcpy(*mSandbox, media_packet_buffer, received_packet.pkt->data.data(), data_size);
  // set members
  received_packet_tainted->ssrc = received_packet.ssrc;
  received_packet_tainted->seq_num = received_packet.seq_num;
  received_packet_tainted->is_fec = received_packet.is_fec;
  received_packet_tainted->is_recovered = received_packet.is_recovered;
  received_packet_tainted->data = media_packet_buffer;
  received_packet_tainted->size = data_size;

  //printf("copying recovered packets\n");
  // copy recovered packets
  tainted_fec<RecoveredPacketWrapper*> recovered_packet_data;
  if (recovered_packets->size()) {
    //printf("copying data here\n");
    recovered_packet_data = mSandbox->malloc_in_sandbox<RecoveredPacketWrapper>(recovered_packets->size());
    size_t i = 0;
    for (auto packet_iter = recovered_packets->begin(); packet_iter != recovered_packets->end(); packet_iter++) {
      //printf("iter...\n");
      size_t data_size = (*packet_iter)->pkt->data.size();
      auto req_packet_buffer = mSandbox->malloc_in_sandbox<uint8_t>(data_size);
      rlbox::memcpy(*mSandbox, req_packet_buffer, (*packet_iter)->pkt->data.data(), data_size);

      recovered_packet_data[i].ssrc = (*packet_iter)->ssrc;
      recovered_packet_data[i].seq_num = (*packet_iter)->seq_num;
      recovered_packet_data[i].returned = (*packet_iter)->returned;
      recovered_packet_data[i].was_recovered = (*packet_iter)->was_recovered;
      recovered_packet_data[i].data = req_packet_buffer;
      recovered_packet_data[i].size = data_size;
      i++;
    }
  }


  //printf("done iter\n");
  auto recovered_packets_tainted = mSandbox->malloc_in_sandbox<RecoveredPacketList*>();
  *recovered_packets_tainted = nullptr;
  // TODO: I'm sure there are bugs, fix em. i.e. output params don't work rn
  //printf("invoking c wrapper\n");
  mSandbox->invoke_sandbox_function(DecodeFec_c, impl, received_packet_tainted, recovered_packet_data, recovered_packets->size(), recovered_packets_tainted);

  //printf("Now we inside Decode adapter\n");
  // extend packets with new ones...
  ////printf("stuck...size huh? %zun", (*recovered_packets_tainted).UNSAFE_unverified()->size());
  // check if null...
  if (!*recovered_packets_tainted.UNSAFE_unverified()) {
    return;
  }

  //printf("pushign back\n");
  /*for (auto packet_iter = std::make_move_iterator((*recovered_packets_tainted).UNSAFE_unverified()->begin()); packet_iter != std::make_move_iterator((*recovered_packets_tainted).UNSAFE_unverified()->end()); packet_iter++) {
    //printf("got copy: %zu\n", (*packet_iter)->pkt->data.size());
    recovered_packets->push_back(*packet_iter);
  }
  //printf("ensuring consistent...\n");
  for (auto packet_iter = recovered_packets->begin(); packet_iter != recovered_packets->end(); packet_iter++) {
    //printf("got: %zu\n", (*packet_iter)->pkt->data.size());
  }*/
  //printf("donzo\n");
  *recovered_packets = std::move(**recovered_packets_tainted.UNSAFE_unverified());

  mSandbox->free_in_sandbox(received_packet_tainted);
  if (recovered_packets->size()) {
    size_t i = 0;
    for (auto packet_iter = recovered_packets->begin(); packet_iter != recovered_packets->end(); packet_iter++) {
      //printf("iter...\n");
      //mSandbox->free_in_sandbox(recovered_packet_data[i].data);
      i++;
    }
    //mSandbox->free_in_sandbox(recovered_packet_data);
  }
  // death...
  //impl.UNSAFE_unverified()->DecodeFec(received_packet, recovered_packets);
}

int FecRlboxAdapter::NumFecPackets(int num_media_packets, int protection_factor) {
  return mSandbox->invoke_sandbox_function(NumFecPackets_c, num_media_packets, protection_factor).UNSAFE_unverified();
}

size_t FecRlboxAdapter::MaxPacketOverhead() {
  return mSandbox->invoke_sandbox_function(MaxPacketOverhead_c, impl).UNSAFE_unverified();
}

void FecRlboxAdapter::ResetState(RecoveredPacketList* recovered_packets) {
  auto recovered_packets_tainted = mSandbox->malloc_in_sandbox<RecoveredPacketList>();
  mSandbox->invoke_sandbox_function(ResetState_c, impl, recovered_packets_tainted);
}

uint16_t FecRlboxAdapter::ParseSequenceNumber(const uint8_t* packet) {
  auto packet_tainted = mSandbox->malloc_in_sandbox<const uint8_t>();
  return mSandbox->invoke_sandbox_function(ParseSequenceNumber_c, packet_tainted).UNSAFE_unverified();
}

uint32_t FecRlboxAdapter::ParseSsrc(const uint8_t* packet) {
  auto packet_tainted = mSandbox->malloc_in_sandbox<const uint8_t>();
  return mSandbox->invoke_sandbox_function(ParseSsrc_c, packet_tainted).UNSAFE_unverified();
}

} // namespace webrtc
