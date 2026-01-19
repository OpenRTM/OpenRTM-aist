#ifndef FLB_API_WRAPPER_H
#define FLB_API_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
/* flb_ctx_t を void として定義し、内部の構造体名の違いを吸収 */
typedef struct flb_wrapper_ctx flb_wrapper_ctx_t;

// FluentBit.cppで使用している関数だけを定義
flb_wrapper_ctx_t* wrap_flb_create(void);
int                wrap_flb_service_set(flb_wrapper_ctx_t* ctx, const char* key, const char* val);
int                wrap_flb_input(flb_wrapper_ctx_t* ctx, const char* name, void* data);
int                wrap_flb_output(flb_wrapper_ctx_t* ctx, const char* name, void* data);
int                wrap_flb_input_set(flb_wrapper_ctx_t* ctx, int f_id, const char* key, const char* val);
int                wrap_flb_output_set(flb_wrapper_ctx_t* ctx, int f_id, const char* key, const char* val);
int                wrap_flb_start(flb_wrapper_ctx_t* ctx);
int                wrap_flb_stop(flb_wrapper_ctx_t* ctx);
void               wrap_flb_destroy(flb_wrapper_ctx_t* ctx);
int                wrap_flb_lib_push(flb_wrapper_ctx_t* ctx, int f_id, const void* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
