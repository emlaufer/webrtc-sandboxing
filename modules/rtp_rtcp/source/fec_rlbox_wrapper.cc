
#include "modules/rtp_rtcp/source/fec_rlbox_wrapper.h"

namespace webrtc {

ForwardErrorCorrectionImpl* CreateCtor(
    uint32_t ssrc,
    uint32_t protected_media_ssrc, bool ulp) {
  return new ForwardErrorCorrectionImpl(ssrc, protected_media_ssrc, ulp);
}

ForwardErrorCorrectionImpl* CreateUlpfec_c(uint32_t ssrc) {
  auto ptr = ForwardErrorCorrectionImpl::CreateUlpfec(ssrc);
  return ptr.release();
}

ForwardErrorCorrectionImpl* CreateFlexfec_c(
      uint32_t ssrc,
      uint32_t protected_media_ssrc) {
  auto ptr = ForwardErrorCorrectionImpl::CreateFlexfec(ssrc, protected_media_ssrc);
  return ptr.release();
}

int EncodeFec_c(ForwardErrorCorrectionImpl* impl, DataWrapper* media_wrappers,
                size_t media_packets_len,
                uint8_t protection_factor,
                int num_important_packets,
                bool use_unequal_protection,
                FecMaskType fec_mask_type,
                std::list<ForwardErrorCorrectionImpl::Packet*>** fec_packets) {
  // convert array of Packets into Packetlist which ForwardErrorCorrectionImpl expects
  *fec_packets = new std::list<ForwardErrorCorrectionImpl::Packet*>();
  ForwardErrorCorrectionImpl::PacketList media_packet_list;
  for (size_t i = 0; i < media_packets_len; i++) {
    // make packet
    rtc::CopyOnWriteBuffer b(media_wrappers[i].data, media_wrappers[i].size);
    ForwardErrorCorrectionImpl::Packet* p = new ForwardErrorCorrectionImpl::Packet();
    p->data = b;
    media_packet_list.push_back(std::unique_ptr<ForwardErrorCorrectionImpl::Packet>(p));
  }

  return impl->EncodeFec(media_packet_list, protection_factor, num_important_packets, use_unequal_protection, fec_mask_type, *fec_packets);
}

void DecodeFec_c(ForwardErrorCorrectionImpl* impl, ReceivedPacketWrapper* wrapper,
                 RecoveredPacketWrapper* recovered_wrappers, size_t recovered_len,std::list<std::unique_ptr<ForwardErrorCorrectionImpl::RecoveredPacket>>** recovered_packets_out) {
  //printf("Inside c wrapper\n");
  // convert recv packet wrapper
  ForwardErrorCorrectionImpl::ReceivedPacket pkt;

  rtc::CopyOnWriteBuffer b(wrapper->data, wrapper->size);
  ForwardErrorCorrectionImpl::Packet* p = new ForwardErrorCorrectionImpl::Packet();
  p->data = b;
  auto refc_p = rtc::scoped_refptr<ForwardErrorCorrectionImpl::Packet>(p);

  pkt.ssrc = wrapper->ssrc;
  pkt.seq_num = wrapper->seq_num;
  pkt.is_fec = wrapper->is_fec;
  pkt.is_recovered = wrapper->is_recovered;
  pkt.pkt = refc_p;

  //printf("converting list: len %zu\n", recovered_len);
  // convert recovered list
  ForwardErrorCorrectionImpl::RecoveredPacketList recovered_list;
  for (size_t i = 0; i < recovered_len; i++) {
    // make packet
    //printf("making buffer %zu\n", i);
    rtc::CopyOnWriteBuffer b1(recovered_wrappers[i].data, recovered_wrappers[i].size);
    //printf("new packet\n");
    ForwardErrorCorrectionImpl::Packet* p1 = new ForwardErrorCorrectionImpl::Packet();
    //printf("data = buffer\n");
    p1->data = b1;
    //printf("scoped ref\n");
    auto refc_p1 = rtc::scoped_refptr<ForwardErrorCorrectionImpl::Packet>(p1);

    //printf("new packet\n");
    ForwardErrorCorrectionImpl::RecoveredPacket* rp = new ForwardErrorCorrectionImpl::RecoveredPacket();
    //printf("setting packet items\n");
    rp->pkt = refc_p1;
    rp->ssrc = recovered_wrappers[i].ssrc;
    rp->seq_num = recovered_wrappers[i].seq_num;
    rp->was_recovered = recovered_wrappers[i].was_recovered;
    rp->returned = recovered_wrappers[i].returned;
    //printf("pushing back\n");
    recovered_list.push_back(std::unique_ptr<ForwardErrorCorrectionImpl::RecoveredPacket>(rp));
  }
  //printf("moving list\n");
  *recovered_packets_out = new ForwardErrorCorrectionImpl::RecoveredPacketList();
  **recovered_packets_out = std::move(recovered_list);
  // check consistency
  //printf("check consistency of original\n");
  //for (auto packet_iter = recovered_list.begin(); packet_iter != recovered_list.end(); packet_iter++) {
    //printf("got copy: %zu\n", (*packet_iter)->pkt->data.size());
  //}

  // check consistency
  //for (auto packet_iter = (*recovered_packets_out)->begin(); packet_iter != (*recovered_packets_out)->end(); packet_iter++) {
    //printf("got copy: %zu\n", (*packet_iter)->pkt->data.size());
  //}
  //impl->MaxPacketOverhead();
  //printf("calling impl\n");
  impl->DecodeFec(pkt, *recovered_packets_out);
  //printf("done with decode\n");
}

int NumFecPackets_c(int num_media_packets, int protection_factor) {
  return ForwardErrorCorrectionImpl::NumFecPackets(num_media_packets, protection_factor);
}

size_t MaxPacketOverhead_c(ForwardErrorCorrectionImpl* impl) {
  return impl->MaxPacketOverhead();
}

void ResetState_c(ForwardErrorCorrectionImpl* impl, ForwardErrorCorrectionImpl::RecoveredPacketList* recovered_packets) {
  impl->ResetState(recovered_packets);
}

uint16_t ParseSequenceNumber_c(const uint8_t* packet) {
  return ForwardErrorCorrectionImpl::ParseSequenceNumber(packet);
} 

uint32_t ParseSsrc_c(const uint8_t* packet) {
  return ForwardErrorCorrectionImpl::ParseSsrc(packet);
}

} // namespace webrtc
