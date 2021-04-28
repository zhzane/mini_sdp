#include <iostream>
#include "mini_sdp.h"

using namespace std;
using namespace mini_sdp;

string origin_sdp =
    "v=0\r\no=- 1 0 IN IP4 127.0.0.1\r\n"
    "s=webrtc_core\r\nt=0 0\r\na=group:BUNDLE 0 1\r\n"
    "a=msid-semantic: WMS 0_xxxx_d71956d9cc93e4a467b11e06fdaf039a\r\nm=audio 1 "
    "UDP/TLS/RTP/SAVPF 111\r\nc=IN IP4 0.0.0.0\r\na=rtcp:1 IN IP4 "
    "0.0.0.0\r\na=candidate:foundation 1 udp 100 127.0.0.1 8000 typ srflx "
    "raddr 127.0.0.1 rport 8000 generation "
    "0\r\na=ice-ufrag:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a_"
    "de71a64097d807c3\r\na=ice-pwd:be8577c0a03b0d3ffa4e5235\r\na=fingerprint:"
    "sha-256 "
    "8A:BD:A6:61:75:AF:31:4C:02:81:2A:FA:12:92:4C:48:7B:9F:23:DD:BF:3D:51:30:"
    "E7:59:5C:9B:17:3D:92:34\r\na=setup:passive\r\na=sendrecv\r\na=extmap:9 "
    "http://www.webrtc.org/experiments/rtp-hdrext/"
    "decoding-timestamp\r\na=extmap:10 "
    "http://www.webrtc.org/experiments/rtp-hdrext/"
    "video-composition-time\r\na=extmap:21 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-01\r\na=extmap:22 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-02\r\na=extmap:23 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-03\r\na=extmap:2 "
    "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:3 "
    "http://www.ietf.org/id/"
    "draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=mid:0\r\na=rtcp-"
    "mux\r\na=rtpmap:111 MP4A-ADTS/48000/2\r\na=rtcp-fb:111 nack\r\na=rtcp-fb:111 "
    "transport-cc\r\na=fmtp:111 "
    "minptime=10;stereo=1;useinbandfec=1;PS-enabled=1;SBR-enabled=1;object=5;cpresent=1;config=4002420adca1fe0\r\n"
    "a=rtpmap:124 flexfec-03/48000/2\r\n"
    "a=rtpmap:125 flexfec-03/44100/2\r\n"
    "a=ssrc-group:FEC-FR 27172315 50331648\r\n"
    "a=ssrc:27172315 "
    "cname:webrtccore\r\na=ssrc:27172315 "
    "msid:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a opus\r\na=ssrc:27172315 "
    "mslabel:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a\r\na=ssrc:27172315 "
    "label:opus\r\n"
    "a=ssrc:50331648 cname:webrtccore\r\n"
    "a=ssrc:50331648 msid:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a opus\r\n"
    "a=ssrc:50331648 mslabel:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a\r\n"
    "a=ssrc:50331648 label:opus\r\n"
    "m=video 1 UDP/TLS/RTP/SAVPF 102 108 123 124 125 127\r\nc=IN "
    "IP4 0.0.0.0\r\na=rtcp:1 IN IP4 0.0.0.0\r\na=candidate:foundation 1 udp "
    "100 127.0.0.1 8000 typ srflx raddr 127.0.0.1 rport 8000 "
    "generation "
    "0\r\na=ice-ufrag:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a_"
    "de71a64097d807c3\r\na=ice-pwd:be8577c0a03b0d3ffa4e5235\r\na=extmap:9 "
    "http://www.webrtc.org/experiments/rtp-hdrext/"
    "decoding-timestamp\r\na=extmap:10 "
    "http://www.webrtc.org/experiments/rtp-hdrext/"
    "video-composition-time\r\na=extmap:21 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-01\r\na=extmap:22 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-02\r\na=extmap:23 "
    "http://www.webrtc.org/experiments/rtp-hdrext/meta-data-03\r\na=extmap:14 "
    "urn:ietf:params:rtp-hdrext:toffset\r\na=extmap:2 "
    "http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=extmap:13 "
    "urn:3gpp:video-orientation\r\na=extmap:3 "
    "http://www.ietf.org/id/"
    "draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=extmap:12 "
    "http://www.webrtc.org/experiments/rtp-hdrext/"
    "playout-delay\r\na=extmap:30 "
    "http://www.webrtc.org/experiments/rtp-hdrext/video-frame-type\r\na=fingerprint:sha-256 "
    "8A:BD:A6:61:75:AF:31:4C:02:81:2A:FA:12:92:4C:48:7B:9F:23:DD:BF:3D:51:30:"
    "E7:59:5C:9B:17:3D:92:34\r\na=setup:passive\r\na=sendrecv\r\na=mid:1\r\na="
    "rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:102 H264/90000\r\na=rtcp-fb:102 ccm "
    "fir\r\na=rtcp-fb:102 goog-remb\r\na=rtcp-fb:102 nack\r\na=rtcp-fb:102 "
    "nack pli\r\na=rtcp-fb:102 transport-cc\r\na=fmtp:102 "
    "bframe-enabled=1;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id="
    "42001f\r\na=rtpmap:108 H264/90000\r\na=rtcp-fb:108 ccm "
    "fir\r\na=rtcp-fb:108 goog-remb\r\na=rtcp-fb:108 nack\r\na=rtcp-fb:108 "
    "nack pli\r\na=rtcp-fb:108 transport-cc\r\na=fmtp:108 "
    "BFrame-enabled=1;level-asymmetry-allowed=1;packetization-mode=0;profile-level-id="
    "42e01f\r\na=rtpmap:123 H264/90000\r\na=rtcp-fb:123 ccm "
    "fir\r\na=rtcp-fb:123 goog-remb\r\na=rtcp-fb:123 nack\r\na=rtcp-fb:123 "
    "nack pli\r\na=rtcp-fb:123 transport-cc\r\na=fmtp:123 "
    "level-asymmetry-allowed=1;packetization-mode=1;profile-level-id="
    "640032\r\na=rtpmap:124 H264/90000\r\na=rtcp-fb:124 ccm "
    "fir\r\na=rtcp-fb:124 goog-remb\r\na=rtcp-fb:124 nack\r\na=rtcp-fb:124 "
    "nack pli\r\na=rtcp-fb:124 transport-cc\r\na=fmtp:124 "
    "level-asymmetry-allowed=1;packetization-mode=1;profile-level-id="
    "4d0032\r\na=rtpmap:125 H264/90000\r\na=rtcp-fb:125 ccm "
    "fir\r\na=rtcp-fb:125 goog-remb\r\na=rtcp-fb:125 nack\r\na=rtcp-fb:125 "
    "nack pli\r\na=rtcp-fb:125 transport-cc\r\na=fmtp:125 "
    "level-asymmetry-allowed=1;packetization-mode=1;profile-level-id="
    "42e01f\r\na=rtpmap:127 H264/90000\r\na=rtcp-fb:127 ccm "
    "fir\r\na=rtcp-fb:127 goog-remb\r\na=rtcp-fb:127 nack\r\na=rtcp-fb:127 "
    "nack pli\r\na=rtcp-fb:127 transport-cc\r\na=fmtp:127 "
    "level-asymmetry-allowed=1;packetization-mode=0;profile-level-id="
    "42001f\r\na=ssrc:10395099 cname:webrtccore\r\na=ssrc:10395099 "
    "msid:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a h264\r\na=ssrc:10395099 "
    "mslabel:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a\r\na=ssrc:10395099 "
    "label:h264\r\n";

