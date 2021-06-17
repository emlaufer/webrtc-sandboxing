/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_WRAPPER_H_
#define MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_WRAPPER_H_

#include "modules/rtp_rtcp/source/forward_error_correction_impl.h"
struct DataWrapper {
  uint8_t * data;
  size_t size;
};

struct ReceivedPacketWrapper {
  uint32_t ssrc;
  uint16_t seq_num;
  bool is_fec;  // Set to true if this is an FEC packet and false
                // otherwise.
  bool is_recovered;
  uint8_t * data;
  size_t size;
};

struct RecoveredPacketWrapper {
  uint32_t ssrc;
  uint16_t seq_num;
  bool was_recovered;  // Will be true if this packet was recovered by
                       // the FEC. Otherwise it was a media packet passed in
                       // through the received packet list.
  bool returned;  // True when the packet already has been returned to the
  uint8_t * data;
  size_t size;
};

// namespace?
namespace webrtc {


  // Creates a ForwardErrorCorrection tailored for a specific FEC scheme.
  ForwardErrorCorrectionImpl* CreateCtor(
    uint32_t ssrc,
    uint32_t protected_media_ssrc, bool ulp);
  ForwardErrorCorrectionImpl* CreateUlpfec_c(uint32_t ssrc);
  ForwardErrorCorrectionImpl* CreateFlexfec_c(
      uint32_t ssrc,
      uint32_t protected_media_ssrc);
  int EncodeFec_c(ForwardErrorCorrectionImpl* impl, DataWrapper* media_packets,
                size_t media_packets_len,
                uint8_t protection_factor,
                int num_important_packets,
                bool use_unequal_protection,
                FecMaskType fec_mask_type,
                std::list<ForwardErrorCorrectionImpl::Packet*>** fec_packets);
  void DecodeFec_c(ForwardErrorCorrectionImpl* impl, ReceivedPacketWrapper* wrapper,
                 RecoveredPacketWrapper* recovered_packets, 
                 size_t recovered_len, 
                 std::list<std::unique_ptr<ForwardErrorCorrectionImpl::RecoveredPacket>>** recovered_packets_out);
  int NumFecPackets_c(int num_media_packets, int protection_factor);
  size_t MaxPacketOverhead_c(ForwardErrorCorrectionImpl* impl);
  void ResetState_c(ForwardErrorCorrectionImpl* impl, ForwardErrorCorrectionImpl::RecoveredPacketList* recovered_packets);
  uint16_t ParseSequenceNumber_c(const uint8_t* packet);
  uint32_t ParseSsrc_c(const uint8_t* packet);

} // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_ADAPTER_H_
