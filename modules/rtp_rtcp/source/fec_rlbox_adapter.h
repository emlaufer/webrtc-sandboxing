/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_ADAPTER_H_
#define MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_ADAPTER_H_

#define RLBOX_SINGLE_THREADED_INVOCATIONS
#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol

#include "rlbox/code/include/rlbox.hpp"
#include "rlbox/code/include/rlbox_noop_sandbox.hpp"
#include "modules/rtp_rtcp/source/fec_rlbox_types.h"

#include "modules/rtp_rtcp/source/fec_rlbox_wrapper.h"
#include "modules/rtp_rtcp/source/fec_rlbox_structs.h"
rlbox_load_structs_from_library(fec);

#include "modules/rtp_rtcp/source/forward_error_correction_impl.h"

namespace webrtc {
using namespace rlbox;

class FecRlboxAdapter {
public:
  FecRlboxAdapter();
  ~FecRlboxAdapter();

  using Packet = ForwardErrorCorrectionImpl::Packet;
  using SortablePacket = ForwardErrorCorrectionImpl::SortablePacket;
  using ReceivedPacket = ForwardErrorCorrectionImpl::ReceivedPacket;
  using RecoveredPacket = ForwardErrorCorrectionImpl::RecoveredPacket;
  using ProtectedPacket = ForwardErrorCorrectionImpl::ProtectedPacket;
  using ProtectedPacketList = ForwardErrorCorrectionImpl::ProtectedPacketList;
  using ReceivedFecPacket = ForwardErrorCorrectionImpl::ReceivedFecPacket;
  using PacketList = ForwardErrorCorrectionImpl::PacketList;
  using RecoveredPacketList = ForwardErrorCorrectionImpl::RecoveredPacketList;
  using ReceivedFecPacketList = ForwardErrorCorrectionImpl::ReceivedFecPacketList;

  // Creates a ForwardErrorCorrection tailored for a specific FEC scheme.
  static std::unique_ptr<FecRlboxAdapter> CreateUlpfec(uint32_t ssrc);
  static std::unique_ptr<FecRlboxAdapter> CreateFlexfec(
      uint32_t ssrc,
      uint32_t protected_media_ssrc);

  // Generates a list of FEC packets from supplied media packets.
  //
  // Input:  media_packets          List of media packets to protect, of type
  //                                Packet. All packets must belong to the
  //                                same frame and the list must not be empty.
  // Input:  protection_factor      FEC protection overhead in the [0, 255]
  //                                domain. To obtain 100% overhead, or an
  //                                equal number of FEC packets as
  //                                media packets, use 255.
  // Input:  num_important_packets  The number of "important" packets in the
  //                                frame. These packets may receive greater
  //                                protection than the remaining packets.
  //                                The important packets must be located at the
  //                                start of the media packet list. For codecs
  //                                with data partitioning, the important
  //                                packets may correspond to first partition
  //                                packets.
  // Input:  use_unequal_protection Parameter to enable/disable unequal
  //                                protection (UEP) across packets. Enabling
  //                                UEP will allocate more protection to the
  //                                num_important_packets from the start of the
  //                                media_packets.
  // Input:  fec_mask_type          The type of packet mask used in the FEC.
  //                                Random or bursty type may be selected. The
  //                                bursty type is only defined up to 12 media
  //                                packets. If the number of media packets is
  //                                above 12, the packet masks from the random
  //                                table will be selected.
  // Output: fec_packets            List of pointers to generated FEC packets,
  //                                of type Packet. Must be empty on entry.
  //                                The memory available through the list will
  //                                be valid until the next call to
  //                                EncodeFec().
  //
  // Returns 0 on success, -1 on failure.
  //
  int EncodeFec(const PacketList& media_packets,
                uint8_t protection_factor,
                int num_important_packets,
                bool use_unequal_protection,
                FecMaskType fec_mask_type,
                std::list<Packet*>* fec_packets);

  // Decodes a list of received media and FEC packets. It will parse the
  // |received_packets|, storing FEC packets internally, and move
  // media packets to |recovered_packets|. The recovered list will be
  // sorted by ascending sequence number and have duplicates removed.
  // The function should be called as new packets arrive, and
  // |recovered_packets| will be progressively assembled with each call.
  // When the function returns, |received_packets| will be empty.
  //
  // The caller will allocate packets submitted through |received_packets|.
  // The function will handle allocation of recovered packets.
  //
  // Input:  received_packets   List of new received packets, of type
  //                            ReceivedPacket, belonging to a single
  //                            frame. At output the list will be empty,
  //                            with packets either stored internally,
  //                            or accessible through the recovered list.
  // Output: recovered_packets  List of recovered media packets, of type
  //                            RecoveredPacket, belonging to a single
  //                            frame. The memory available through the
  //                            list will be valid until the next call to
  //                            DecodeFec().
  //
  void DecodeFec(const ReceivedPacket& received_packet,
                 RecoveredPacketList* recovered_packets);

  // Get the number of generated FEC packets, given the number of media packets
  // and the protection factor.
  static int NumFecPackets(int num_media_packets, int protection_factor);

  // Gets the maximum size of the FEC headers in bytes, which must be
  // accounted for as packet overhead.
  // todo(evan): const breaks this cause rlbox can't handle the types
  size_t MaxPacketOverhead();

  // Reset internal states from last frame and clear |recovered_packets|.
  // Frees all memory allocated by this class.
  void ResetState(RecoveredPacketList* recovered_packets);

  // TODO(brandtr): Remove these functions when the Packet classes
  // have been refactored.
  static uint16_t ParseSequenceNumber(const uint8_t* packet);
  static uint32_t ParseSsrc(const uint8_t* packet);

  FecRlboxAdapter(uint32_t ssrc,
                         uint32_t protected_media_ssrc, bool ulp);

private:
  static rlbox_sandbox_fec* mSandbox;

  tainted_fec<ForwardErrorCorrectionImpl *> impl;
};

} // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_ADAPTER_H_
