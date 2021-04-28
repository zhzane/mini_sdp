/**
 * @file mini_sdp/util.cc
 * @brief 
 * @version 0.1
 * @date 2021-01-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <cstring>
#include "util.h"

namespace mini_sdp {

std::vector<StrSlice> StrSplit(const char* data, size_t len, char chr, bool is_remove_space) {
    std::vector<StrSlice> slices;
    const char* ppos = nullptr;
    size_t word_len;

    StrSlice slice;
    while (len > 0) {
        ppos = (const char*)memchr(data, chr, len);
        slice.ptr = data;
        if (ppos) {
            slice.len = ppos - data;
            len -= slice.len + 1;
            data = ppos + 1;
            if (is_remove_space) {
                while (len > 0 && *data == chr) {
                    data++;
                    len--;
                }
            }
        } else {
            slice.len = len;
            len = 0;
        }
        slices.push_back(slice);
    }
    return slices;
}

std::pair<std::string, const char*> StrGetFirstSplit(const char* data, size_t len, char chr) {
    const char* pos = (const char*)memchr(data, chr, len);
    if (pos == nullptr) {
        return std::make_pair(std::string(data, len), nullptr);
    } 
    return std::make_pair(std::string(data, pos - data), pos + 1);
}

std::string& Trim(std::string &str) {  
    if (str.empty())   
    {  
        return str;  
    }  
 
    str.erase(0,str.find_first_not_of("\r\t"));  
    str.erase(str.find_last_not_of("\r\t") + 1);  
    return str;  
}  

}  // namespace mini_sdp