/**
 * @file mini_sdp/mini_sdp_impl.h
 * @brief 
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
#ifndef MINI_SDP_MINI_SDP_IMPL_H_
#define MINI_SDP_MINI_SDP_IMPL_H_

#include <string>
#include <vector>
#include "sdp_parser.h"

namespace mini_sdp {


constexpr uint8_t kMiniSdpPacketType = 0xFF;
constexpr uint8_t kMiniSdpAuthLength = 16;

constexpr char kMiniSdpMagic[] = "SDP";
constexpr char kMiniSdpEncryptKey[] = "zDAtJsmOyhljoSu4";
constexpr char kMiniSdpUrlPrefix[] = "webrtc://";
constexpr size_t kMiniSdpUrlMaxLen = 1200;
constexpr size_t kMiniMiniSdpMaxLen = 1400;

enum StreamDirection {
    kStreamDefault = -1,
    kStreamPull = 0,
    kStreamPush = 1,
};

struct MiniSdpHdr {
    uint8_t packet_type                ;
    char magic_word[3]                 ;  //"SDP"

    uint8_t version                    ;

    uint16_t status_code               ;

    uint8_t not_seq_align           : 1;
    uint8_t not_support_aac_fmtp    : 1;
    uint8_t is_string_bundle        : 1;
    uint8_t role                    : 2;
    uint8_t has_candidate           : 1;  // sdp_type为offer时，可无candidate，此时candidate相关置0
    uint8_t encrypt_switch          : 1;  //   encrypt_switch为0表示不加密，1表示通过encrypt_key加密
    uint8_t ip_type                 : 1;         // 0-ipv4，1-ipv6
    
    uint16_t candidate_port;

    uint32_t canditate_ip[4]           ;

    uint16_t seq                       ;  //用于服务端去重


    uint8_t not_imm_send            : 1;
    uint8_t video_audio_data_flag   : 3;  //对应3位表示是否有video，audio，datachannel描述
    uint8_t direction               : 2;
    uint8_t sdp_type                : 2;
} __attribute__((packed));

struct MiniMediaHdr {
    uint32_t ssrc1                  : 32;
    uint32_t ssrc2                  : 32;
    uint8_t media_type              :  2;
    uint8_t codec_num               :  6;  // codec_num表示以下有多少个uint32_t的codec描述
} __attribute__((packed));

struct MiniAacConfig {
    uint8_t     object;
    uint8_t     config_len;
    uint16_t    flag;
    char        config_data[];
} __attribute__((packed));

constexpr uint16_t kMiniAacFlagPs       = 0x1;
constexpr uint16_t kMiniAacFlagSbr      = 0x2;
constexpr uint16_t kMiniAacFlagStereo   = 0x4;
constexpr uint16_t kMiniAacFlagCPresent = 0x8;

struct MiniCodecDesc {
    uint32_t frequency    :  4;
    uint32_t codec        :  4;

    uint32_t mark_a       :  1;
    uint32_t payload_type :  7;

    uint32_t mark_b       :  3;
    uint32_t bfame_enable :  1;
    uint32_t goog_remb    :  1;
    uint32_t transport_cc :  1;
    uint32_t flex_fec     :  1;
    uint32_t nack         :  1;

    uint32_t reversed     :  6;
    uint32_t channels     :  2;
} __attribute__((packed));

struct MiniExtDesc{
    uint16_t id                      :  8;
    uint16_t uri                     :  8;
} __attribute__((packed));

class MiniSdp {
public:
    MiniSdp();

    void HdrNtoh();

    void HdrHton();

    bool containVideo() { return ((mini_sdp_hdr.video_audio_data_flag & 4) >> 2 == 1); }
    bool containAudio() { return ((mini_sdp_hdr.video_audio_data_flag & 2) >> 1 == 1);  }
    bool containData()  { return ((mini_sdp_hdr.video_audio_data_flag & 1) >> 0 == 1); }

public:
    MiniSdpHdr mini_sdp_hdr;

    uint16_t ufrag_len;
    const char* ufrag;

    uint16_t pwd_len;
    const char* pwd;

    uint32_t stream_url_len;
    const char* stream_url;

    uint16_t key_len;
    const char* encrypt_key;

    char auth[16];
};



class MiniSdpPacker {
public:
    /**
     * @brief packer PullReq and original sdp to dst buffer
     * 
     * @param data buffer to store dst data
     * @param sdp need to be packed
     * @param sdp_type offer/answer/none
     * @param stream_url pull stream url
     * @param status_code only sdp_type=none need
     * 
     * @return >0 buffer size 
     * @return =0 pack error
     */
    int PackToDstMem(char *data, size_t len, const std::string &origin_sdp, SdpType sdp_type, 
                     const std::string &stream_url,const std::string &svrsig, uint16_t seq = 0, 
                     int status_code = 0, bool imm_send = false, bool is_support_aac_fmtp = false,
                     StreamDirection is_push = kStreamDefault);

private:
    void copyStr16(uint16_t len, char *str, char *data, uint32_t &offset);

    void copyStr32(uint32_t len, char *str, char *data, uint32_t &offset);

    std::string encrypt_key;

    std::string ip_addr;

    static uint16_t sdp_seq;

}; // class MiniSdpPacker

class MiniSdpLoader {
public:
    /**
     * @brief Parse raw data to origin sdp
     * 
     * @param data recv packet buffer
     * @param data_len recv packet buffer len
     * 
     * @param seq return seq
     * @param dst_sdp return origin sdp
     * @param dst_stream_url return stream_url
     * 
     * @return >0 buffer size 
     * @return =0 parse error
     */
    int ParseToString(char *data, uint32_t data_len, uint16_t &seq, SdpType &sdp_type, 
                      std::string &dst_sdp, std::string &dst_stream_url, std::string &svrsig, 
                      int &status_code, bool &imm_send, bool &is_support_aac_fmtp,
                      StreamDirection &is_push);

private:
    MediaDescriptionPtr parseMedia(char *data, uint32_t &offset, MiniSdpHdr *mini_sdp_hdr);

    void readStr16(std::string &dst, char *data, uint32_t &offset);

    void readStr32(std::string &dst, char *data, uint32_t &offset);

    std::string encrypt_key;

    SdpAddrType addr_type = SdpAddrType::kIPv4;
    std::string ip_addr;
}; // class MiniSdpLoader

struct StopStreamSignalHeader {
    uint8_t     pack_type;
    char        magic_word[3];
    uint8_t     version;
    uint16_t    status;
    uint16_t    seq;
    uint16_t    svrsig_len;
} __attribute__((packed));

}  // namespace mini_sdp

#endif  // MINI_SDP_MINI_SDP_IMPL_H_
