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
#include "sdp.h"

namespace mini_sdp {

/**
 * @brief Return Code
 *  统一返回码
 */
enum SdpRetcode {
    kSdpRetWrongFormat    = -1,     // 格式错误
    kSdpRetSizeExceeded   = -2,     // 打包结果超过所提供的 buffer 大小
    kSdpRetUrlExceeded    = -3      // 流 URL 过长，使得无法存入所有提供的 buffer
};

/**
 * @brief Stream Direction
 *  流类型（传输方向）
 *  - 可为拉流、推流或者默认
 */
enum StreamDirection {
    kStreamDefault = -1,
    kStreamPull = 0,    // 拉流
    kStreamPush = 1,    // 推流
};

/**
 * @brief Origin SDP Attribute
 *  原始 SDP 属性，原始 SDP 与 mini sdp 互转的参数结构
 */
struct OriginSdpAttr {
    // SDP Type
    // - SDP 类型，offer 或者 answer
    // - SdpType的定义见 mini_sdp/sdp.h
    SdpType             sdp_type;

    // Origin SDP
    // - 原始SDP
    std::string         origin_sdp;

    // Stream Url
    // - 流URL，格式如：webrtc://<domain>/[<path>/]<stream id>
    std::string         stream_url;

    // Server Signature
    // - 服务端标识，唯一标识 Session
    // - 停流请求需要设置该值
    // - offer 不需要该值
    // - 格式: <ip>:<ice-ufrag in answer>:<ice-ufrag in offer>
    // * 服务端需要保存 answer （响应UDP）中的 svrsig
    std::string         svrsig;

    // Status Code
    // - 响应状态码，仅在 answer 中为有效值
    int                 status_code = 0;

    // Sequence
    // - 请求序号
    // - UDP 可能会有丢包的情况，为保证到达服务端，需要有一定的重试策略，
    //   相同请求的 seq 需要保持一致，并且需要确保同一客户端不同请求的 seq 是不一致，
    //   建议客户端本地对 seq 递增处理。
    // - 服务端可以根据 seq 来过滤重复 UDP 请求
    // * 客户端需要保存当前请求的 seq
    uint16_t            seq = 0;

    // Flag: Immediately Sneding
    // - 立即发送标志位，即 0-RTT
    // - 若开启，表示客户端有能力直接立即接收媒体数据,
    //   服务端可以向该 UDP 请求的源地址发送媒体数据
    bool                is_imm_send = false;

    // Flag: Aac Fmtp Supported
    // - Aac 参数标志位
    // - 表示是否保留 ADTS 和 LATM 音频的 fmtp属性，默认开启
    // - 这是用于 v0 版本兼容不同子版本的标志位，v1 版本将不再需要
    bool                is_support_aac_fmtp = true;

    // Flag: Stream Direction
    // - 流类型标志位，指示拉流或者推流
    // - 默认表示依据原始 SDP 的描述
    StreamDirection     is_push = kStreamDefault;   // -1 not have field , 0 false, 1 true
};  // struct OriginSdpAttr

/**
 * @brief Status Code
 *  响应码，与 http 接口保持一致
 */
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
 *  检查 UDP 包是否为 mini sdp 请求
 * @param data 
 * @param len 
 * @return true 
 * @return false 
 */
bool IsMiniSdpReqPack(const char* data, size_t len);

/**
 * @brief Load mini_sdp to origin_sdp
 *  将 mini sdp 转换成原始 SDP
 * @param buff mini_sdp
 * @param len mini_sdp
 * @param attr result
 * @return int SdpRetCode or size of mini_sdp
 */
ssize_t LoadMiniSdpToOriginSdp(const char* buff, size_t len, OriginSdpAttr& attr);

/**
 * @brief Parse origin_sdp to mini_sdp
 *  将原始 SDP 转换成 mini sdp
 * @param attr origin sdp
 * @param buff mini_sdp
 * @param len mini_sdp
 * @return int SdpRetCode or size of mini_sdp
 */
ssize_t ParseOriginSdpToMiniSdp(const OriginSdpAttr& attr, char* buff, size_t len);

/**
 * @brief Stop Stream Attribute
 *  停流参数
 */
struct StopStreamAttr {
    // Server Signature
    // - 服务端标识
    // * 与 answer （响应 UDP）的 svrsig 保持一致
    std::string svrsig;

    // Status Code
    // - 响应状态码，仅在响应中为有效值
    uint16_t    status = 0;

    // Sequence
    // - 请求序号
    // * 与请求的 seq 保持一致
    uint16_t    seq = 0;
};  // struct StopStreamAttr

/**
 * @brief Check Stop Packet
 *  检查 UDP 包是否为 mini sdp 停流包
 * @param data 
 * @param len 
 * @return true 
 * @return false 
 */
bool IsMiniSdpStopPack(const char* data, size_t len);

/**
 * @brief Build packet for stop stream
 *  构建 mini sdp 停流 UDP 包
 * @param buff packet
 * @param len packet
 * @param attr
 * @return ssize_t 
 */
ssize_t BuildStopStreamPacket(char* buff, size_t len, const StopStreamAttr& attr);

/**
 * @brief Load Response of Stop Stream Packet
 *  解析 mini sdp 停流 UDP 包
 * @param buff 
 * @param len 
 * @param attr
 * @return ssize_t 
 */
ssize_t LoadStopStreamPacket(const char* buff, size_t len, StopStreamAttr& attr);

}  // namespace mini_sdp

#endif  // MINI_SDP_MINI_SDP_H_
