#pragma once

/*
	Detects the target platform and provides a basic language to talk about platforms within C++
*/

// Potential target platforms should be listed below

// Unknown or unsupported target platform code
#define CCAP_NET_TARGET_PLATFORM_ERROR 0
// Windows target platform code
#define CCAP_NET_TARGET_PLATFORM_WINDOWS 1
// Unix or similar target platform code
#define CCAP_NET_TARGET_PLATFORM_UNIX 2

namespace ccap::net
{
	/**
	 * @brief Target platform enumerator to bridge the gap between macros and C++
	*/
	enum class TargetPlatform : int
	{
		Error = CCAP_NET_TARGET_PLATFORM_ERROR,
		Windows = CCAP_NET_TARGET_PLATFORM_WINDOWS,
		Unix = CCAP_NET_TARGET_PLATFORM_UNIX
	};
};

// Platform detection

#if defined(WIN32) || defined(_WIN32)
// The determined target platform
#define CCAP_NET_TARGET CCAP_NET_TARGET_PLATFORM_WINDOWS
#elif defined(__unix__) or defined(__linux__)
// The determined target platform
#define CCAP_NET_TARGET CCAP_NET_TARGET_PLATFORM_UNIX
#endif



// If the target was not set in the above, set it to error

#ifndef CCAP_NET_TARGET
// The determined target platform
#define CCAP_NET_TARGET CCAP_NET_TARGET_PLATFORM_ERROR
#endif

namespace ccap::net
{
	/**
	 * @brief The detected target platform the library is being compiled for
	*/
	constexpr inline TargetPlatform platform_v = TargetPlatform{ CCAP_NET_TARGET };
};

// Unsure shorthands are not already defined

#ifdef CCAP_NET_WINDOWS
#error CCAP_NET_WINDOWS should not already be defined!
#endif
#ifdef CCAP_NET_UNIX
#error CCAP_NET_UNIX should not already be defined!
#endif

// Depending on the target determined above, set the corresponding shorthand macro for #ifdef use

#if CCAP_NET_TARGET == CCAP_NET_TARGET_PLATFORM_WINDOWS
// Defined if Windows is the determined target platform
#define CCAP_NET_WINDOWS
#elif CCAP_NET_TARGET == CACP_NET_TARGET_PLATORM_UNIX
// Defined if Unix or similar is the determined target platform
#define CCAP_NET_UNIX
#endif



#ifdef CCAP_NET_WINDOWS

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#undef min
#undef max

#elif defined(CCAP_NET_UNIX)

#include <cerrno>

#endif

