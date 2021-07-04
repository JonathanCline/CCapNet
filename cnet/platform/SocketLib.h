#pragma once

#include <cnet/platform/Exception.h>
#include <jclib/exception.h>

#include <optional>
#include <variant>
#include <bit>
#include <concepts>
#include <cerrno>

namespace ccap::net
{
#ifdef CCAP_NET_WINDOWS
	using SocketLibraryVersion = WORD;
#else
	using SocketLibraryVersion = unsigned short;
#endif

	constexpr inline SocketLibraryVersion make_version(uint8_t _major, uint8_t _minor) noexcept
	{
#ifdef CCAP_NET_WINDOWS
		return MAKEWORD(_major, _minor);
#else
		return SocketLibraryVersion{};
#endif
	};

	struct SocketLibrary
	{
	public:
		bool good() const noexcept { return this->alive_; };
		explicit operator bool() const noexcept { return this->good(); };

		void release()
		{
			this->alive_ = false;
		};
		void reset() noexcept
		{
			if (this->good())
			{
#ifdef CCAP_NET_WINDOWS
				const auto _result = WSACleanup();
				if (_result != 0)
				{
					switch ((SocketError)adjust_platform_error(_result))
					{
					case SocketError::ERR_NOTINITIALISED: // No corresponding startup call, abort if in debug mode
					{
						JCLIB_ASSERT(false);
					};
						break;
					default:
						std::terminate();
						break;
					};
				};
#endif
				this->release();
			};
		};

		SocketLibrary(SocketLibraryVersion _version)
		{
#ifdef CCAP_NET_WINDOWS
			const auto _result = WSAStartup(_version, &this->data_);
			if (_result != 0)
			{
				throw socket_exception{ (SocketError)adjust_platform_error(_result), "on call to WSAStartup" };
			};
#endif
			this->alive_ = true;
		};

#ifdef WINSOCK_VERSION
		SocketLibrary() :
			SocketLibrary{ WINSOCK_VERSION }
		{};
#elif defined(CCAP_NET_WINDOWS)
		SocketLibrary() :
			SocketLibrary{ MAKEWORD(2, 2) }
		{};
#else
		SocketLibrary() = default;
#endif

		SocketLibrary(const SocketLibrary&) = delete;
		SocketLibrary& operator=(const SocketLibrary&) = delete;

#ifdef CCAP_NET_WINDOWS
		SocketLibrary(SocketLibrary&& other) noexcept :
			alive_{ std::exchange(other.alive_, false) }, data_{ std::move(other.data_) }
		{};
#else
		SocketLibrary(SocketLibrary&& other) noexcept :
			alive_{ std::exchange(other.alive_, false) }
		{};
#endif

		SocketLibrary& operator=(SocketLibrary&& other) noexcept
		{
			this->reset();
			this->alive_ = std::exchange(other.alive_, false);
#ifdef CCAP_NET_WINDOWS
			this->data_ = std::move(other.data_);
#endif
			return *this;
		};

		~SocketLibrary()
		{
			this->reset();
		};
	private:
		bool alive_ = true;
#ifdef CCAP_NET_WINDOWS
		WSAData data_;
#endif
	};
};
