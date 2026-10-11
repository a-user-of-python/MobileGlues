// dbglog.cpp -- minimal on-device GL-call logging for iOS diagnosis.
#include "dbglog.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
namespace {
constexpr unsigned MG_DBG_MAX_LINES = 20000;
std::mutex g_mu;
FILE* g_f = nullptr;
bool g_tried = false;
unsigned g_lines = 0;
void mg_dbg_init_locked() {
    g_tried = true;
    const char* home = getenv("HOME");
    if (!home || !*home) return;
    char path[1024];
    int n = std::snprintf(path, sizeof(path), "%s/Documents/mobileglues_debug.log", home);
    if (n < 0 || (size_t)n >= sizeof(path)) return;
    g_f = std::fopen(path, "w");
}
void mg_dbg_line_locked(const char* line) {
    if (!g_f) return;
    if (g_lines >= MG_DBG_MAX_LINES) return;
    std::fputs(line, g_f);
    std::fputc('\n', g_f);
    ++g_lines;
    std::fflush(g_f);
}
}
void mg_dbg(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    std::lock_guard<std::mutex> lock(g_mu);
    if (!g_tried) mg_dbg_init_locked();
    mg_dbg_line_locked(buf);
}
void mg_dbg_status(unsigned target, unsigned real_status) {
    mg_dbg("FBO status: target=0x%x real=0x%x%s", target, real_status,
           real_status == 0x8CD5 ? " COMPLETE" : " *** INCOMPLETE ***");
}
void mg_dbg_drawbuffers(unsigned fbo, int n, const unsigned* in_bufs, const unsigned* out_bufs) {
    char inb[256] = {0}, outb[256] = {0};
    for (int i = 0; i < n && i < 16; ++i) {
        char t[16];
        std::snprintf(t, sizeof(t), "0x%x ", in_bufs[i]);
        std::strncat(inb, t, sizeof(inb)-strlen(inb)-1);
        std::snprintf(t, sizeof(t), "0x%x ", out_bufs[i]);
        std::strncat(outb, t, sizeof(outb)-strlen(outb)-1);
    }
    mg_dbg("glDrawBuffers fb=%u n=%d in=[%s] out=[%s]", fbo, n, inb, outb);
}
void mg_dbg_attach(const char* what, unsigned fbo, unsigned attachment, unsigned obj) {
    mg_dbg("attach %s fb=%u att=0x%x obj=%u", what, fbo, attachment, obj);
}
void mg_dbg_texfmt(const char* what, unsigned before, unsigned after) {
    if (before != after)
        mg_dbg("texfmt %s: 0x%x -> 0x%x", what, before, after);
}
