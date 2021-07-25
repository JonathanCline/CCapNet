#pragma once

#include <cnet/platform/SocketLib.h>
#include "AddrInfo.h"

#include <memory>

namespace ccap::net
{
#ifdef CCAP_NET_WINDOWS
	/**
	 * @brief Platform socket type alias
	*/
	using socket_t = ::SOCKET;
#else
	/**
	 * @brief Platform socket type alias
	*/
	using socket_t = int;
#endif

#ifdef CCAP_NET_WINDOWS
	/**
	 * @brief Platform-independent null socket value
	*/
	constexpr inline socket_t nullsock = INVALID_SOCKET;
	constexpr inline int sockerr = SOCKET_ERROR;
#else
	/**
	 * @brief Platform-independent null socket value
	*/
	constexpr inline socket_t nullsock = -1;
	constexpr inline int sockerr = -1;
#endif

	struct AddrList;

	/**
	 * @brief Attempts to connect to an address returned from getaddrinfo
	 * @param _address Address list
	 * @return The connected socket if succesfull, otherwise invalid socket
	*/
	inline socket_t connect(const AddrList& _address)
	{
		socket_t _sock = nullsock;
		for (auto& addr : _address)
		{
			_sock = ::socket(addr.ai_family, addr.ai_socktype, addr.ai_protocol);
			if (_sock == nullsock)
			{
				return nullsock;
			};

			const auto _result = ::connect(_sock, addr.ai_addr, addr.ai_addrlen);
			if (_result == sockerr)
			{
				const auto _result = ::closesocket(_sock);
				if (_result == sockerr)
				{
					return nullsock;
				};

				_sock = nullsock;
				continue;
			};
		};
		return _sock;
	};

	/**
	 * @brief Attempts to connect to an address returned from getaddrinfo
	 * @param _address Address name
	 * @param _service Service name, usually port number
	 * @return The connected socket if succesful, otherwise invalid socket
	*/
	inline socket_t connect(const char* _address, const char* _service, ::addrinfo* _hints = nullptr)
	{
		const auto _addrList = (_hints)? getaddrinfo(_address, _service, *_hints) : getaddrinfo(_address, _service);
		return connect(_addrList);
	};




	inline socket_t new_listener(const char* _address, const char* _service, int _backlog, ::addrinfo* _hints = nullptr)
	{
		socket_t _sock{};

		auto _addrList = (_hints) ? getaddrinfo(_address, _service, *_hints) : getaddrinfo(_address, _service);
		for (auto& v : _addrList)
		{
			_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (_sock != net::nullsock)
			{
				const auto _error = ::bind(_sock, v.ai_addr, v.ai_addrlen);
				if (_error == (int)net::ERR_NONE)
				{
					break;
				};
			};

			const auto _closeError = ::closesocket(_sock);
			if (_closeError != (int)net::ERR_NONE)
			{
				continue;
			};
		};

		if (_sock != net::nullsock)
		{
			const auto _result = ::listen(_sock, _backlog);
			if (_result == net::sockerr) [[unlikely]]
			{
				return nullsock;
			};
		};

		return _sock;
	};


	inline void set_blocking(net::socket_t _sock, bool _blocking)
	{
		u_long _arg = (_blocking) ? 0 : 1;
		auto _result = ::ioctlsocket(_sock, FIONBIO, &_arg);
		if (_result == net::sockerr)
		{
			std::terminate();
		};
	};


};

