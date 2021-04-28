/**
 * @file mini_sdp/util.h
 * @brief 
 * @version 0.1
 * @date 2021-01-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef MINI_SDP_UTIL_H_
#define MINI_SDP_UTIL_H_

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <arpa/inet.h>

namespace mini_sdp {

struct StrSlice {
    const char* ptr;
    size_t      len;

    std::string ToString() const { return std::string(ptr, len); }

    bool IsEqual(const char* rhs, size_t rhs_len) const {
        if (len != rhs_len) return false;
        return strncmp(ptr, rhs, len) == 0;
    }
    
    bool IsEqual(const StrSlice& slice) const { return IsEqual(slice.ptr, slice.len); }
    
    bool IsEqual(const std::string& str) const { return IsEqual(str.c_str(), str.size()); }
};  // struct StrSlice

/**
 * @brief Split String by a char
 * 
 * @param data 
 * @param len 
 * @param chr 
 * @param is_remove_space ignore empty slice if true
 * @return std::vector<StrSlice> 
 */
std::vector<StrSlice> StrSplit(const char* data, size_t len, char chr, bool is_remove_space = true);

std::pair<std::string, const char*> StrGetFirstSplit(const char* data, size_t len, char chr);

inline bool IsStrEqual(const char* str1, size_t len1, const char* str2, size_t len2) {
    return len1 == len2 ? strncmp(str1, str2, len1) == 0 : false;
}

std::string& Trim(std::string &str);

constexpr uint32_t IPV6_ADDR_LEN = 16;


inline std::string ip2strv6(const unsigned char* ipv6) {
    std::string s_ip;
    char buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, ipv6, buf, INET6_ADDRSTRLEN) != NULL) {
        s_ip.assign(buf);
    }

    return s_ip;
}

inline std::string ip2strv4(unsigned ipv4) {
    std::string s_ip;
    char buf[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &ipv4, buf, INET_ADDRSTRLEN) != NULL) {
        s_ip.assign(buf);
    }

    return s_ip;
}

inline int str2ipv4(const char* str, void *ipv4) {
    if (NULL == str) {
        return -1;
    }

    struct in_addr addr;
    int ret = inet_pton(AF_INET, str, &addr);
    // return 1 on success
    // return 0 when str does not contain a character string representing a valid
    // network address in the specified address family(AF_INET)
    // return -1 when af does not contain a valid address family(not exist in this case)
    if (ret <= 0) {
        return -2;
    } else {
        memcpy(ipv4, &addr.s_addr, 4);
        return 0;
    }
}

inline int str2ipv6(const char* str, void *ipv6) {
    if (NULL == ipv6 || NULL == str) {
        return -1;
    }

    struct in6_addr addr6;
    int ret = inet_pton(AF_INET6, str, &addr6);
    // return 1 on success
    // return 0 when str does not contain a character string representing a valid
    // network address in the specified address family(AF_INET6)
    // return -1 when af does not contain a valid address family
    if (ret <= 0) {
        return -2;
    } else {
        memcpy(ipv6, addr6.s6_addr, IPV6_ADDR_LEN);
        return 0;
    }
}

}  // namespace mini_sdp

#endif  // MINI_SDP_UTIL_H_
