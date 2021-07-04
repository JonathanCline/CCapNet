#pragma once

#include <cnet/platform/ErrorCode.h>

#include <exception>

namespace ccap::net
{
	struct socket_exception : public std::exception
	{
	public:
		explicit operator bool() const noexcept = delete;
		operator SocketError() const noexcept
		{
			return this->code_;
		};

		SocketError code() const noexcept
		{
			return this->code_;
		};

		socket_exception(SocketError _code, const char* _what) :
			std::exception{ _what }, code_{ _code }
		{};
		socket_exception(SocketError _code) :
			socket_exception{ _code, nullptr }
		{};

	private:
		SocketError code_ = SocketError::ERR_ERROR;
	};
};
