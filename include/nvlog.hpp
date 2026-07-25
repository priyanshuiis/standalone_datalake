#include <cstdio>  
#include <fmt/format.h>  
#include "aerial_event_code.h"  
#include "memtrace.h"  
  
#define NVLOG_TAG_BASE_CUPHY_CONTROLLER 100  
  
#define NVLOGV_FMT(tag, fmt_str, ...)  /* no-op */  
#define NVLOGD_FMT(tag, fmt_str, ...)  /* no-op */  
#define NVLOGI_FMT(tag, fmt_str, ...)  fmt::print("[INFO] " fmt_str "\n", ##__VA_ARGS__)  
#define NVLOGW_FMT(tag, fmt_str, ...)  fmt::print("[WARN] " fmt_str "\n", ##__VA_ARGS__)  
#define NVLOGC_FMT(tag, fmt_str, ...)  fmt::print("[CONS] " fmt_str "\n", ##__VA_ARGS__)  
#define NVLOGE_FMT(tag, event, fmt_str, ...) \  
    fmt::print("[ERR][{}] " fmt_str "\n", #event, ##__VA_ARGS__)  
#define NVLOGF_FMT(tag, event, fmt_str, ...) \  
    do { fmt::print("[FATAL][{}] " fmt_str "\n", #event, ##__VA_ARGS__); exit(1); } while(0)
