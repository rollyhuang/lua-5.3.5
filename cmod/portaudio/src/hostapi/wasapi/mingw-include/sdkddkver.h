/**
 * sdkddkver.h: Version definitions for SDK and DDK. Originally
 * from ReactOS PSDK/DDK, this file is in the public domain:
 *
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#ifndef _INC_SDKDDKVER
#define _INC_SDKDDKVER

/* _WIN32_WINNT */
#define _WIN32_WINNT_NT4 0x0400
#define _WIN32_WINNT_WIN2K 0x0500
#define _WIN32_WINNT_WINXP 0x0501
#define _WIN32_WINNT_WS03 0x0502
#define _WIN32_WINNT_WIN6 0x0600
#define _WIN32_WINNT_VISTA 0x0600
#define _WIN32_WINNT_WS08 0x0600
#define _WIN32_WINNT_LONGHORN 0x0600
#define _WIN32_WINNT_WIN7 0x0601
#define _WIN32_WINNT_WIN8 0x0602
#define _WIN32_WINNT_WINBLUE 0x0603
#define _WIN32_WINNT_WINTHRESHOLD 0x0A00
#define _WIN32_WINNT_WIN10 0x0A00

/* _WIN32_IE */
#define _WIN32_IE_IE20 0x0200
#define _WIN32_IE_IE30 0x0300
#define _WIN32_IE_IE302 0x0302
#define _WIN32_IE_IE40 0x0400
#define _WIN32_IE_IE401 0x0401
#define _WIN32_IE_IE50 0x0500
#define _WIN32_IE_IE501 0x0501
#define _WIN32_IE_IE55 0x0550
#define _WIN32_IE_IE60 0x0600
#define _WIN32_IE_IE60SP1 0x0601
#define _WIN32_IE_IE60SP2 0x0603
#define _WIN32_IE_IE70 0x0700
#define _WIN32_IE_IE80 0x0800
#define _WIN32_IE_IE90 0x0900
#define _WIN32_IE_IE100 0x0a00
#define _WIN32_IE_IE110 0x0A00

/* Mappings Between IE Version and Windows Version */
#define _WIN32_IE_NT4 _WIN32_IE_IE20
#define _WIN32_IE_NT4SP1 _WIN32_IE_IE20
#define _WIN32_IE_NT4SP2 _WIN32_IE_IE20
#define _WIN32_IE_NT4SP3 _WIN32_IE_IE302
#define _WIN32_IE_NT4SP4 _WIN32_IE_IE401
#define _WIN32_IE_NT4SP5 _WIN32_IE_IE401
#define _WIN32_IE_NT4SP6 _WIN32_IE_IE50
#define _WIN32_IE_WIN98 _WIN32_IE_IE401
#define _WIN32_IE_WIN98SE _WIN32_IE_IE50
#define _WIN32_IE_WINME _WIN32_IE_IE55
#define _WIN32_IE_WIN2K _WIN32_IE_IE501
#define _WIN32_IE_WIN2KSP1 _WIN32_IE_IE501
#define _WIN32_IE_WIN2KSP2 _WIN32_IE_IE501
#define _WIN32_IE_WIN2KSP3 _WIN32_IE_IE501
#define _WIN32_IE_WIN2KSP4 _WIN32_IE_IE501
#define _WIN32_IE_XP _WIN32_IE_IE60
#define _WIN32_IE_XPSP1 _WIN32_IE_IE60SP1
#define _WIN32_IE_XPSP2 _WIN32_IE_IE60SP2
#define _WIN32_IE_WS03 0x0602
#define _WIN32_IE_WS03SP1 _WIN32_IE_IE60SP2
#define _WIN32_IE_WIN6 _WIN32_IE_IE70
#define _WIN32_IE_LONGHORN _WIN32_IE_IE70
#define _WIN32_IE_WIN7 _WIN32_IE_IE80
#define _WIN32_IE_WIN8 _WIN32_IE_IE100
#define _WIN32_IE_WINBLUE _WIN32_IE_IE100
#define _WIN32_IE_WINTHRESHOLD _WIN32_IE_IE110
#define _WIN32_IE_WIN10 _WIN32_IE_IE110

/* NTDDI_VERSION */
#ifndef NTDDI_WIN2K
#define NTDDI_WIN2K 0x05000000
#endif
#ifndef NTDDI_WIN2KSP1
#define NTDDI_WIN2KSP1 0x05000100
#endif
#ifndef NTDDI_WIN2KSP2
#define NTDDI_WIN2KSP2 0x05000200
#endif
#ifndef NTDDI_WIN2KSP3
#define NTDDI_WIN2KSP3 0x05000300
#endif
#ifndef NTDDI_WIN2KSP4
#define NTDDI_WIN2KSP4 0x05000400
#endif

#ifndef NTDDI_WINXP
#define NTDDI_WINXP 0x05010000
#endif
#ifndef NTDDI_WINXPSP1
#define NTDDI_WINXPSP1 0x05010100
#endif
#ifndef NTDDI_WINXPSP2
#define NTDDI_WINXPSP2 0x05010200
#endif
#ifndef NTDDI_WINXPSP3
#define NTDDI_WINXPSP3 0x05010300
#endif
#ifndef NTDDI_WINXPSP4
#define NTDDI_WINXPSP4 0x05010400
#endif

#define NTDDI_WS03 0x05020000
#define NTDDI_WS03SP1 0x05020100
#define NTDDI_WS03SP2 0x05020200
#define NTDDI_WS03SP3 0x05020300
#define NTDDI_WS03SP4 0x05020400

