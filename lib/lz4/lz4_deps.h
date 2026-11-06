/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Author: Michal Wilczynski <m.wilczynski@samsung.com>
 *
 * Build environment for the vendored upstream LZ4 sources.
 *
 * upstream/ is LZ4 v1.10.0 plus three backports: 5ef1f16929b5, the aarch64
 * fast loop (0aa33884faf5, 51f8af506b4a, ed86bc1ed802) and the match length
 * limit of 587ed9557708, adapted.  Replace it with a later release rather
 * than patch it further.  The three entry point files include this header
 * first.
 */

#ifndef __LZ4_DEPS_H__
#define __LZ4_DEPS_H__

#include <linux/build_bug.h>	/* static_assert */
#include <linux/compiler.h>	/* __maybe_unused */
#include <linux/string.h>
#include <linux/types.h>

/* lz4.c uses "current" as a local; <asm/current.h> breaks the build. */
#undef current

/* Static so an object carries only the codec it forwards.  Each object
 * forwards only some of them, so mark them __maybe_unused rather than turn
 * off -Wunused-function, which the pre-boot builds would not see anyway.
 */
#define LZ4LIB_VISIBILITY static __maybe_unused

/* Route upstream's "static linking only" entry points through
 * LZ4LIB_VISIBILITY too.
 */
#define LZ4_PUBLISH_STATIC_FUNCTIONS

/* Heap mode only: the stack mode puts LZ4HC's ~64K opt[] in a frame.
 * Allocation goes through the failing stubs below; upstream handles
 * NULL by returning 0.
 */
#define LZ4_USER_MEMORY_FUNCTIONS
#define LZ4_HEAPMODE 1
#define LZ4HC_HEAPMODE 1

/* Failing stubs; nothing in the kernel reaches these. */
static void *LZ4_malloc(size_t s) { return NULL; }
static void *LZ4_calloc(size_t n, size_t s) { return NULL; }
static void LZ4_free(void *p) { }

/* Left alone, upstream reads unaligned words through packed structs, and
 * on ARMv6 through plain misaligned dereferences.  Use memcpy(), upstream's
 * documented default, where unaligned access is cheap, and packed structs,
 * as get_unaligned() does, where it is not: there GCC builds a memcpy()'d
 * word through a stack slot.
 */
#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#define LZ4_FORCE_MEMORY_ACCESS 0
#else
#define LZ4_FORCE_MEMORY_ACCESS 1
#endif

/* On ppc64le GCC, lz4.c marks its decompression entry points and every
 * LZ4_FORCE_INLINE helper optimize("O2"), which makes GCC forget command
 * line options such as -fno-strict-aliasing for those functions (see commit
 * 080b6f407635).  Expand the attribute to nothing; lz4_kernel_api.h undoes
 * this.
 */
#define optimize(level)

/* lz4.c sets this before including lz4.h, since its own code calls
 * deprecated entry points.  lz4.h is included below, ahead of lz4.c.
 */
#define LZ4_DISABLE_DEPRECATE_WARNINGS 1

/* Rename upstream's entry points out of the way; four of them take a
 * trailing wrkmem argument in the kernel.
 */
#define LZ4_compress_fast		__lz4_compress_fast
#define LZ4_compress_default		__lz4_compress_default
#define LZ4_compress_destSize		__lz4_compress_destSize
#define LZ4_resetStream			__lz4_resetStream
#define LZ4_loadDict			__lz4_loadDict
#define LZ4_saveDict			__lz4_saveDict
#define LZ4_compress_fast_continue	__lz4_compress_fast_continue

#define LZ4_decompress_safe		__lz4_decompress_safe
#define LZ4_decompress_safe_partial	__lz4_decompress_safe_partial
#define LZ4_setStreamDecode		__lz4_setStreamDecode
#define LZ4_decompress_safe_continue	__lz4_decompress_safe_continue
#define LZ4_decompress_safe_usingDict	__lz4_decompress_safe_usingDict

#define LZ4_compress_HC			__lz4_compress_HC
#define LZ4_resetStreamHC		__lz4_resetStreamHC
#define LZ4_loadDictHC			__lz4_loadDictHC
#define LZ4_compress_HC_continue	__lz4_compress_HC_continue
#define LZ4_saveDictHC			__lz4_saveDictHC

/* Upstream declares these bare, so both entry point objects would define
 * them.  Declaring them static first gives them internal linkage (C11
 * 6.2.2p4): this one in lz4.h, the three below in lz4.c.
 */
static __maybe_unused int LZ4_compress_destSize_extState(void *state, const char *src,
		char *dst, int *srcSizePtr, int targetDstSize,
		int acceleration);

#include "upstream/lz4.h"

static __maybe_unused int LZ4_compress_forceExtDict(LZ4_stream_t *LZ4_dict,
		const char *source, char *dest, int srcSize);
static __maybe_unused int LZ4_decompress_safe_forceExtDict(const char *source,
		char *dest,
		int compressedSize, int maxOutputSize,
		const void *dictStart, size_t dictSize);
static __maybe_unused int LZ4_decompress_safe_partial_forceExtDict(const char *source,
		char *dest, int compressedSize, int targetOutputSize,
		int dstCapacity, const void *dictStart, size_t dictSize);

#endif /* __LZ4_DEPS_H__ */
