#pragma once

#include <cnet/platform/Platform.h>
#include <cnet/socket/Socket.h>

#include <jclib/iterator.h>

#ifdef JCLIB_EXCEPTIONS
#include <exception>
#include <stdexcept>
#endif

#include <chrono>



namespace ccap::net
{
	/**
	 * @brief Minimal wrapper around fd_set giving it STL container semantics
	*/
	struct FDSet
	{
	public:
		using value_type = socket_t;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		using iterator = value_type*;
		using const_iterator = const value_type*;

		constexpr size_type size() const noexcept
		{
			return this->data_.fd_count;
		};

		constexpr pointer data() noexcept
		{
			return &this->data_.fd_array[0];
		};
		constexpr const_pointer data() const noexcept
		{
			return &this->data_.fd_array[0];
		};

		constexpr iterator begin() noexcept
		{
			return this->data();
		};
		constexpr const_iterator begin() const noexcept
		{
			return this->data();
		};
		constexpr const_iterator cbegin() const noexcept
		{
			return this->data();
		};

		constexpr iterator end() noexcept
		{
			return this->begin() + this->size();
		};
		constexpr const_iterator end() const noexcept
		{
			return this->begin() + this->size();
		};
		constexpr const_iterator cend() const noexcept
		{
			return this->begin() + this->size();
		};

		constexpr size_type capacity() const noexcept
		{
			return static_cast<size_type>(FD_SETSIZE);
		};
		constexpr void resize(size_type _len) noexcept
		{
			JCLIB_ASSERT(_len <= this->capacity());
			this->data_.fd_count = _len;
		};

		constexpr reference at(size_type n) noexcept
		{
			JCLIB_ASSERT(this->size() > n);
			return *(this->data() + n);
		};
		constexpr const_reference at(size_type n) const noexcept
		{
			JCLIB_ASSERT(this->size() > n);
			return *(this->data() + n);
		};

		constexpr reference operator[](size_type n) noexcept
		{
			return this->at(n);
		};
		constexpr const_reference operator[](size_type n) const noexcept
		{
			return this->at(n);
		};

		constexpr reference front() noexcept
		{
			return this->at(0);
		};
		constexpr const_reference front() const noexcept
		{
			return this->at(0);
		};

		constexpr reference back() noexcept
		{
			JCLIB_ASSERT(this->size() > 0);
			return *((this->data() + this->size()) - 1);
		};
		constexpr const_reference back() const noexcept
		{
			JCLIB_ASSERT(this->size() > 0);
			return *((this->data() + this->size()) - 1);
		};

		constexpr void insert(value_type _sock, jc::nothrow_t) noexcept
		{
			if (this->size() >= this->capacity())
			{
				JCLIB_ABORT();
			};
			this->resize(this->size() + 1);
			this->back() = _sock;
		};
		constexpr void insert(value_type _sock) noexcept(!JCLIB_EXCEPTIONS)
		{
			if constexpr (JCLIB_EXCEPTIONS)
			{
				if (this->size() >= this->capacity())
				{
					throw std::length_error{ "on FDSet::insert" };
				};
				this->resize(this->size() + 1);
				this->back() = _sock;
			}
			else
			{
				return this->insert(_sock, jc::nothrow);
			};
		};

		constexpr void clear() noexcept
		{
			std::ranges::fill(*this, nullsock);
			this->resize(0);
		};


		fd_set& as_fdset() noexcept
		{
			return this->data_;
		};
		const fd_set& as_fdset() const noexcept
		{
			return this->data_;
		};

	private:
		fd_set data_;
	};




	/**
	 * @brief Converts a chrono duration into a timeval
	 * @return Socket timeout value
	*/
	constexpr inline ::timeval convert_timeval(std::chrono::microseconds _duration)
	{
		const auto _seconds = std::chrono::duration_cast<std::chrono::seconds>(_duration);
		const auto _microSeconds = _duration - std::chrono::duration_cast<std::chrono::microseconds>(_seconds);

		::timeval _out{};

		using timeval_duration = decltype(_out.tv_sec);
		_out.tv_sec = static_cast<timeval_duration>(_seconds.count());
		_out.tv_usec = static_cast<timeval_duration>(_microSeconds.count());

		return _out;
	};

	/**
	 * @brief Converts a chrono duration into a timeval
	 * @return Socket timeout value
	*/
	template <typename Rep, typename Period>
	constexpr inline ::timeval convert_timeval(std::chrono::duration<Rep, Period> _duration)
	{
		return convert_timeval(std::chrono::duration_cast<std::chrono::microseconds>(_duration));
	};




	namespace impl
	{
		constexpr auto get_if = [](auto _getfn, auto _empty)
		{
			return [_getfn, _empty](auto& v)
			{
				return (v && v->size() != 0)? _getfn(v) : _empty;
			};
		};
		constexpr auto get_fdset = impl::get_if([](FDSet* _fd) { return &_fd->as_fdset(); }, nullptr);
	};



	inline int select(int _flags, FDSet* _read, FDSet* _write, FDSet* _excepts, const ::timeval& _timeout)
	{
		auto& _get = impl::get_fdset;
		return ::select(_flags, _get(_read), _get(_write), _get(_excepts), &_timeout);
	};
	inline int select(int _flags, FDSet* _read, FDSet* _write, FDSet* _excepts)
	{
		auto& _get = impl::get_fdset;
		return ::select(_flags, _get(_read), _get(_write), _get(_excepts), nullptr);
	};


};