#define NTDDI_WIN6 0x06000000
#define NTDDI_WIN6SP1 0x06000100
#define NTDDI_WIN6SP2 0x06000200
#define NTDDI_WIN6SP3 0x06000300
#define NTDDI_WIN6SP4 0x06000400

#define NTDDI_VISTA NTDDI_WIN6
#define NTDDI_VISTASP1 NTDDI_WIN6SP1
#define NTDDI_VISTASP2 NTDDI_WIN6SP2
#define NTDDI_VISTASP3 NTDDI_WIN6SP3
#define NTDDI_VISTASP4 NTDDI_WIN6SP4
#define NTDDI_LONGHORN NTDDI_VISTA

#define NTDDI_WS08 NTDDI_WIN6SP1
#define NTDDI_WS08SP2 NTDDI_WIN6SP2
#define NTDDI_WS08SP3 NTDDI_WIN6SP3
#define NTDDI_WS08SP4 NTDDI_WIN6SP4

#define NTDDI_WIN7 0x06010000
#define NTDDI_WIN8 0x06020000
#define NTDDI_WINBLUE 0x06030000
#define NTDDI_WINTHRESHOLD 0x0A000000
#define NTDDI_WIN10 0x0A000000
#define NTDDI_WIN10_TH2 0x0A000001
#define NTDDI_WIN10_RS1 0x0A000002
#define NTDDI_WIN10_RS2 0x0A000003
#define NTDDI_WIN10_RS3 0x0A000004
#define NTDDI_WIN10_RS4 0x0A000005
#define NTDDI_WIN10_RS5 0x0A000006
#define NTDDI_WIN10_19H1 0x0A000007
#define NTDDI_WIN10_VB 0x0A000008
#define NTDDI_WIN10_MN 0x0A000009
#define NTDDI_WIN10_FE 0x0A00000A

#define WDK_NTDDI_VERSION NTDDI_WIN10_FE

/* Version Fields in NTDDI_VERSION */
#define OSVERSION_MASK 0xFFFF0000U
#define SPVERSION_MASK 0x0000FF00
#define SUBVERSION_MASK 0x000000FF

/* Macros to Extract Version Fields From NTDDI_VERSION */
#define OSVER(Version) ((Version)&OSVERSION_MASK)
#define SPVER(Version) (((Version)&SPVERSION_MASK) >> 8)
#define SUBVER(Version) (((Version)&SUBVERSION_MASK))

/* Macros to get the NTDDI for a given WIN32 */
#define NTDDI_VERSION_FROM_WIN32_WINNT2(Version) Version##0000
#define NTDDI_VERSION_FROM_WIN32_WINNT(Version) NTDDI_VERSION_FROM_WIN32_WINNT2(Version)

/* Select Default WIN32_WINNT Value */
#if !defined(_WIN32_WINNT) && !defined(_CHICAGO_)
#define _WIN32_WINNT _WIN32_WINNT_WS03
#endif

/* Choose NTDDI Version */
#ifndef NTDDI_VERSION
#ifdef _WIN32_WINNT
#define NTDDI_VERSION NTDDI_VERSION_FROM_WIN32_WINNT(_WIN32_WINNT)
#else
#define NTDDI_VERSION NTDDI_WS03
#endif
#endif

/* Choose WINVER Value */
#ifndef WINVER
#ifdef _WIN32_WINNT
#define WINVER _WIN32_WINNT
#else
#define WINVER 0x0502
#endif
#endif

/* Choose IE Version */
#ifndef _WIN32_IE
#ifdef _WIN32_WINNT
#if (_WIN32_WINNT <= _WIN32_WINNT_NT4)
#define _WIN32_IE _WIN32_IE_IE50
#elif (_WIN32_WINNT <= _WIN32_WINNT_WIN2K)
#define _WIN32_IE _WIN32_IE_IE501
#elif (_WIN32_WINNT <= _WIN32_WINNT_WINXP)
#define _WIN32_IE _WIN32_IE_IE60
#elif (_WIN32_WINNT <= _WIN32_WINNT_WS03)
#define _WIN32_IE _WIN32_IE_WS03
#elif (_WIN32_WINNT <= _WIN32_WINNT_VISTA)
#define _WIN32_IE _WIN32_IE_LONGHORN
#elif (_WIN32_WINNT <= _WIN32_WINNT_WIN7)
#define _WIN32_IE _WIN32_IE_WIN7
#elif (_WIN32_WINNT <= _WIN32_WINNT_WIN8)
#define _WIN32_IE _WIN32_IE_WIN8
#else
#define _WIN32_IE 0x0a00
#endif
#else
#define _WIN32_IE 0x0700
#endif
#endif

/* Make Sure NTDDI_VERSION and _WIN32_WINNT Match */
#if ((OSVER(NTDDI_VERSION) == NTDDI_WIN2K) && (_WIN32_WINNT != _WIN32_WINNT_WIN2K)) || \
    ((OSVER(NTDDI_VERSION) == NTDDI_WINXP) && (_WIN32_WINNT != _WIN32_WINNT_WINXP)) || \
    ((OSVER(NTDDI_VERSION) == NTDDI_WS03) && (_WIN32_WINNT != _WIN32_WINNT_WS03)) || \
    ((OSVER(NTDDI_VERSION) == NTDDI_WINXP) && (_WIN32_WINNT != _WIN32_WINNT_WINXP))
#error NTDDI_VERSION and _WIN32_WINNT mismatch!
#endif

#endif /* _INC_SDKDDKVER */