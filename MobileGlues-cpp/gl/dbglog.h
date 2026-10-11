// dbglog.h -- minimal on-device GL-call logging for iOS diagnosis.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void mg_dbg(const char* fmt, ...);
void mg_dbg_status(unsigned target, unsigned real_status);
void mg_dbg_drawbuffers(unsigned fbo, int n, const unsigned* in_bufs, const unsigned* out_bufs);
void mg_dbg_attach(const char* what, unsigned fbo, unsigned attachment, unsigned obj);
void mg_dbg_texfmt(const char* what, unsigned before, unsigned after);
#ifdef __cplusplus
}
#endif