SdpType sdp_type = SdpType::kAnswer;
string url =
    "webrtc://domain/live/xxxx_d71956d9cc93e4a467b11e06fdaf039a";

char buffer[1400];

int main() {
    cout << "test sdp" << endl;

    OriginSdpAttr attr;
    attr.origin_sdp = origin_sdp;
    attr.sdp_type = sdp_type;
    attr.stream_url = url;
    attr.seq = 0;
    attr.status_code = 0;
    attr.svrsig = "1h8s";
    attr.is_imm_send = true;
    attr.is_support_aac_fmtp = true;
    attr.is_push = false;
    int ret = ParseOriginSdpToMiniSdp(attr, buffer, 1400);
    cout << "pack ret:" << ret << endl;

    OriginSdpAttr attr2;
    ret = LoadMiniSdpToOriginSdp(buffer, ret, attr2);
    cout << "parse ret:" << ret << endl;
    cout << "tyep: " << (uint32_t)attr2.sdp_type << endl;
    cout << "parse sdp" << endl;
    cout << attr2.origin_sdp << endl;

    cout << "parse url: ";
    cout << attr2.stream_url << endl;

    cout << "seq: " << attr2.seq << endl;
    
    cout << "status_code: " << attr2.status_code << endl;

    cout << "svrsig " << attr2.svrsig << endl;

    cout << "is_push " << attr2.is_push << endl;

    cout << "test end" << endl;
    return 0;
}

