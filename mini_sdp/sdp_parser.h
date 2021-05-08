/**
 * @file mini_sdp/sdp_parser.h
 * @brief 
 * @version 0.1
 * @date 2021-01-07
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
#ifndef MINI_SDP_SDP_PARSER_H_
#define MINI_SDP_SDP_PARSER_H_

#include <functional>
#include "sdp.h"

namespace mini_sdp {

constexpr char kSdpAddrIP4[]      = "IP4";
constexpr char kSdpAddrIP6[]      = "IP6";

constexpr char kSdpTransSendRecv[] = "sendrecv";
constexpr char kSdpTransSendOnly[] = "sendonly";
constexpr char kSdpTransRecvOnly[] = "recvonly";
constexpr char kSdpTransInactive[] = "inactive";

constexpr char kSdpMediaAudio[] = "audio";
constexpr char kSdpMediaVideo[] = "video";
constexpr char kSdpMediaData[]  = "application";

constexpr char kSdpRoleActpass[] = "actpass";
constexpr char kSdpRoleActive[]  = "active";
constexpr char kSdpRolePassive[] = "passive";

constexpr char kSdpMediaProtoEncryptDefault[] = "UDP/TLS/RTP/SAVPF";
constexpr char kSdpMediaProtoNotEncryptDefault[] = "RTP/AVPF";

constexpr char kSdpCodecOpus[] = "opus";
constexpr char kSdpCodecLatm[] = "MP4A-LATM";
constexpr char kSdpCodecAdts[] = "MP4A-ADTS";
constexpr char kSdpCodecH264[] = "H264";
constexpr char kSdpCodecH265[] = "H265";

constexpr char kSdpCodecNack[] = "nack";
constexpr char kSdpCodecFlexFec[] = "flexfec-03";
constexpr char kSdpCodecTransportCc[] = "transport-cc";
constexpr char kSdpCodecGoogleRemb[] = "goog-remb";
constexpr char kSdpCodecBFrameEnabled[] = "bframe-enabled";
constexpr char kSdpCodecBFrameEnabled2[] = "BFrame-enabled";

constexpr char kSdpExtAbsSendTime[] = "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time";
constexpr char kSdpExtPayloutDelay[] = "http://www.webrtc.org/experiments/rtp-hdrext/playout-delay";
constexpr char kSdpExtTransportCc[] = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
constexpr char kSdpExtMetaData01[] = "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-01";
constexpr char kSdpExtMetaData02[] = "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-02";
constexpr char kSdpExtMetaData03[] = "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-03";
constexpr char kSdpExtDts[] = "http://www.webrtc.org/experiments/rtp-hdrext/decoding-timestamp";
constexpr char kSdpExtCts[] = "http://www.webrtc.org/experiments/rtp-hdrext/video-composition-time";
constexpr char kSdpExtVideoFrameType[] = "http://www.webrtc.org/experiments/rtp-hdrext/video-frame-type";

std::pair<SdpAddrType, bool> ParseSdpAddrType(const char* word, size_t len);
std::pair<SdpTransType, bool> ParseSdpTransType(const char* word, size_t len);
std::pair<SdpMediaType, bool> ParseSdpMediaType(const char* word, size_t len);
std::pair<SdpRoleType, bool> ParseSdpRoleType(const char* word, size_t len);

/**
 * @brief SessionDescription Parser
 * 
 */
class SdpParser {
  public:
    SdpParser(const char* data, size_t length);
    ~SdpParser();

    enum class StatCode : int {
        kNotParsed    = -1,
        kSuccess      = 0,
        kFormatError  = 1,
        kParamError   = 2,
        kUnknownLine  = 3
    };

    using StatInfo = std::pair<StatCode, std::string>;  // <code, message>

    /**
     * @brief Start Parse
     * 
     * @return true when success
     * @return false and set message of error
     */
    bool Parse();

    bool IsParsed() const { return stat_info_.first != StatCode::kNotParsed; }

    bool IsSucess() const { return stat_info_.first == StatCode::kSuccess; }

    SessionDescriptionPtr GetSessionDescription() { return sd_ptr_; }

    const StatInfo& GetErrorMessage() const { return stat_info_; }

  private:
    void setStatInfo(StatCode code, const std::string& msg, size_t line = 0);

    bool isInSessionLevel() { return !(bool)cur_media_ptr_; }

    bool isInMediaLevel() { return (bool)cur_media_ptr_; }

    // load line and store in <line_data_, line_length_, line_idx_>
    bool loadNextLine();

    bool parseLine();

    bool parseLineOrigin();

    bool parseLineSessionName();

    bool parseLineSessionInfo();

    bool parseLineConnection();

    bool parseLineMedia();

    bool parseLineAttribute();

    void appendMedia();

  private:
    const char* data_;
    size_t      length_;

    StatInfo stat_info_;
    SessionDescriptionPtr sd_ptr_;  

    MediaDescriptionPtr  cur_media_ptr_;
    uint64_t    cur_media_id_ = 0;  // would be used if 'a=mid' is not included
    size_t      line_idx_ = 0;
    const char* line_data_;
    size_t      line_length_;
};  // class SdpParser


using SessionAttrParseHandle = std::function<bool(SessionDescriptionPtr, std::string&&, const char*, size_t)>;
using MediaAttrParseHandle = std::function<bool(MediaDescriptionPtr, std::string&&, const char*, size_t)>;

bool SessionAttrParseGroup(SessionDescriptionPtr sess, std::string&& key, const char* data, size_t len);

bool MediaAttrParseIceUfrag(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseIcePwd(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseIceOptions(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseFingerprint(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseSetup(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseMid(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseExtmap(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseTransType(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseRtpmap(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseRtcpFb(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseFmtp(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseSsrc(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseCandidate(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);

bool MediaAttrParseMsid(MediaDescriptionPtr media, std::string&& key, const char* data, size_t len);


}  // namespace mini_sdp

#endif  // MINI_SDP_SDP_PARSER_H_
