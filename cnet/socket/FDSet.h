#pragma once

#include <cnet/platform/Platform.h>
#include <cnet/socket/Socket.h>

#include <jclib/iterator.h>

#include <exception>
#include <stdexcept>

namespace ccap::net
{
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
				CCAP_EXIT();
			};
			this->resize(this->size() + 1);
			this->back() = _sock;
		};
		constexpr void insert(value_type _sock) noexcept(!exceptions_v)
		{
			if constexpr (exceptions_v)
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

		fd_set data_;
	};

	/**
	 * @brief Same as socketlib builtin select but takes in the wrapepd FDSet type
	 * @param _numFds Unused, just set to 0
	 * @param _readSet Optional read set pointer
	 * @param _writeSet Optional write set pointer
	 * @param _exceptSet Optional except set pointer
	 * @param _timeout Optional timeout value, if nullptr this will block until a set was selected
	 * @return Number of sockets changed or error code (if return < 0)
	*/
	inline auto select(int _numFds, FDSet* _readSet, FDSet* _writeSet, FDSet* _exceptSet, ::timeval* _timeout)
	{
		constexpr auto getif = [](FDSet* _set) -> ::fd_set*
		{
			if (_set)
			{
				return &_set->data_;
			}
			else
			{
				return nullptr;
			};
		};
		return ::select(_numFds, getif(_readSet), getif(_writeSet), getif(_exceptSet), _timeout);
	};

};
