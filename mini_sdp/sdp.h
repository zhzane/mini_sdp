/**
 * @file mini_sdp/sdp.h
 * @brief 
 * @version 0.1
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef MINI_SDP_SDP_H_
#define MINI_SDP_SDP_H_

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mini_sdp {

constexpr char kSdpEndOfLine[] = "\r\n";
constexpr char kSdpPlaceholder[] = "-";
constexpr uint16_t kSdpMediaPortDefault = 9;  // webrtc set it to 9

enum class SdpType {
  kOffer = 0,
  kAnswer,
  kSdpNone
};

// Address Type
enum class SdpAddrType {
    kIPv4 = 0,
    kIPv6
};

// Media Type
enum class SdpMediaType {
    kAudio = 0,
    kVideo,
    kData   // would be supported latter
};

// Transport Type
enum class SdpTransType {
    kTransNone = 0,   // means not set
    kSendRecv,
    kRecvOnly,
    kSendOnly,
    kInactive
};

// Role Type for DTLS
enum class SdpRoleType {
    kRoleNone = 0,  // means not set
    kActpass,
    kActive,
    kPassive
};

using IpPort = std::pair<std::string, uint16_t>;


/**
 * @brief Codec Description
 * 
 */
class CodecDescription {
  public:
    CodecDescription() = default;
    CodecDescription(const std::string& name, uint32_t sample_rate, uint16_t channels = 0);

  public:
    // a=rtpmap:<fmt> <name>/<sample_rate>[/<channels>]
    std::string   Name;             // <name>
    uint8_t       Format      = 0;  // <format>
    uint16_t      Channels    = 0;  // <channels>
    uint32_t      SampleRate  = 0;  // <sample_rate>

    // a=rtcp-fb:<fmt> <value>
    std::set<std::string> Feedbacks;

    // a=fmtp:<fmt> <key>=<value>[;<key>=<value>]
    std::map<std::string, std::string> FormatParams;

  public:
    // simple equal: only compare with <name> <channel> <sample_rate>
    bool IsSimpleEqual(const CodecDescription& rhs) const;

    // strict equal: compare with all attributes
    bool IsStrictEqual(const CodecDescription& rhs) const;

    const std::string& GetFormatParam(const std::string& key, const std::string& def_val) const;

    bool HasAttribute(const std::string& key) const;

    const std::string& GetAttribute(const std::string& key) const;

    void SetAttribute(const std::string& key, const std::string& value);

    std::string ToString() const;

  private:
    // all attributes: exclude a=rtcp and a=fmtp
    // a=<key>:<fmt> <value>
    std::map<std::string, std::string> attributes_;
};  // class CodecDescription

using CodecDescriptionPtr = std::shared_ptr<CodecDescription>;

inline CodecDescriptionPtr MakeCodecDescription() { return std::make_shared<CodecDescription>(); }


/**
 * @brief Track Description
 * 
 */
class TrackDescription {
  public:
    TrackDescription() = default;
    explicit TrackDescription(uint32_t ssrc);
  
  public:
    uint32_t  Ssrc;   // <ssrc>

  public:
    // simple equal: only compare <ssrc>
    bool IsSimpleEqual(const TrackDescription& rhs) const { return Ssrc == rhs.Ssrc; }

    bool IsStrictEqaul(const TrackDescription& rhs) const { return IsSimpleEqual(rhs) && attributes_ == rhs.attributes_; }

    bool HasAttribute(const std::string& key) const;

    const std::string& GetAttribute(const std::string& key) const;

    void SetAttribute(const std::string& key, const std::string& value);

    std::string ToString() const;

  private:
    // all attributes: exclude a=ssrc:<ssrc> cname
    // a=ssrc:<ssrc> <key>:<value>
    std::map<std::string, std::string> attributes_;
};  // class TrackDescription

using TrackDescriptionPtr = std::shared_ptr<TrackDescription>;

inline TrackDescriptionPtr MakeTrackDescription() { return std::make_shared<TrackDescription>(); }


/**
 * @brief Media Description
 * 
 */
