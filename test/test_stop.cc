/**
 * @file test/test_stop.cc
 * @brief 
 * @version 0.1
 * @date 2021-01-14
 * 
 * @copyright Copyright (c) 2021 Tencent. All rights reserved.
 * 
 */
#include <cstdio>
#include "mini_sdp.h"

void printx(const char* buff, size_t len) {
    for (int i = 0; i < len; i++) {
        printf("%02X ", (uint8_t)buff[i]);
    }
    printf("\n");
}

int main(int argc, char ** argv) {
    uint32_t ssrc = 0x12345678;

    char pack[1200];
    mini_sdp::StopStreamAttr attr;
    attr.svrsig = "127.0.0.1:abcd:efgh";
    attr.status = 0;
    attr.seq = 0;
    size_t len = mini_sdp::BuildStopStreamPacket(pack, 1200, attr);
    printx(pack, len);

    printf("check pack: %d\n", mini_sdp::IsMiniSdpStopPack(pack, len));

    mini_sdp::StopStreamAttr attr2;
    len = mini_sdp::LoadStopStreamPacket(pack, len, attr2);
    printf("svrsig: %s\nstatus: %hu\nseq: %hu\n", attr2.svrsig.c_str(), attr2.status, attr2.seq);
}
