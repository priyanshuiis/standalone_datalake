#pragma once  
// Stub: no-op memory tracing  
struct MemtraceDisableScope {};  
inline void memtrace_set_config(int) {}  
#define MI_MEMTRACE_CONFIG_ENABLE 0  
#define MI_MEMTRACE_CONFIG_EXIT_AFTER_BACKTRACE 0
