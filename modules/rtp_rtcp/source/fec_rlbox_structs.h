
#ifndef MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_STRUCTS_H_
#define MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_STRUCTS_H_

#include "modules/rtp_rtcp/source/fec_rlbox_wrapper.h"

namespace webrtc {
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#elif defined(__GNUC__) || defined(__GNUG__)
// Can't turn off the variadic macro warning emitted from -pedantic
#  pragma GCC system_header
#elif defined(_MSC_VER)
// Doesn't seem to emit the warning
#else
// Don't know the compiler... just let it go through
#endif

#define sandbox_fields_reflection_fec_class_DataWrapper(f, g, ...) \
    f(uint8_t*, data       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(size_t, size    , FIELD_NORMAL, ##__VA_ARGS__) g() 

#define sandbox_fields_reflection_fec_class_ReceivedPacketWrapper(f, g, ...) \
    f(uint32_t, ssrc       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(uint16_t, seq_num      , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(bool, is_fec       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(bool, is_recovered       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(uint8_t*, data       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(size_t, size    , FIELD_NORMAL, ##__VA_ARGS__) g() 

#define sandbox_fields_reflection_fec_class_RecoveredPacketWrapper(f, g, ...) \
    f(uint32_t, ssrc       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(uint16_t, seq_num      , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(bool, was_recovered       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(bool, returned       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(uint8_t*, data       , FIELD_NORMAL, ##__VA_ARGS__) g()    \
    f(size_t, size    , FIELD_NORMAL, ##__VA_ARGS__) g() 

#define sandbox_fields_reflection_fec_allClasses(f, ...) \
  f(DataWrapper, fec, ##__VA_ARGS__) \
  f(ReceivedPacketWrapper, fec, ##__VA_ARGS__)                \
  f(RecoveredPacketWrapper, fec, ##__VA_ARGS__)                

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#elif defined(_MSC_VER)
#else
#endif

} // namespace webrtc


#endif  // MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_TYPES_H_
