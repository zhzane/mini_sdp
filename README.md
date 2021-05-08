# mini sdp

mini_sdp 通过将原生 sdp 转换成更小的二进制格式，使其能够通过一个 UDP 包来传输。源码实现了mini_sdp v0 版本，后续将逐步转为 v1 版本，具体规范参考：[基于udp的sdp信令交互方案V1](./mini_sdp_spec_v1.pdf)

> [mini sdp v1 腾讯在线文档](https://docs.qq.com/doc/DSVBmY2R2Y1dLU0xa?)

## Architecture
mini_sdp 主要包含原始 SDP 的解析和 mini sdp 格式的转换。主要文件包括：
- `sdp.h (.cc)` 原始 SDP 描述结构，包含从 C++ 结构到原始 SDP 字符串的转换
- `sdp_parser.h (.cc)` 原始 SDP 解析，将原始 SDP 字符串解析为 C++ SDP 描述结构
- `mini_sdp.h (.cc)` MiniSdp 接口。mini sdp 的二进制表示格式会有不同版本，包括当前已经实现的 v0，和即将实现的 v1。但 MiniSdp 的 C++ 接口是一致的

## C++ Interface
mini_sdp 的 C++ 接口说明参考源码 [C++ Interface](./mini_sdp/mini_sdp.h)
