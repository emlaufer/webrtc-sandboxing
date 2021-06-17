/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_FORWARD_ERROR_CORRECTION_H_
#define MODULES_RTP_RTCP_SOURCE_FORWARD_ERROR_CORRECTION_H_

#include "modules/rtp_rtcp/source/forward_error_correction_impl.h"
#include "modules/rtp_rtcp/source/fec_rlbox_adapter.h"

namespace webrtc {

#define SANDBOX_FEC 1
#if SANDBOX_FEC
typedef FecRlboxAdapter ForwardErrorCorrection;
#else
typedef ForwardErrorCorrectionImpl ForwardErrorCorrection;
#endif // SANDBOX_FEC

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_FORWARD_ERROR_CORRECTION_H_
