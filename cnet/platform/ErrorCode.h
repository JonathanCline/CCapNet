#pragma once

/*
	Defines named error codes and provides a target independent get_error() function.
	I went a little try hard with the portability stuff.
*/

// Small bit of handling to make this header file more portable

#include <cnet/platform/Platform.h>

namespace ccap::net
{

	/**
	 * @brief Named socket function error codes
	*/
	enum class SocketError : int
	{
		// Generic unnamed error.
		ERR_ERROR = -1,

		// No error.
		ERR_NONE = 0,

		// Interrupted function call.
		ERR_INTR = 4,

		// File handle is not valid.
		ERR_BADF = 9,

		// Permission denied.
		ERR_ACCES = 13,

		// Bad address.
		ERR_FAULT = 14,

		// Invalid argument.
		ERR_INVAL = 22,

		// Too many open files.
		ERR_MFILE = 24,

		// Resource temporarily unavailable.
		ERR_WOULDBLOCK = 35,

		// Operation now in progress.
		ERR_INPROGRESS = 36,

		// Operation already in progress.
		ERR_ALREADY = 37,

		// Socket operation on nonsocket.
		ERR_NOTSOCK = 38,

		// Destination address required.
		ERR_DESTADDRREQ = 39,

		// Message too long.
		ERR_MSGSIZE = 40,

		// Protocol wrong type for socket.
		ERR_PROTOTYPE = 41,

		// Bad protocol option.
		ERR_NOPROTOOPT = 42,

		// Protocol not supported.
		ERR_PROTONOSUPPORT = 43,

		// Socket type not supported.
		ERR_SOCKTNOSUPPORT = 44,

		// Operation not supported.
		ERR_OPNOTSUPP = 45,

		// Protocol family not supported.
		ERR_PFNOSUPPORT = 46,

		// Address family not supported by protocol family.
		ERR_AFNOSUPPORT = 47,

		// Address already in use.
		ERR_ADDRINUSE = 48,

		// Cannot assign requested address.
		ERR_ADDRNOTAVAIL = 49,

		// Network is down.
		ERR_NETDOWN = 50,

		// Network is unreachable.
		ERR_NETUNREACH = 51,

		// Network dropped connection on reset.
		ERR_NETRESET = 52,

		// Software caused connection abort.
		ERR_CONNABORTED = 53,

		// Connection reset by peer.
		ERR_CONNRESET = 54,

		// No buffer space available.
		ERR_NOBUFS = 55,

		// Socket is already connected.
		ERR_ISCONN = 56,

		// Socket is not connected.
		ERR_NOTCONN = 57,

		// Cannot send after socket shutdown.
		ERR_SHUTDOWN = 58,

		// Too many references.
		ERR_TOOMANYREFS = 59,

		// Connection timed out.
		ERR_TIMEDOUT = 60,

		// Connection refused.
		ERR_CONNREFUSED = 61,

		// Cannot translate name.
		ERR_LOOP = 62,

		// Name too long.
		ERR_NAMETOOLONG = 63,

		// Host is down.
		ERR_HOSTDOWN = 64,

		// No route to host.
		ERR_HOSTUNREACH = 65,

		// Directory not empty.
		ERR_NOTEMPTY = 66,

		// Too many processes.
		ERR_PROCLIM = 67,

		// User quota exceeded.
		ERR_USERS = 68,

		// Disk quota exceeded.
		ERR_DQUOT = 69,

		// Stale file handle reference.
		ERR_STALE = 70,

		// Item is remote.
		ERR_REMOTE = 71,

		// Network subsystem is unavailable.
		ERR_SYSNOTREADY = 91,

		// Socketlib version out of range.
		ERR_VERNOTSUPPORTED = 92,

		// Successful socketlib startup not yet performed.
		ERR_NOTINITIALISED = 93
	};

	// Expose socket error enumerations
	using enum SocketError;

	/**
	 * @brief Returns the last error set by the current platform, DOES NOT ADJUST FOR TARGET PLATFORM
	 * @return Platform error code
	*/
	inline int get_platform_error() noexcept
	{
#if defined(CCAP_NET_WINDOWS)
		return WSAGetLastError();
#elif defined(CCAP_NET_UNIX)
		return errno();
#else
		return (int)ERR_NONE;
#endif
	};

	namespace impl
	{
#if defined(CCAP_NET_WINDOWS)
		/**
		 * @brief Defines the error code offset for the detected target platform
		*/
		constexpr inline int platform_error_offset_v = 10000;
#else
		/**
		 * @brief Defines the error code offset for the detected target platform
		*/
		constexpr inline int platform_error_offset_v = 0;
#endif
	};

	/**
	 * @brief Used to convert platform-specific socket error codes to Unix error codes,
		does nothing if CCAP_NET_UNIX is defined.
	 * @param _err Platform-specific error code.
	 * @return Unix adjusted error code.
	*/
	constexpr inline int adjust_platform_error(int _err) noexcept
	{
		return _err - impl::platform_error_offset_v;
	};

	/**
	 * @brief Returns the last socket error code, adjusted for target platform
	 * @return Error code, not guarenteed to be enumerated
	*/
	inline SocketError get_error() noexcept
	{
		return SocketError{ adjust_platform_error(get_platform_error()) };
	};

};
