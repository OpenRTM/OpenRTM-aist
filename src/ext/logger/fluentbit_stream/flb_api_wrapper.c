#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* typeof が無効化されている場合の対策 */
#if !defined(__GNUC__) && !defined(typeof)
#define typeof __typeof__
#endif

/* * 重要: Fluent Bitのヘッダーを読み込む前に、
 * Linuxの標準的なオフセットマクロを確定させます
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* 1. Monkey のコアヘッダーを先にロードさせてマクロ定義を発動させる */
#include <monkey/mk_core.h>

/* 2. 問題の container_of / mk_list_entry を安全な標準 C 構文で強制再定義 */
#ifdef container_of
#undef container_of
#endif
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* --- mk_list_entry のエラー回避用の定義 --- */
#ifdef mk_list_entry
#undef mk_list_entry
#endif
#define mk_list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#include <fluent-bit.h>
#include "flb_api_wrapper.h"

// 実際のFluent Bit APIを呼び出すだけの仲介
flb_wrapper_ctx_t* wrap_flb_create(void) {
    return (flb_wrapper_ctx_t*) flb_create();
}

int wrap_flb_service_set(flb_wrapper_ctx_t* ctx, const char* key, const char* val) {
    return flb_service_set((flb_ctx_t*)ctx, key, val, NULL);
}

int wrap_flb_input(flb_wrapper_ctx_t* ctx, const char* name, void* data) {
    return flb_input((flb_ctx_t*)ctx, name, data);
}

int wrap_flb_output(flb_wrapper_ctx_t* ctx, const char* name, void* data) {
    return flb_output((flb_ctx_t*)ctx, name, data);
}

int wrap_flb_input_set(flb_wrapper_ctx_t* ctx, int f_id, const char* key, const char* val) {
    return flb_input_set((flb_ctx_t*)ctx, f_id, key, val);
}

int wrap_flb_output_set(flb_wrapper_ctx_t* ctx, int f_id, const char* key, const char* val) {
    return flb_output_set((flb_ctx_t*)ctx, f_id, key, val);
}

int wrap_flb_start(flb_wrapper_ctx_t* ctx) {
    return flb_start((flb_ctx_t*)ctx);
}

int wrap_flb_stop(flb_wrapper_ctx_t* ctx) {
    return flb_stop((flb_ctx_t*)ctx);
}

void wrap_flb_destroy(flb_wrapper_ctx_t* ctx) {
    flb_destroy((flb_ctx_t*)ctx);
}

int wrap_flb_lib_push(flb_wrapper_ctx_t* ctx, int f_id, const void* data, size_t len) {
    return flb_lib_push((flb_ctx_t*)ctx, f_id, data, len);
}

