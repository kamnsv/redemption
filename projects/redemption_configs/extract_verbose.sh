#!/usr/bin/env sh

cd "$(dirname "$0")/../.."
extract() {
    sed -E '/REDEMPTION_VERBOSE_FLAGS/,/};/!d;/=/!d;s/^ *([^ =]+ *= *[^,]+).*/- \1/' "$@"
}

rdp_serializer=$(extract src/core/RDP/RDPSerializerVerbose.hpp)

echo "//
// DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN
//

namespace cfg_specs {

constexpr char const* CONFIG_DESC_AUTH = R\"($(extract src/acl/acl_serializer.hpp))\";

constexpr char const* CONFIG_DESC_FRONT = R\"($(extract src/front/front.hpp)

(Serializer)
$(echo "$rdp_serializer" | sed -E 's/0x[0-9]+/&'\''0000/'))\";

constexpr char const* CONFIG_DESC_RDP = R\"($(extract src/mod/rdp/rdp_verbose.hpp))\";

constexpr char const* CONFIG_DESC_VNC = R\"($(extract src/mod/vnc/vnc_verbose.hpp))\";

constexpr char const* CONFIG_DESC_SCK = R\"($(extract src/transport/socket_transport.hpp))\";

constexpr char const* CONFIG_DESC_CAPTURE = R\"(- kbd / ocr when != 0

(Wrm)
$rdp_serializer)\";

constexpr char const* CONFIG_DESC_SESSION = R\"($(sed -E '/^enum class SessionVerbose/,/^};/!d;/=/!d;s/^ *([^ =]+ *= *[^,]+).*/- \1/' src/core/session.cpp))\";

constexpr char const* CONFIG_DESC_MOD_INTERNAL = R\"(- copy_paste != 0
- client_execute = 0x01)\";

constexpr char const* CONFIG_DESC_COMPRESSION = R\"(- when != 0)\";

constexpr char const* CONFIG_DESC_CACHE = R\"($(extract src/core/RDP/caches/bmpcache.hpp))\";

constexpr char const* CONFIG_DESC_OCR = R\"(- when != 0)\";

}"
