/**
 * @file mini_sdp/mini_sdp.h
 * @brief 
 * @version 0.1
 * @date 2021-01-14
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
#ifndef MINI_SDP_MINI_SDP_H_
#define MINI_SDP_MINI_SDP_H_

#include <string>
#include <vector>
#include "mini_sdp_impl.h"

namespace mini_sdp {

enum SdpRetcode {
    kSdpRetWrongFormat    = -1,
    kSdpRetSizeExceeded   = -2,
    kSdpRetUrlExceeded    = -3
};

struct OriginSdpAttr {
    SdpType     sdp_type;       // offer sdp or answer sdp
    std::string origin_sdp;
    std::string stream_url;
    std::string svrsig;         // unique id for session.
                                // format: <ip>:<ice-ufrag in answer>:<ice-ufrag in offer>
                                // it would be used in stopping stream.
                                // keep it empty when parse origin sdp to mini sdp.
    int         status_code = 0;    // offer sdp not need
    uint16_t    seq = 0;
    bool        is_imm_send = false;    // sending media data immediately
    bool        is_support_aac_fmtp = true;
    bool        is_push = false;
};

enum StatusCode {
    kStatCodeSuccess      = 0,
    kStatCodeFormatError  = 100,  // sdp format error
    kStatCodeParamError   = 101,  // parameters of request error
    kStatCodeInfoError    = 102,  // stream info error
    kStatCodeAuthError    = 103,  // auth error
    kStatCodeNotFound     = 104   // stream not existed
};

/**
 * @brief Check Request Packet
 * 
 * @param data 
 * @param len 
 * @return true 
 * @return false 
 */
bool IsMiniSdpReqPack(const char* data, size_t len);

/**
 * @brief Load mini_sdp to origin_sdp
 * 
 * @param buff mini_sdp
 * @param len mini_sdp
 * @param attr result
 * @return int SdpRetCode or size of mini_sdp
 */
ssize_t LoadMiniSdpToOriginSdp(const char* buff, size_t len, OriginSdpAttr& attr);

/**
 * @brief Parse origin_sdp to mini_sdp
 * 
 * @param attr origin sdp
 * @param buff mini_sdp
 * @param len mini_sdp
 * @return int SdpRetCode or size of mini_sdp
 */
ssize_t ParseOriginSdpToMiniSdp(const OriginSdpAttr& attr, char* buff, size_t len);

struct StopStreamAttr {
    std::string svrsig;
    uint16_t    status = 0;
    uint16_t    seq = 0;
};

/**
 * @brief Check Stop Packet
 * 
 * @param data 
 * @param len 
 * @return true 
 * @return false 
 */
bool IsMiniSdpStopPack(const char* data, size_t len);

/**
 * @brief Build packet for stop stream
 * 
 * @param buff packet
 * @param len packet
 * @param svrsig 
 * @param seq 
 * @param status 
 * @return ssize_t 
 */
ssize_t BuildStopStreamPacket(char* buff, size_t len, const StopStreamAttr& attr);

/**
 * @brief Load Response of Stop Stream Packet
 * 
 * @param buff 
 * @param len 
 * @param status return StatusCode when status is not nullptr
 * @param seq return Seq when seq is not nullptr
 * @return ssize_t 
 */
ssize_t LoadStopStreamPacket(const char* buff, size_t len, StopStreamAttr& attr);

}  // namespace mini_sdp

#endif  // MINI_SDP_MINI_SDP_H_
