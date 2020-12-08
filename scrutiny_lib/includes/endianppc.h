/*
 *
 * Copyright (C) 2019 - 2020 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1.Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * 3.Neither the name of the copyright holder nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef __Endian__
#define __Endian__
//#define   EFI 1

#define swap16(x)            \
	((((U16)(x)>>8)&0xff) |  \
	 (((U16)(x)&0xff)<<8))

#define swap32(x)                 \
	((((U32)(x)>>24)&0xff) |      \
	((((U32)(x)>>16)&0xff)<<8) |  \
	((((U32)(x)>>8)&0xff)<<16) |  \
	 (((U32)(x)&0xff)<<24))

#if OS_WINDOWS || _DOS || OS_UEFI
#define get16(x) (x)
#define get32(x) (x)
#define get64(x) (x)
#define set16(x) (x)
#define set32(x) (x)
#define set64(x) (x)
#endif

#if __linux__
#include <endian.h>
#include <linux/types.h>
typedef  unsigned long long UINT64;// CQ#86224
#if __BYTE_ORDER == __BIG_ENDIAN
#include <linux/byteorder/big_endian.h>
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <linux/byteorder/little_endian.h>
#endif

#define get16(x) __le16_to_cpu(x)
#define get32(x) __le32_to_cpu(x)
//#define get64(x) __le64_to_cpu(x)// CQ#86224
#define set16(x) __cpu_to_le16(x)
#define set32(x) __cpu_to_le32(x)
#define set64(x) __cpu_to_le64(x)
#define get32ppc64(x) swap32(x)// CQ#86224
#define get64(x) (((UINT64)get32ppc64(((U32 *)&(x))[1])<<32) | get32ppc64(((U32 *)&(x))[0]))// CQ#86224
#endif

#if _FreeBSD_
#include <sys/endian.h>
#include <sys/_types.h>
typedef  uint64_t UINT64;
#define get16(x) (x)
#define get32(x) (x)
#define set16(x) (x)
#define set32(x) (x)
#define get16x(x) (x)
#define get32x(x) (x)
#define set16x(x) (x)
#define set32x(x) (x)
#define get16x_be(x) swap16(x)
#define get32x_be(x) swap32(x)
#define set16x_be(x) swap16(x)
#define set32x_be(x) swap32(x)
#define get64(x) (((uint64_t)get32(((U32 *)&(x))[1])<<32) | get32(((U32 *)&(x))[0]))
#endif // _FreeBSD_

#if _sparc_ || __irix__
#if i386
#define get16(x) (x)
#define get32(x) (x)
#define set16(x) (x)
#define set32(x) (x)
#define get16x(x) (x)
#define get32x(x) (x)
#define set16x(x) (x)
#define set32x(x) (x)
#define get16x_be(x) swap16(x)
#define get32x_be(x) swap32(x)
#define set16x_be(x) swap16(x)
#define set32x_be(x) swap32(x)
#else
#define get16(x) swap16(x)
#define get32(x) swap32(x)
#define set16(x) swap16(x)
#define set32(x) swap32(x)
#define get16x(x) swap16(x)
#define get32x(x) swap32(x)
#define set16x(x) swap16(x)
#define set32x(x) swap32(x)
#define get16x_be(x) (x)
#define get32x_be(x) (x)
#define set16x_be(x) (x)
#define set32x_be(x) (x)
#endif

#define get64(x) (((uint64_t)get32(((U32 *)&(x))[1])<<32) | get32(((U32 *)&(x))[0]))
#endif


#define get2bytes(x, y) (((x[y] << 8) + x[y+1]) & 0xffff)
#define get3bytes(x, y) (((x[y] << 16) + (x[y+1] << 8) + x[y+2]) & 0xffffff)
#define get4bytes(x, y) (((x[y] << 24) + (x[y+1] << 16) + (x[y+2] << 8) + x[y+3]) & 0xffffffff)
#define get8bytes(x, y) (((uint64_t)get4bytes(x, y) << 32) + get4bytes(x, y+4))

#define endian32swap(x) ((((x & 0x000000ff) << 24) | ((x & 0x0000ff00) <<  8) | ((x & 0x00ff0000) >>  8) | ((x & 0xff000000) >> 24)))
#endif   // ifdef __endian.h__                   

