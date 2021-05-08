/**
 * @file mini_sdp/mini_sdp_impl.cc
 * @brief 
 * @version 0.1
 * @date 2021-01-11
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
#include "mini_sdp_impl.h"
#include <cstring>
#include <limits>
#include "util.h"

namespace mini_sdp {

uint16_t MiniSdpPacker::sdp_seq = 0;

static std::unordered_map<std::string, uint8_t> mini_sdp_codec_name_map = {
    {kSdpCodecOpus, 0},
    {kSdpCodecLatm, 1},
    {kSdpCodecAdts, 2},
    {kSdpCodecH264, 3},
    {kSdpCodecH265, 4},
    {kSdpCodecFlexFec, 5},
};

static std::vector<std::string> mini_sdp_codec_name_vec = {
    kSdpCodecOpus, kSdpCodecLatm, kSdpCodecAdts, kSdpCodecH264, kSdpCodecH265, kSdpCodecFlexFec
};

static std::unordered_map<uint32_t, uint8_t> mini_sdp_frequency_map = {
    {96000, 0},  {88200, 1}, {64000, 2}, {48000, 3}, {44100, 4},
    {32000, 5},  {24000, 6}, {22050, 7}, {16000, 8}, {12000, 9},
    {11025, 10}, {8000, 11}, {7350, 12}, {0, 13},    {90000, 15},
};

static std::vector<uint32_t> mini_sdp_frequency_vec = {
    96000, 88200, 64000, 48000,
    44100, 32000, 24000, 22050,
    16000, 12000, 11025,  8000,
    7350,     0,     0,  90000,
};

static std::unordered_map<uint8_t, uint8_t>  mini_sdp_media_type_map = {
    {uint8_t(SdpMediaType::kVideo), 0x1 << 2},
    {uint8_t(SdpMediaType::kAudio), 0x1 << 1},
    {uint8_t(SdpMediaType::kData),  0x1     },
};

static std::unordered_map<uint8_t, uint8_t>  mini_sdp_trans_type_map = {
    {uint8_t(SdpTransType::kSendOnly), 0},
    {uint8_t(SdpTransType::kRecvOnly), 1},
    {uint8_t(SdpTransType::kSendRecv), 2},
};

static std::vector<uint8_t> mini_sdp_trans_type_vec = {
    uint8_t(SdpTransType::kSendOnly), 
    uint8_t(SdpTransType::kRecvOnly), 
    uint8_t(SdpTransType::kSendRecv),
};

static std::unordered_map<uint8_t, uint8_t>  mini_sdp_role_type_map = {
    {uint8_t(SdpRoleType::kActpass), 0},
    {uint8_t(SdpRoleType::kActive), 1},
    {uint8_t(SdpRoleType::kPassive), 2},
};

static std::vector<uint8_t> mini_sdp_role_type_vec = {
    uint8_t(SdpRoleType::kActpass),
    uint8_t(SdpRoleType::kActive),
    uint8_t(SdpRoleType::kPassive),
};

static std::unordered_map<std::string, uint8_t> mini_sdp_ext_map = {
    {std::string(kSdpExtAbsSendTime), 0}, {std::string(kSdpExtPayloutDelay), 1},
    {std::string(kSdpExtTransportCc), 2}, {std::string(kSdpExtMetaData01), 3},
    {std::string(kSdpExtMetaData02), 4},  {std::string(kSdpExtMetaData03), 5},
    {std::string(kSdpExtDts), 6},         {std::string(kSdpExtCts), 7},
    {std::string(kSdpExtVideoFrameType), 8}
};

static std::vector<std::string> mini_sdp_ext_vec = {
    kSdpExtAbsSendTime, kSdpExtPayloutDelay, kSdpExtTransportCc,
    kSdpExtMetaData01,  kSdpExtMetaData02,   kSdpExtMetaData03,
    kSdpExtDts,         kSdpExtCts,          kSdpExtVideoFrameType
};

MiniSdp::MiniSdp() {
    mini_sdp_hdr.packet_type = kMiniSdpPacketType;
    memcpy(mini_sdp_hdr.magic_word, kMiniSdpMagic, 3 * sizeof(char));
    mini_sdp_hdr.version = 0;
    mini_sdp_hdr.ip_type = 0;
    mini_sdp_hdr.encrypt_switch = 0;
    mini_sdp_hdr.has_candidate = 0;
    mini_sdp_hdr.role = 0;
    mini_sdp_hdr.is_string_bundle = 0;
    mini_sdp_hdr.candidate_port = 0;
    memset(mini_sdp_hdr.canditate_ip, 0, 4 * sizeof(uint32_t));
    mini_sdp_hdr.seq = 0;
    mini_sdp_hdr.sdp_type = 0;
    mini_sdp_hdr.direction = 0;
    mini_sdp_hdr.video_audio_data_flag = 0;
    mini_sdp_hdr.not_imm_send = 1u;
    ufrag_len = 0;
    ufrag = nullptr;
    pwd_len = 0;
    pwd = nullptr;
    stream_url_len = 0;
    stream_url = nullptr;
    key_len = 0;
    encrypt_key = nullptr;
    memset(auth, 0, 16 * sizeof(char));
}

void MiniSdp::HdrHton() {
    mini_sdp_hdr.candidate_port = htons(mini_sdp_hdr.candidate_port);
    for (int i = 0; i < 4; i++) {
        mini_sdp_hdr.canditate_ip[0] = htonl(mini_sdp_hdr.canditate_ip[0]);
    }
    mini_sdp_hdr.seq = htons(mini_sdp_hdr.seq);
    mini_sdp_hdr.status_code = htons(mini_sdp_hdr.status_code);
}

void MiniSdp::HdrNtoh() {
    mini_sdp_hdr.candidate_port = ntohs(mini_sdp_hdr.candidate_port);
    for (int i = 0; i < 4; i++) {
        mini_sdp_hdr.canditate_ip[0] = ntohs(mini_sdp_hdr.canditate_ip[0]);
    }
    mini_sdp_hdr.seq = ntohs(mini_sdp_hdr.seq);
    mini_sdp_hdr.status_code = ntohs(mini_sdp_hdr.status_code);
}

// pack to mini sdp
int MiniSdpPacker::PackToDstMem(char *data, size_t len, const std::string &origin_sdp, SdpType sdp_type,
                                const std::string &stream_url, const std::string &svrsig, uint16_t seq, 
                                int status_code, bool imm_send, bool is_support_aac_fmtp,
                                StreamDirection is_push) {
    MiniSdp mini_sdp;
    uint32_t offset = 0;

    std::string mini_stream_url = std::string(stream_url.begin() + 9, stream_url.end());
    mini_sdp.stream_url_len = mini_stream_url.size();
    mini_sdp.stream_url = mini_stream_url.c_str();

    // for error code
    if (sdp_type == SdpType::kSdpNone) {
        mini_sdp.mini_sdp_hdr.status_code = status_code;
        mini_sdp.mini_sdp_hdr.seq = seq;
        mini_sdp.HdrHton();
        if (offset + sizeof(MiniSdpHdr) > len) return offset + sizeof(MiniSdpHdr);
        memcpy(data + offset, &(mini_sdp.mini_sdp_hdr), sizeof(MiniSdpHdr));
        offset += sizeof(MiniSdpHdr);
        std::string empty_str;
        copyStr16(empty_str.size(), const_cast<char *>(empty_str.c_str()), data, offset);
        copyStr16(empty_str.size(), const_cast<char *>(empty_str.c_str()), data, offset);
        copyStr32(mini_sdp.stream_url_len, const_cast<char *>(mini_sdp.stream_url), data, offset);
        copyStr16(empty_str.size(), const_cast<char *>(empty_str.c_str()), data, offset);
        copyStr16(empty_str.size(), const_cast<char *>(empty_str.c_str()), data, offset);
        if (offset + 16 > len) return offset + 16;
        memcpy(data+offset, mini_sdp.auth, 16);
        offset += 16;
        return offset;
    }

    SdpParser sdp_parser(origin_sdp.c_str(), origin_sdp.size());
    if (!sdp_parser.Parse()) {
        return 0;
    }

    SessionDescriptionPtr sdp_info = sdp_parser.GetSessionDescription();

    mini_sdp.mini_sdp_hdr.version = (sdp_info->Version <= 0) ? 0 : sdp_info->Version;
    mini_sdp.mini_sdp_hdr.ip_type = uint8_t(sdp_info->AddrType);
    mini_sdp.mini_sdp_hdr.status_code = status_code;
    mini_sdp.mini_sdp_hdr.seq = seq;
    mini_sdp.mini_sdp_hdr.sdp_type = uint8_t(sdp_type);
    mini_sdp.mini_sdp_hdr.not_imm_send = !imm_send;
    mini_sdp.mini_sdp_hdr.not_support_aac_fmtp = !is_support_aac_fmtp;
    mini_sdp.mini_sdp_hdr.not_seq_align = !(sdp_info->SessionId == "1");

    for (auto media_info_pair : sdp_info->Medias) {
        auto media_info = media_info_pair.second;
        if (media_info->Protos == kSdpMediaProtoEncryptDefault) {
            mini_sdp.mini_sdp_hdr.encrypt_switch = 1;
        }
        if (media_info_pair.first == "video" || media_info_pair.first == "audio") {
            mini_sdp.mini_sdp_hdr.is_string_bundle = 1;
        }

        if (!media_info->Candidate.first.empty() && media_info->Candidate.second != 0) {
            mini_sdp.mini_sdp_hdr.has_candidate = 1;
            if (sdp_info->AddrType == SdpAddrType::kIPv4) {
                str2ipv4(media_info->Candidate.first.c_str(),
                         static_cast<void *>(mini_sdp.mini_sdp_hdr.canditate_ip));
            } else {
                str2ipv6(media_info->Candidate.first.c_str(),
                         static_cast<void *>(mini_sdp.mini_sdp_hdr.canditate_ip));
            }
            mini_sdp.mini_sdp_hdr.candidate_port = media_info->Candidate.second;
        }
        
        mini_sdp.mini_sdp_hdr.video_audio_data_flag |= mini_sdp_media_type_map[uint8_t(media_info->MediaType)];

        mini_sdp.mini_sdp_hdr.direction = mini_sdp_trans_type_map[uint8_t(media_info->TransType)];
        mini_sdp.mini_sdp_hdr.role =  mini_sdp_role_type_map[uint8_t(media_info->RoleType)];

        mini_sdp.ufrag_len = media_info->IceUfrag.size();
        mini_sdp.ufrag = media_info->IceUfrag.c_str();
        mini_sdp.pwd_len = media_info->IcePwd.size();
        mini_sdp.pwd = media_info->IcePwd.c_str();

        std::string fingerprint = media_info->Fingerprint.first + " " + media_info->Fingerprint.second;
        if (fingerprint.size() > 1) encrypt_key = fingerprint;
    }  // sdp_hdr
    
    mini_sdp.key_len = encrypt_key.size();
    mini_sdp.encrypt_key = encrypt_key.c_str();

    mini_sdp.HdrHton();
    if (offset + sizeof(MiniSdpHdr) > len) return offset + sizeof(MiniSdpHdr);
    memcpy(data + offset, &(mini_sdp.mini_sdp_hdr), sizeof(MiniSdpHdr));
    offset += sizeof(MiniSdpHdr);
    

    for (auto media_info_pair : sdp_info->Medias) {
        auto media_info = media_info_pair.second;

        MiniMediaHdr mini_media_hdr;
        mini_media_hdr.media_type = uint8_t(media_info->MediaType);
        mini_media_hdr.ssrc1 = 0;
        mini_media_hdr.ssrc2 = 0;
        int i = 0;
        for (auto it = media_info->TracksOrder.begin(); it != media_info->TracksOrder.end(); it++) {
            if (i == 0)
                mini_media_hdr.ssrc1 = htonl(*it);
            else if (i == 1)
                mini_media_hdr.ssrc2 = htonl(*it);
            else
                break;
            i++;
        }
        char *media_hdr_pos = data + offset;
        mini_media_hdr.codec_num = uint8_t(media_info->Codecs.size());
        offset += sizeof(MiniMediaHdr);

        for (auto it = media_info->Codecs.begin(); it != media_info->Codecs.end(); it++) {
            if (!mini_sdp_codec_name_map.count(it->second->Name) ||
                !mini_sdp_frequency_map.count(it->second->SampleRate)) {
                mini_media_hdr.codec_num--;
                continue;
            }
            MiniCodecDesc mini_codec_desc;
            mini_codec_desc.mark_a = 0;
            mini_codec_desc.mark_b = 0;
            mini_codec_desc.reversed = 0;
            mini_codec_desc.codec = mini_sdp_codec_name_map[it->second->Name];
            mini_codec_desc.payload_type = it->second->Format;
            mini_codec_desc.channels = it->second->Channels;
            mini_codec_desc.frequency = mini_sdp_frequency_map[it->second->SampleRate];
            mini_codec_desc.nack = (it->second->Feedbacks.count(kSdpCodecNack)) ? 1u : 0u;
            mini_codec_desc.flex_fec = it->second->Name.compare(kSdpCodecFlexFec) == 0 ? 1u: 0u;
            mini_codec_desc.transport_cc = (it->second->Feedbacks.count(kSdpCodecTransportCc)) ? 1u : 0u;
            mini_codec_desc.goog_remb = (it->second->Feedbacks.count(kSdpCodecGoogleRemb)) ? 1u : 0u;
            mini_codec_desc.bfame_enable = bool(std::stol(it->second->GetFormatParam(kSdpCodecBFrameEnabled, "0"))
                                            || std::stol(it->second->GetFormatParam(kSdpCodecBFrameEnabled2, "0")));
            if (offset + sizeof(MiniCodecDesc) > len) return offset + sizeof(MiniCodecDesc);
            memcpy(data + offset, &mini_codec_desc, sizeof(MiniCodecDesc));
            offset += sizeof(MiniCodecDesc);

            if (is_support_aac_fmtp && (it->second->Name == kSdpCodecLatm || it->second->Name == kSdpCodecAdts)) {
                auto config = it->second->GetFormatParam("config", "");
                auto aac_config = std::unique_ptr<MiniAacConfig>((MiniAacConfig*) new char[sizeof(MiniAacConfig) + config.size()]);
                aac_config->object = std::stoul(it->second->GetFormatParam("object", "0"));
                aac_config->flag |= std::stoul(it->second->GetFormatParam("PS-enabled", "0")) ? kMiniAacFlagPs : 0;
                aac_config->flag |= std::stoul(it->second->GetFormatParam("SBR-enabled", "0")) ? kMiniAacFlagSbr : 0;
                aac_config->flag |= std::stoul(it->second->GetFormatParam("stereo", "0")) ? kMiniAacFlagStereo : 0;
                aac_config->flag |= std::stoul(it->second->GetFormatParam("cpresent", "0")) ? kMiniAacFlagCPresent : 0;
                aac_config->config_len = config.size();

                if (offset + sizeof(MiniAacConfig) + config.size() > len) return offset + sizeof(MiniAacConfig) + config.size();
                memcpy(data + offset, aac_config.get(), sizeof(MiniAacConfig));
                offset += sizeof(MiniAacConfig);
                if (!config.empty()) {
                    memcpy(data + offset, config.c_str(), config.size());
                    offset += config.size();
                }
            }
        }
        if (offset + sizeof(MiniMediaHdr) > len) return offset + sizeof(MiniMediaHdr);         
        memcpy(media_hdr_pos, &mini_media_hdr, sizeof(MiniMediaHdr));

        uint8_t ext_num = media_info->ExtMap.size();
        char *ext_pos = data + offset;
        offset += sizeof(uint8_t);
        for(auto it = media_info->ExtMap.begin(); it != media_info->ExtMap.end(); it++) {
            MiniExtDesc mini_ext_desc;
            Trim(it->second);
            if (!mini_sdp_ext_map.count(it->second)) {
                ext_num--;
                continue;
            }
            mini_ext_desc.id = it->first;
            mini_ext_desc.uri = mini_sdp_ext_map[it->second];
            if (offset + sizeof(MiniExtDesc) > len) return offset + sizeof(MiniExtDesc);         
            memcpy(data + offset, &mini_ext_desc, sizeof(MiniExtDesc));
            offset += sizeof(MiniExtDesc);
        }
        if (offset + sizeof(uint8_t) > len) return offset + sizeof(uint8_t);         
        memcpy(ext_pos, &ext_num, sizeof(uint8_t));

    }  // media descs

    size_t mem_len = offset + mini_sdp.ufrag_len + mini_sdp.pwd_len + mini_sdp.stream_url_len + mini_sdp.key_len + 16 + 1;
    if (mem_len > len) return mem_len;         
    copyStr16(mini_sdp.ufrag_len, const_cast<char *>(mini_sdp.ufrag), data, offset);
    copyStr16(mini_sdp.pwd_len, const_cast<char *>(mini_sdp.pwd), data, offset);
    copyStr32(mini_sdp.stream_url_len, const_cast<char *>(mini_sdp.stream_url), data, offset);
    copyStr16(mini_sdp.key_len, const_cast<char *>(mini_sdp.encrypt_key), data, offset);
    copyStr16(svrsig.size(), const_cast<char *>(svrsig.c_str()), data, offset);
    memcpy(data+offset, mini_sdp.auth, 16);
    offset += 16;

    if (is_push == kStreamPull || is_push == kStreamPush) {
        uint8_t extern_byte = 0;
        extern_byte |= (is_push ? 1u : 0u) << 0u;
        memcpy(data+offset, &extern_byte, 1);
        offset += 1;
    }

    return offset;
}

void MiniSdpPacker::copyStr16(uint16_t len, char *str, char *data, uint32_t &offset) {
    uint16_t nlen = htons(len);
    memcpy(data + offset, &(nlen), sizeof(uint16_t));
    offset += sizeof(uint16_t);
    memcpy(data + offset, str, len);
    offset += len;
}

void MiniSdpPacker::copyStr32(uint32_t len, char *str, char *data, uint32_t &offset) {
    uint32_t nlen = htonl(len);
    memcpy(data + offset, &(nlen), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(data + offset, str, len);
    offset += len;
}

int MiniSdpLoader::ParseToString(char *data, uint32_t data_len, uint16_t &seq, SdpType &sdp_type, 
                                 std::string &dst_sdp, std::string &dst_stream_url, std::string &svrsig, 
                                 int &status_code, bool &imm_send, bool &is_support_aac_fmtp,
                                 StreamDirection &is_push) {
    uint32_t offset = 0;
    SessionDescriptionPtr sdp_info = MakeSessionDescription();

    MiniSdp mini_sdp;
    MiniSdpHdr *mini_sdp_hdr = reinterpret_cast<MiniSdpHdr*>(data + offset);
    offset += sizeof(MiniSdpHdr);

    mini_sdp.mini_sdp_hdr = *mini_sdp_hdr;
    sdp_type = (SdpType)mini_sdp_hdr->sdp_type;
    
    sdp_info->Version = mini_sdp_hdr->version;
    addr_type = SdpAddrType(mini_sdp_hdr->ip_type);
    sdp_info->AddrType = addr_type;
    if (mini_sdp_hdr->direction >= mini_sdp_trans_type_vec.size()) {
        sdp_info->TransType = SdpTransType::kSendRecv;
    } else {
        sdp_info->TransType = SdpTransType(mini_sdp_trans_type_vec[mini_sdp_hdr->direction]);
    }
    if (mini_sdp_hdr->role >= mini_sdp_role_type_vec.size()) {
        sdp_info->RoleType = SdpRoleType::kActpass;
    } else {
        sdp_info->RoleType = SdpRoleType(mini_sdp_role_type_vec[mini_sdp_hdr->role]);
    }
    if (!mini_sdp_hdr->not_seq_align) {
        sdp_info->SessionId = "1";
    }
    uint32_t ipv6[4] = {0};
    for (int i=0; i<4; i++) {
        ipv6[i] = (mini_sdp_hdr->canditate_ip[i]);
    }
    ip_addr = (sdp_info->AddrType == SdpAddrType::kIPv4)
                  ? ip2strv4((mini_sdp_hdr->canditate_ip[0]))
                  : ip2strv6(reinterpret_cast<unsigned char *>(ipv6));
    std::vector<MediaDescriptionPtr> medias;
    if (mini_sdp.containVideo()) {
        medias.push_back(parseMedia(data, offset, mini_sdp_hdr));
    }
    if (mini_sdp.containAudio()) {
        medias.push_back(parseMedia(data, offset, mini_sdp_hdr));
    }
    if (mini_sdp.containData()) {
        medias.push_back(parseMedia(data, offset, mini_sdp_hdr));
    }

    std::string ice_ufrag;
    std::string ice_pwd;    
    std::string stream_url;    
    std::string encrypt_key;
    readStr16(ice_ufrag, data, offset);
    readStr16(ice_pwd, data, offset);
    readStr32(stream_url, data, offset);
    readStr16(encrypt_key, data, offset);
    readStr16(svrsig, data, offset);
    //auth
    offset += 16;

    is_push = kStreamDefault;
    if (offset < data_len) {
        uint8_t extern_byte = 0;
        extern_byte = *reinterpret_cast<uint8_t*>(data + offset);
        offset += 1;
        if (extern_byte & 1u) {
            is_push = kStreamPush;
        } else {
            is_push = kStreamPull;
        }
    }


    uint32_t cur_media_id = 0;
    for (auto media: medias) {
        media->Protos = (mini_sdp_hdr->encrypt_switch ? kSdpMediaProtoEncryptDefault : kSdpMediaProtoNotEncryptDefault);
        media->IceUfrag = ice_ufrag;
        media->IcePwd = ice_pwd;
        media->RoleType = sdp_info->RoleType;
        for (auto track: media->Tracks) {
            track.second->SetAttribute("cname", media->IceUfrag);
            std::string codec_name = track.second->GetAttribute("label");
            if (!codec_name.empty()) {
                track.second->SetAttribute("msid", media->IceUfrag + " " + media->IceUfrag + "_" + codec_name);
                track.second->SetAttribute("mslabel", media->IceUfrag);
                track.second->SetAttribute("label", media->IceUfrag + "_" + codec_name);
            }
        }
        std::string mid = media->MediaId;
        if (mid.empty()) {
            if (mini_sdp_hdr->is_string_bundle) {
                if (media->MediaType == SdpMediaType::kVideo) {
                    mid = "video";
                } else if (media->MediaType == SdpMediaType::kAudio) {
                    mid = "audio";
                } else {
                    mid = "data";
                }
            } else {
                mid = std::to_string(cur_media_id++);
            }
        } 
        media->MediaId = mid;
        sdp_info->GroupBundle.push_back(mid);
        sdp_info->Medias.emplace(mid, media);

        auto pos = encrypt_key.find(' ');
        if (pos != std::string::npos) {
            media->Fingerprint.first = encrypt_key.substr(0, pos);
            media->Fingerprint.second = encrypt_key.substr(pos + 1);
        }
    }
    //TODO auth    
    dst_stream_url = kMiniSdpUrlPrefix + stream_url;
    dst_sdp = sdp_info->ToString();
    seq = ntohs(mini_sdp_hdr->seq);
    status_code = ntohs(mini_sdp_hdr->status_code);
    imm_send = !mini_sdp_hdr->not_imm_send;
    is_support_aac_fmtp = !mini_sdp_hdr->not_support_aac_fmtp;
    svrsig = ip_addr + ":" + ice_ufrag + ":" + svrsig; 
    return offset;
}

MediaDescriptionPtr MiniSdpLoader::parseMedia(char *data, uint32_t &offset, MiniSdpHdr *mini_sdp_hdr) {
    MiniMediaHdr *media_hdr = reinterpret_cast<MiniMediaHdr *>(data + offset);
    offset += sizeof(MiniMediaHdr);
    MediaDescriptionPtr media_info = MakeMediaDescription();
    media_info->MediaType = SdpMediaType(media_hdr->media_type);
    std::string codec_name;
    media_info->AddrType = addr_type;
    media_info->TransType = SdpTransType(mini_sdp_trans_type_vec[mini_sdp_hdr->direction]);
    if (!ip_addr.empty() && ip_addr != "0.0.0.0") {
        media_info->Candidate.first = ip_addr;
        media_info->Candidate.second = ntohs(mini_sdp_hdr->candidate_port);
    }

    for (int i = 0; i < media_hdr->codec_num; i++) {
        MiniCodecDesc *codec_desc =
            reinterpret_cast<MiniCodecDesc *>(data + offset);
        offset += sizeof(MiniCodecDesc);
        MiniAacConfig *aac_config = nullptr;
        if (!mini_sdp_hdr->not_support_aac_fmtp && (codec_desc->codec == 1 || codec_desc->codec == 2)) {
            // is LATM || ADTS
            aac_config = reinterpret_cast<MiniAacConfig*>(data + offset);
            offset += sizeof(MiniAacConfig) + aac_config->config_len;
        }
        CodecDescriptionPtr code_info = MakeCodecDescription();
        if (codec_desc->codec >= mini_sdp_codec_name_vec.size()) {
            continue;
        }
        code_info->Name = mini_sdp_codec_name_vec[codec_desc->codec];
        codec_name = code_info->Name;
        code_info->Format = codec_desc->payload_type;
        code_info->Channels = codec_desc->channels;
        if (codec_desc->frequency >= mini_sdp_frequency_vec.size()) {
            continue;
        }
        code_info->SampleRate = mini_sdp_frequency_vec[codec_desc->frequency];
        if (codec_desc->nack) {
            code_info->Feedbacks.emplace(kSdpCodecNack);
        }
        if (codec_desc->flex_fec) {
            //code_info->Feedbacks.emplace(kSdpCodecFlexFec);
        }
        if (codec_desc->transport_cc) {
            code_info->Feedbacks.emplace(kSdpCodecTransportCc);
        }
        if (codec_desc->goog_remb) {
            code_info->Feedbacks.emplace(kSdpCodecGoogleRemb);
        }
        if (codec_desc->bfame_enable) {
            code_info->FormatParams.emplace(kSdpCodecBFrameEnabled, "1");
        }
        if (media_info->MediaType == SdpMediaType::kVideo) {
            code_info->FormatParams.emplace("level-asymmetry-allowed","1");
            code_info->FormatParams.emplace("packetization-mode","1");
            code_info->FormatParams.emplace("profile-level-id","42e01f");
        }
        if (media_info->MediaType == SdpMediaType::kAudio) {
            if (aac_config) {
                if (aac_config->object) code_info->FormatParams.emplace("object", std::to_string((int)aac_config->object));
                if (aac_config->flag & kMiniAacFlagPs) code_info->FormatParams.emplace("PS-enabled", "1");
                if (aac_config->flag & kMiniAacFlagSbr) code_info->FormatParams.emplace("SBR-enabled", "1");
                if (aac_config->flag & kMiniAacFlagStereo) code_info->FormatParams.emplace("stereo", "1");
                if (aac_config->flag & kMiniAacFlagStereo) code_info->FormatParams.emplace("cpresent", "1");
                if (aac_config->config_len > 0) code_info->FormatParams.emplace("config", std::string(aac_config->config_data, aac_config->config_len));
            } else if (!codec_desc->flex_fec){
                code_info->FormatParams.emplace("stereo","1");
            }
        }
        media_info->Codecs.emplace(code_info->Format, code_info);
    }
    uint8_t *ext_num = reinterpret_cast<uint8_t *>(data + offset);
    offset += sizeof(uint8_t);
    for (int i = 0; i < *ext_num; i++) {
        MiniExtDesc *ext_desc = reinterpret_cast<MiniExtDesc *>(data + offset);
        offset += sizeof(MiniExtDesc);
        uint8_t ext_id = ext_desc->id;
        if (ext_desc->uri >= mini_sdp_ext_vec.size()) {
            continue;
        }
        media_info->ExtMap.emplace(ext_id, mini_sdp_ext_vec[ext_desc->uri]);
    }
    //todo add stream_id to track
    TrackDescriptionPtr track_info = MakeTrackDescription();
    track_info->Ssrc = ntohl(media_hdr->ssrc1);
    if (track_info->Ssrc) {
        // track_info->SetAttribute("msid", "- " + codec_name);
        // track_info->SetAttribute("mslabel", "-");
        track_info->SetAttribute("label", codec_name);
        media_info->Tracks.emplace(ntohl(media_hdr->ssrc1), track_info);
        media_info->TracksOrder.push_back(ntohl(media_hdr->ssrc1));
    }
    TrackDescriptionPtr track_info2 = MakeTrackDescription();
    track_info2->Ssrc = ntohl(media_hdr->ssrc2);
    if (track_info2->Ssrc) {
        // track_info2->SetAttribute("msid", "- " + codec_name);
        // track_info2->SetAttribute("mslabel", "-");
        track_info2->SetAttribute("label", codec_name);
        media_info->Tracks.emplace(ntohl(media_hdr->ssrc2), track_info2);
        media_info->TracksOrder.push_back(ntohl(media_hdr->ssrc2));
    }

    return media_info;
}

void MiniSdpLoader::readStr16(std::string &dst, char *data,
                                 uint32_t &offset) {
    uint16_t *nlen = reinterpret_cast<uint16_t *>(data + offset);
    uint16_t len = ntohs(*nlen);
    offset += sizeof(uint16_t);
    dst.assign(data + offset, len);
    offset += len;
}

void MiniSdpLoader::readStr32(std::string &dst, char *data,
                                 uint32_t &offset) {
    uint32_t *nlen = reinterpret_cast<uint32_t *>(data + offset);
    uint32_t len = ntohl(*nlen);
    offset += sizeof(uint32_t);
    dst.assign(data + offset, len);
    offset += len;
}

}  // namespace mini_sdp
