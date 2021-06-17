
#ifndef MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_TYPES_H_
#define MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_TYPES_H_

#include "rlbox/code/include/rlbox_types.hpp"

#define WASM_SANDBOXING_FEC 0

#if WASM_SANDBOXING_FEC
namespace rlbox {
  class rlbox_lucet_sandbox;
}
using rlbox_fec_sandbox_type = rlbox::rlbox_lucet_sandbox;
#else
using rlbox_fec_sandbox_type = rlbox::rlbox_noop_sandbox;
#endif

using rlbox_sandbox_fec = rlbox::rlbox_sandbox<rlbox_fec_sandbox_type>;
template <typename T>
using sandbox_callback_fec =
rlbox::sandbox_callback<T, rlbox_fec_sandbox_type>;
template <typename T>
using tainted_fec = rlbox::tainted<T, rlbox_fec_sandbox_type>;
template <typename T>
using tainted_opaque_fec =
rlbox::tainted_opaque<T, rlbox_fec_sandbox_type>;
template <typename T>
using tainted_volatile_fec =
rlbox::tainted_volatile<T, rlbox_fec_sandbox_type>;
using rlbox::tainted_boolean_hint;


#endif  // MODULES_RTP_RTCP_SOURCE_FEC_RLBOX_TYPES_H_
