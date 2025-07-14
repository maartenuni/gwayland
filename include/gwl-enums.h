/*
 * GWayland library gobject wrappers around waylandclient library.
 * Copyright (C) 2025 Maarten Duijndam
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#pragma once

#include "wayland-client-protocol.h"

typedef enum {
    GWL_SHM_FORMAT_ARGB8888             = WL_SHM_FORMAT_ARGB8888,
    GWL_SHM_FORMAT_XRGB888              = WL_SHM_FORMAT_XRGB8888,
    /**
     * 8-bit color index format, [7:0] C
     */
    GWL_SHM_FORMAT_C8                   = WL_SHM_FORMAT_C8,
    /**
     * 8-bit RGB format, [7:0] R:G:B 3:3:2
     */
    GWL_SHM_FORMAT_RGB332               = WL_SHM_FORMAT_RGB332,
    /**
     * 8-bit BGR format, [7:0] B:G:R 2:3:3
     */
    GWL_SHM_FORMAT_BGR233               = WL_SHM_FORMAT_BGR233,
    /**
     * 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_XRGB4444             = WL_SHM_FORMAT_XRGB4444,
    /**
     * 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_XBGR4444             = WL_SHM_FORMAT_XBGR4444,
    /**
     * 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_RGBX4444             = WL_SHM_FORMAT_RGBX4444,
    /**
     * 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_BGRX4444             = WL_SHM_FORMAT_BGRX4444,
    /**
     * 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_ARGB4444             = WL_SHM_FORMAT_ARGB4444,
    /**
     * 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_ABGR4444             = WL_SHM_FORMAT_ABGR4444,
    /**
     * 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_RGBA4444             = WL_SHM_FORMAT_RGBA4444,
    /**
     * 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian
     */
    GWL_SHM_FORMAT_BGRA4444             = WL_SHM_FORMAT_BGRA4444,
    /**
     * 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian
     */
    GWL_SHM_FORMAT_XRGB1555             = WL_SHM_FORMAT_XRGB1555,
    /**
     * 16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian
     */
    GWL_SHM_FORMAT_XBGR1555             = WL_SHM_FORMAT_XBGR1555,
    /**
     * 16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian
     */
    GWL_SHM_FORMAT_RGBX5551             = WL_SHM_FORMAT_RGBX5551,
    /**
     * 16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian
     */
    GWL_SHM_FORMAT_BGRX5551             = WL_SHM_FORMAT_BGRX5551,
    /**
     * 16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian
     */
    GWL_SHM_FORMAT_ARGB1555             = WL_SHM_FORMAT_ARGB1555,
    /**
     * 16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian
     */
    GWL_SHM_FORMAT_ABGR1555             = WL_SHM_FORMAT_ABGR1555,
    /**
     * 16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian
     */
    GWL_SHM_FORMAT_RGBA5551             = WL_SHM_FORMAT_RGBA5551,
    /**
     * 16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian
     */
    GWL_SHM_FORMAT_BGRA5551             = WL_SHM_FORMAT_BGRA5551,
    /**
     * 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian
     */
    GWL_SHM_FORMAT_RGB565               = WL_SHM_FORMAT_RGB565,
    /**
     * 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian
     */
    GWL_SHM_FORMAT_BGR565               = WL_SHM_FORMAT_BGR565,
    /**
     * 24-bit RGB format, [23:0] R:G:B little endian
     */
    GWL_SHM_FORMAT_RGB888               = WL_SHM_FORMAT_RGB888,
    /**
     * 24-bit BGR format, [23:0] B:G:R little endian
     */
    GWL_SHM_FORMAT_BGR888               = WL_SHM_FORMAT_BGR888,
    /**
     * 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_XBGR8888             = WL_SHM_FORMAT_XBGR8888,
    /**
     * 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_RGBX8888             = WL_SHM_FORMAT_RGBX8888,
    /**
     * 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_BGRX8888             = WL_SHM_FORMAT_BGRX8888,
    /**
     * 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_ABGR8888             = WL_SHM_FORMAT_ABGR8888,
    /**
     * 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_RGBA8888             = WL_SHM_FORMAT_RGBA8888,
    /**
     * 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_BGRA8888             = WL_SHM_FORMAT_BGRA8888,
    /**
     * 32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_XRGB2101010          = WL_SHM_FORMAT_XRGB2101010,
    /**
     * 32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_XBGR2101010          = WL_SHM_FORMAT_XBGR2101010,
    /**
     * 32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian
     */
    GWL_SHM_FORMAT_RGBX1010102          = WL_SHM_FORMAT_RGBX1010102,
    /**
     * 32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian
     */
    GWL_SHM_FORMAT_BGRX1010102          = WL_SHM_FORMAT_BGRX1010102,
    /**
     * 32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_ARGB2101010          = WL_SHM_FORMAT_ARGB2101010,
    /**
     * 32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_ABGR2101010          = WL_SHM_FORMAT_ABGR2101010,
    /**
     * 32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian
     */
    GWL_SHM_FORMAT_RGBA1010102          = WL_SHM_FORMAT_RGBA1010102,
    /**
     * 32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian
     */
    GWL_SHM_FORMAT_BGRA1010102          = WL_SHM_FORMAT_BGRA1010102,
    /**
     * packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_YUYV                 = WL_SHM_FORMAT_YUYV,
    /**
     * packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_YVYU                 = WL_SHM_FORMAT_YVYU,
    /**
     * packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_UYVY                 = WL_SHM_FORMAT_UYVY,
    /**
     * packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_VYUY                 = WL_SHM_FORMAT_VYUY,
    /**
     * packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_AYUV                 = WL_SHM_FORMAT_AYUV,
    /**
     * 2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane
     */
    GWL_SHM_FORMAT_NV12                 = WL_SHM_FORMAT_NV12,
    /**
     * 2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane
     */
    GWL_SHM_FORMAT_NV21                 = WL_SHM_FORMAT_NV21,
    /**
     * 2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane
     */
    GWL_SHM_FORMAT_NV16                 = WL_SHM_FORMAT_NV16,
    /**
     * 2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane
     */
    GWL_SHM_FORMAT_NV61                 = WL_SHM_FORMAT_NV61,
    /**
     * 3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes
     */
    GWL_SHM_FORMAT_YUV410               = WL_SHM_FORMAT_YUV410,
    /**
     * 3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes
     */
    GWL_SHM_FORMAT_YVU410               = WL_SHM_FORMAT_YVU410,
    /**
     * 3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes
     */
    GWL_SHM_FORMAT_YUV411               = WL_SHM_FORMAT_YUV411,
    /**
     * 3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes
     */
    GWL_SHM_FORMAT_YVU411               = WL_SHM_FORMAT_YVU411,
    /**
     * 3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes
     */
    GWL_SHM_FORMAT_YUV420               = WL_SHM_FORMAT_YUV420,
    /**
     * 3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes
     */
    GWL_SHM_FORMAT_YVU420               = WL_SHM_FORMAT_YVU420,
    /**
     * 3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes
     */
    GWL_SHM_FORMAT_YUV422               = WL_SHM_FORMAT_YUV422,
    /**
     * 3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes
     */
    GWL_SHM_FORMAT_YVU422               = WL_SHM_FORMAT_YVU422,
    /**
     * 3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes
     */
    GWL_SHM_FORMAT_YUV444               = WL_SHM_FORMAT_YUV444,
    /**
     * 3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes
     */
    GWL_SHM_FORMAT_YVU444               = WL_SHM_FORMAT_YVU444,
    /**
     * [7:0] R
     */
    GWL_SHM_FORMAT_R8                   = WL_SHM_FORMAT_R8,
    /**
     * [15:0] R little endian
     */
    GWL_SHM_FORMAT_R16                  = WL_SHM_FORMAT_R16,
    /**
     * [15:0] R:G 8:8 little endian
     */
    GWL_SHM_FORMAT_RG88                 = WL_SHM_FORMAT_RG88,
    /**
     * [15:0] G:R 8:8 little endian
     */
    GWL_SHM_FORMAT_GR88                 = WL_SHM_FORMAT_GR88,
    /**
     * [31:0] R:G 16:16 little endian
     */
    GWL_SHM_FORMAT_RG1616               = WL_SHM_FORMAT_RG1616,
    /**
     * [31:0] G:R 16:16 little endian
     */
    GWL_SHM_FORMAT_GR1616               = WL_SHM_FORMAT_GR1616,
    /**
     * [63:0] x:R:G:B 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_XRGB16161616F        = WL_SHM_FORMAT_XRGB16161616F,
    /**
     * [63:0] x:B:G:R 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_XBGR16161616F        = WL_SHM_FORMAT_XBGR16161616F,
    /**
     * [63:0] A:R:G:B 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_ARGB16161616F        = WL_SHM_FORMAT_ARGB16161616F,
    /**
     * [63:0] A:B:G:R 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_ABGR16161616F        = WL_SHM_FORMAT_ABGR16161616F,
    /**
     * [31:0] X:Y:Cb:Cr 8:8:8:8 little endian
     */
    GWL_SHM_FORMAT_XYUV8888             = WL_SHM_FORMAT_XYUV8888,
    /**
     * [23:0] Cr:Cb:Y 8:8:8 little endian
     */
    GWL_SHM_FORMAT_VUY888               = WL_SHM_FORMAT_VUY888,
    /**
     * Y followed by U then V, 10:10:10. Non-linear modifier only
     */
    GWL_SHM_FORMAT_VUY101010            = WL_SHM_FORMAT_VUY101010,
    /**
     * [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y
     * pixels
     */
    GWL_SHM_FORMAT_Y210                 = WL_SHM_FORMAT_Y210,
    /**
     * [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y
     * pixels
     */
    GWL_SHM_FORMAT_Y212                 = WL_SHM_FORMAT_Y212,
    /**
     * [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels
     */
    GWL_SHM_FORMAT_Y216                 = WL_SHM_FORMAT_Y216,
    /**
     * [31:0] A:Cr:Y:Cb 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_Y410                 = WL_SHM_FORMAT_Y410,
    /**
     * [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
     */
    GWL_SHM_FORMAT_Y412                 = WL_SHM_FORMAT_Y412,
    /**
     * [63:0] A:Cr:Y:Cb 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_Y416                 = WL_SHM_FORMAT_Y416,
    /**
     * [31:0] X:Cr:Y:Cb 2:10:10:10 little endian
     */
    GWL_SHM_FORMAT_XVYU2101010          = WL_SHM_FORMAT_XVYU2101010,
    /**
     * [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
     */
    GWL_SHM_FORMAT_XVYU12_16161616      = WL_SHM_FORMAT_XVYU12_16161616,
    /**
     * [63:0] X:Cr:Y:Cb 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_XVYU16161616         = WL_SHM_FORMAT_XVYU16161616,
    /**
     * [63:0]   A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0
     * 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
     */
    GWL_SHM_FORMAT_Y0L0                 = WL_SHM_FORMAT_Y0L0,
    /**
     * [63:0]   X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0
     * 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
     */
    GWL_SHM_FORMAT_X0L0                 = WL_SHM_FORMAT_X0L0,
    /**
     * [63:0]   A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
    GWL_SHM_FORMAT_Y0L2                 = WL_SHM_FORMAT_Y0L2,
    /**
     * [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10
     * little endian
     */
    GWL_SHM_FORMAT_X0L2                 = WL_SHM_FORMAT_X0L2,
    GWL_SHM_FORMAT_YUV420_8BIT          = WL_SHM_FORMAT_YUV420_8BIT,
    GWL_SHM_FORMAT_YUV420_10BIT         = WL_SHM_FORMAT_YUV420_10BIT,
    GWL_SHM_FORMAT_XRGB8888_A8          = WL_SHM_FORMAT_XRGB8888_A8,
    GWL_SHM_FORMAT_XBGR8888_A8          = WL_SHM_FORMAT_XBGR8888_A8,
    GWL_SHM_FORMAT_RGBX8888_A8          = WL_SHM_FORMAT_RGBX8888_A8,
    GWL_SHM_FORMAT_BGRX8888_A8          = WL_SHM_FORMAT_BGRX8888_A8,
    GWL_SHM_FORMAT_RGB888_A8            = WL_SHM_FORMAT_RGB888_A8,
    GWL_SHM_FORMAT_BGR888_A8            = WL_SHM_FORMAT_BGR888_A8,
    GWL_SHM_FORMAT_RGB565_A8            = WL_SHM_FORMAT_RGB565_A8,
    GWL_SHM_FORMAT_BGR565_A8            = WL_SHM_FORMAT_BGR565_A8,
    /**
     * non-subsampled Cr:Cb plane
     */
    GWL_SHM_FORMAT_NV24                 = WL_SHM_FORMAT_NV24,
    /**
     * non-subsampled Cb:Cr plane
     */
    GWL_SHM_FORMAT_NV42                 = WL_SHM_FORMAT_NV42,
    /**
     * 2x1 subsampled Cr:Cb plane, 10 bit per channel
     */
    GWL_SHM_FORMAT_P210                 = WL_SHM_FORMAT_P210,
    /**
     * 2x2 subsampled Cr:Cb plane 10 bits per channel
     */
    GWL_SHM_FORMAT_P010                 = WL_SHM_FORMAT_P010,
    /**
     * 2x2 subsampled Cr:Cb plane 12 bits per channel
     */
    GWL_SHM_FORMAT_P012                 = WL_SHM_FORMAT_P012,
    /**
     * 2x2 subsampled Cr:Cb plane 16 bits per channel
     */
    GWL_SHM_FORMAT_P016                 = WL_SHM_FORMAT_P016,
    /**
     * [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian
     */
    GWL_SHM_FORMAT_AXBXGXRX106106106106 = WL_SHM_FORMAT_AXBXGXRX106106106106,
    /**
     * 2x2 subsampled Cr:Cb plane
     */
    GWL_SHM_FORMAT_NV15                 = WL_SHM_FORMAT_NV15,
    GWL_SHM_FORMAT_Q410                 = WL_SHM_FORMAT_Q410,
    GWL_SHM_FORMAT_Q401                 = WL_SHM_FORMAT_Q401,
    /**
     * [63:0] x:R:G:B 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_XRGB16161616         = WL_SHM_FORMAT_XRGB16161616,
    /**
     * [63:0] x:B:G:R 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_XBGR16161616         = WL_SHM_FORMAT_XBGR16161616,
    /**
     * [63:0] A:R:G:B 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_ARGB16161616         = WL_SHM_FORMAT_ARGB16161616,
    /**
     * [63:0] A:B:G:R 16:16:16:16 little endian
     */
    GWL_SHM_FORMAT_ABGR16161616         = WL_SHM_FORMAT_ABGR16161616,
} GwlShmFormat;
