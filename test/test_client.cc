/**
 * @file test/test_client.cc
 * @brief 
 * @version 0.1
 * @date 2021-01-14
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include "../mini_sdp/mini_sdp.h"
#include "../mini_sdp/util.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>

#define PORT	 8000 
#define MAXLINE 1024 

using namespace std;
using namespace mini_sdp;

string origin_sdp =
    "v=0\r\no=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=webrtc_core\r\nt=0 0\r\na=group:BUNDLE audio video\r\n"
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
    "draft-holmer-rmcat-transport-wide-cc-extensions-01\r\na=mid:audio\r\na=rtcp-"
    "mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 nack\r\na=rtcp-fb:111 "
    "transport-cc\r\na=fmtp:111 "
    "minptime=10;stereo=1;useinbandfec=1\r\na=ssrc:27172315 "
    "cname:webrtccore\r\na=ssrc:27172315 "
    "msid:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a opus\r\na=ssrc:27172315 "
    "mslabel:0_xxxx_d71956d9cc93e4a467b11e06fdaf039a\r\na=ssrc:27172315 "
    "label:opus\r\nm=video 1 UDP/TLS/RTP/SAVPF 102 108 123 124 125 127\r\nc=IN "
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
    "playout-delay\r\na=fingerprint:sha-256 "
    "8A:BD:A6:61:75:AF:31:4C:02:81:2A:FA:12:92:4C:48:7B:9F:23:DD:BF:3D:51:30:"
    "E7:59:5C:9B:17:3D:92:34\r\na=setup:passive\r\na=sendrecv\r\na=mid:video\r\na="
    "rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:102 H264/90000\r\na=rtcp-fb:102 ccm "
    "fir\r\na=rtcp-fb:102 goog-remb\r\na=rtcp-fb:102 nack\r\na=rtcp-fb:102 "
    "nack pli\r\na=rtcp-fb:102 transport-cc\r\na=fmtp:102 "
    "level-asymmetry-allowed=1;packetization-mode=1;profile-level-id="
    "42001f\r\na=rtpmap:108 H264/90000\r\na=rtcp-fb:108 ccm "
    "fir\r\na=rtcp-fb:108 goog-remb\r\na=rtcp-fb:108 nack\r\na=rtcp-fb:108 "
    "nack pli\r\na=rtcp-fb:108 transport-cc\r\na=fmtp:108 "
    "level-asymmetry-allowed=1;packetization-mode=0;profile-level-id="
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

SdpType sdp_type = SdpType::kOffer;  
string url = "webrtc://domain/live/xxxx_d71956d9cc93e4a467b11e06fdaf039a";

const uint32_t kMICROSECOND_PER_SECOND = 1000 * 1000;

uint64_t GetNowms() {
    timeval tm;
    gettimeofday(&tm, NULL);
    uint64_t TimeMs = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    return TimeMs;
}

string ip_addr("127.0.0.1");

// Driver code 
int main() {
    cout << "start time: " << GetNowms() << endl;
    int sockfd;
    char buffer[MAXLINE];
    char msg[MAXLINE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    uint32_t temp;
    str2ipv4(ip_addr.c_str(), &temp);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = temp;
    int n;
    socklen_t len = sizeof(struct sockaddr_in);

    OriginSdpAttr attr;
    attr.origin_sdp = origin_sdp;
    attr.sdp_type = sdp_type;
    attr.stream_url = url;
    attr.seq = 0;
    attr.status_code = 0;
    attr.svrsig = "1h8s";
    int ret = ParseOriginSdpToMiniSdp(attr, msg, 1400);
    cout << "pack ret:" << ret << endl;
    int flags = 0;
#ifdef __linux__
    flags = MSG_CONFIRM;
#endif
    sendto(sockfd, (const char *)msg, ret, flags,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,
                 (struct sockaddr *)&servaddr, &len);
    OriginSdpAttr attr2;
    ret = LoadMiniSdpToOriginSdp(buffer, n, attr2);

    cout << "pack ret:" << ret << endl;
    cout << "parse sdp" << endl;
    cout << attr2.origin_sdp << endl;
    cout << "parse url: ";
    cout << attr2.stream_url << endl;
    cout << "seq: " << attr2.seq << endl;
    cout << "status_code: " << attr2.status_code << endl;
    cout << "svrsig " << attr2.svrsig << endl;
    cout << "test end" << endl;
    cout << "end time: " << GetNowms() << endl;
    close(sockfd);
    return 0; 
}