class MediaDescription {
  public:
    MediaDescription() = default;
    explicit MediaDescription(SdpMediaType type, bool is_encrypt);

  public:
    // m=<media> <port> <protos> <fmt> ...
    SdpMediaType  MediaType;                    // <media>
    uint16_t      Port = kSdpMediaPortDefault;  // <port>
    std::string   Protos;           // <protos>: A/B/C

    // a=mid:
    std::string   MediaId;
    // used for datachannel(m=application)
    std::string   MediaName;

    // a=ice-ufrag:<value>
    std::string   IceUfrag;
    // a=ice-pwd:<value>
    std::string   IcePwd;
    // a=ice-option:<value>
    std::string   IceOptions;
    // a=msid:<id> <appdata>
    std::string   StreamId;
    std::string   TrackId;

    // ipv4 / ipv6 : decode from 'c=' / 'a=rtcp:9'
    SdpAddrType   AddrType = SdpAddrType::kIPv4;
    // a=sendrecv / a=sendonly / a=recvonly / a=inactive
    SdpTransType  TransType = SdpTransType::kTransNone;
    // a=setup
    SdpRoleType   RoleType = SdpRoleType::kRoleNone;

    // a=candidate:foundation 
    // SDP support multi-candidates, but we get one of it.
    IpPort        Candidate;

    // a=extmap:<ext_id> <uri>
    std::map<uint8_t, std::string>  ExtMap;

    // codecs: <fmt> <Codec>
    std::map<uint8_t, CodecDescriptionPtr> Codecs;

    // track: <ssrc> <Track>
    std::map<uint32_t, TrackDescriptionPtr> Tracks;

    std::vector<uint32_t> TracksOrder;

    // a=fingerprint:<first:method> <second:value>
    std::pair<std::string, std::string> Fingerprint;

  public:
    bool HasAttribute(const std::string& key) const;

    const std::string& GetAttribute(const std::string& key) const;

    void SetAttribute(const std::string& key, const std::string& value);

    std::string ToString() const;
  
  private:
    // a=<key>:<value>
    std::map<std::string, std::string> attributes_;
};  // class MediaDescription

using MediaDescriptionPtr = std::shared_ptr<MediaDescription>;

inline MediaDescriptionPtr MakeMediaDescription() { return std::make_shared<MediaDescription>(); }


/**
 * @brief Session Description
 * 
 */
class SessionDescription {
  public:
    SessionDescription() = default;

  public:
    // v=0
    int           Version = 0;

    // o=<username> <sess-id> <sses-vesion> <nettype> <addrtype> <unicast-address>
    std::string   UserName;         // <username>
    std::string   SessionId;        // <sess-id>
    std::string   SessionVersion;   // <sess-version>

    // s=<value>
    std::string   SessionName;
    // i=<value>
    std::string   SessionInfo;

    // a=msid-semantic: WMS <value>
    std::string   MediaStreamId;

    // ipv4 / ipv6 : decode from 'o=' / 'c=' / 'a=rtcp:9'
    SdpAddrType   AddrType = SdpAddrType::kIPv4;
    // a=sendrecv / a=sendonly / a=recvonly / a=inactive
    SdpTransType  TransType = SdpTransType::kTransNone;
    // a=setup
    SdpRoleType   RoleType = SdpRoleType::kRoleNone;

    // a=group:BUNDLE <mid> <mid> ...
    std::vector<std::string>  GroupBundle;

    // map <mid> to <MediaDecription>
    std::map<std::string, MediaDescriptionPtr>  Medias;

  public:
    bool HasAttribute(const std::string& key);

    const std::string& GetAttribute(const std::string& key);

    void SetAttribute(const std::string& key, const std::string& value);

    std::string ToString() const;

  private:
    // a=<key>:<value>
    std::map<std::string, std::string> attributes_;
};  // class SessionDescription

using SessionDescriptionPtr = std::shared_ptr<SessionDescription>;

inline SessionDescriptionPtr MakeSessionDescription() { return std::make_shared<SessionDescription>(); }

}  // namespace mini_sdp

#endif  // MINI_SDP_SDP_H_
