/**
 * @file mini_sdp/mini_sdp.cc
 * @brief 
 * @version 0.1
 * @date 2021-01-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <cstring>
#include <limits>
#include "mini_sdp.h"
#include "util.h"

namespace mini_sdp {

bool IsMiniSdpReqPack(const char* data, size_t len) {
    return len >= 4 && (uint8_t)data[0] == kMiniSdpPacketType && data[1] == 'S' && data[2] == 'D' && data[3] == 'P';
}

ssize_t ParseOriginSdpToMiniSdp(const OriginSdpAttr& attr, char* buff, size_t len) {
    if (attr.stream_url.size() > kMiniSdpUrlMaxLen) {
        return kSdpRetUrlExceeded;
    }
    MiniSdpPacker packer;
    int pack_size = packer.PackToDstMem(buff, len, attr.origin_sdp, attr.sdp_type, attr.stream_url, attr.svrsig, attr.seq, attr.status_code, attr.is_imm_send, attr.is_support_aac_fmtp, attr.is_push);
    if (pack_size == 0) {
        return kSdpRetWrongFormat;
    }
    if (pack_size > kMiniMiniSdpMaxLen || pack_size > len) {
        return kSdpRetSizeExceeded;
    }
    return pack_size;
}

ssize_t LoadMiniSdpToOriginSdp(const char* buff, size_t len, OriginSdpAttr& attr) {
    MiniSdpLoader loader;
    int parse_size = loader.ParseToString(const_cast<char *>(buff), len, attr.seq, attr.sdp_type, attr.origin_sdp, attr.stream_url, attr.svrsig, attr.status_code, attr.is_imm_send, attr.is_support_aac_fmtp, attr.is_push);
    return parse_size;
}

bool IsMiniSdpStopPack(const char* data, size_t len) {
    return len >= 4 && (uint8_t)data[0] == kMiniSdpPacketType && data[1] == 'S' && data[2] == 'T' && data[3] == 'P';
}

ssize_t BuildStopStreamPacket(char* buff, size_t len, const StopStreamAttr& attr) {
    size_t total_bytes = sizeof(StopStreamSignalHeader) + attr.svrsig.size() + kMiniSdpAuthLength;
    if (total_bytes > len || attr.svrsig.size() > std::numeric_limits<uint16_t>::max()) return kSdpRetSizeExceeded;

    StopStreamSignalHeader* hdr = (StopStreamSignalHeader*)buff;
    hdr->pack_type = kMiniSdpPacketType;
    memcpy(hdr->magic_word, "STP", 3);
    hdr->version = 0;
    hdr->status = htons(attr.status);
    hdr->seq = htons(attr.seq);
    hdr->svrsig_len = htons((uint16_t)attr.svrsig.size());
    
    buff += sizeof(StopStreamSignalHeader);
    memcpy(buff, attr.svrsig.c_str(), attr.svrsig.size());
    
    buff += attr.svrsig.size();
    memset(buff, 0, kMiniSdpAuthLength);

    return total_bytes;
}

ssize_t LoadStopStreamPacket(const char* buff, size_t len, StopStreamAttr& attr) {
    if (len < sizeof(StopStreamSignalHeader) + kMiniSdpAuthLength) return kSdpRetSizeExceeded;
    if ((uint8_t)*buff != kMiniSdpPacketType || buff[1] != 'S' || buff[2] != 'T' || buff[3] != 'P') {
        return kSdpRetWrongFormat;
    }

    const StopStreamSignalHeader* hdr = (const StopStreamSignalHeader*)buff;
    if (hdr->version != 0) return kSdpRetWrongFormat;

    attr.status = ntohs(hdr->status);
    attr.seq = ntohs(hdr->seq);
    uint16_t length = ntohs(hdr->svrsig_len);

    if (sizeof(StopStreamSignalHeader) + length + kMiniSdpAuthLength > len) {
        return kSdpRetSizeExceeded;
    }

    attr.svrsig.assign(buff + sizeof(StopStreamSignalHeader), length);
    return sizeof(StopStreamSignalHeader) + kMiniSdpAuthLength + length;
}

}  // namespace mini_sdp
