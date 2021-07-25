#pragma once

#include <cnet/platform/Platform.h>

#include <jclib/ranges.h>
#include <jclib/iterator.h>

#include <compare>

namespace ccap::net
{
	struct AddrInfo
	{
	public:
		using value_type = ::addrinfo;
		using pointer = value_type*;

		pointer get() const noexcept
		{
			return this->data_;
		};
		pointer operator->() const noexcept
		{
			return this->get();
		};

		value_type& operator*() noexcept
		{
			return *this->get();
		};
		const value_type& operator*() const noexcept
		{
			return *this->get();
		};

		bool good() const noexcept
		{
			return this->get() != nullptr;
		};
		explicit operator bool() const noexcept
		{
			return this->good();
		};

		void release()
		{
			this->data_ = nullptr;
		};
		void reset()
		{
			if (this->good())
			{
				::freeaddrinfo(this->get());
				this->release();
			};
		};
		pointer [[nodiscard("owning pointer")]] extract() noexcept
		{
			auto _out = this->get();
			this->release();
			return _out;
		};

		friend inline auto operator<=>(const AddrInfo& lhs, pointer rhs) noexcept
		{
			return lhs.get() <=> rhs;
		};
		friend inline auto operator<=>(pointer lhs, const AddrInfo& rhs) noexcept
		{
			return lhs <=> rhs.get();
		};
		
		auto operator<=>(const AddrInfo& rhs) const noexcept
		{
			return *this <=> rhs.get();
		};



		AddrInfo(pointer _data) noexcept :
			data_{ _data }
		{};
		AddrInfo() noexcept :
			AddrInfo{ nullptr }
		{};

		AddrInfo(const AddrInfo&) = delete;
		AddrInfo& operator=(const AddrInfo&) = delete;

		AddrInfo(AddrInfo&& other) noexcept :
			data_{ other.extract() }
		{};
		AddrInfo& operator=(AddrInfo&& other) noexcept
		{
			JCLIB_ASSERT((*this <=> other) != 0);
			this->reset();
			this->data_ = other.extract();
			return *this;
		};

		~AddrInfo()
		{
			this->reset();
		};

	private:
		::addrinfo* data_;
	};


	struct AddrList
	{
	public:
		using value_type = ::addrinfo;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		reference front() noexcept
		{
			return *this->data_;
		};
		const_reference front() const noexcept
		{
			return *this->data_;
		};

		pointer data()
		{
			return &this->front();
		};
		const_pointer data() const
		{
			return &this->front();
		};

	private:
		template <typename T>
		requires std::same_as<jc::remove_const_t<T>, ::addrinfo>
		struct base_iterator
		{
		public:
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;

			using iterator_category = std::forward_iterator_tag;

			constexpr pointer get() const noexcept
			{
				return this->at_;
			};
			constexpr pointer operator->() const noexcept
			{
				return this->get();
			};
			constexpr reference operator*() const noexcept
			{
				JCLIB_ASSERT(this->at_ != nullptr);
				return *this->get();
			};

			constexpr base_iterator& operator++() noexcept
			{
				auto& _ptr = this->at_;
				JCLIB_ASSERT(_ptr != nullptr);
				_ptr = _ptr->ai_next;
				return *this;
			};
			constexpr base_iterator operator++(int) noexcept
			{
				auto _out{ *this };
				++(*this);
				return _out;
			};

			template <typename _T>
			constexpr bool operator==(const base_iterator<_T>& other) const noexcept
			{
				return this->get() == other.get();
			};
			template <typename _T>
			constexpr bool operator!=(const base_iterator<_T>& other) const noexcept
			{
				return !(*this == other);
			};

			constexpr base_iterator() = default;
			constexpr base_iterator(pointer _at) noexcept :
				at_{ _at }
			{};

		private:
			pointer at_;
		};
		
	public:
		using iterator = base_iterator<value_type>;
		struct const_iterator : public base_iterator<const value_type>
		{
			using base_iterator<const value_type>::base_iterator;
			using base_iterator<const value_type>::operator==;
			using base_iterator<const value_type>::operator!=;

			constexpr const_iterator(iterator _it) noexcept :
				base_iterator<const value_type>{ _it.get() }
			{};
		};

		iterator begin()
		{
			return iterator{ this->data() };
		};
		const_iterator begin() const
		{
			return const_iterator{ this->data() };
		};
		const_iterator cbegin() const
		{
			return const_iterator{ this->begin() };
		};

		iterator end()
		{
			return iterator{ nullptr };
		};
		const_iterator end() const
		{
			return const_iterator{ nullptr };
		};
		const_iterator cend() const
		{
			return const_iterator{ nullptr };
		};

		bool empty() const noexcept
		{
			return !(bool)this->data_;
		};


		AddrList(AddrInfo _data) :
			data_{ std::move(_data) }
		{};

	private:
		AddrInfo data_;
	};


	inline AddrList getaddrinfo(const char* _name, const char* _service, ::addrinfo& _hints)
	{
		addrinfo* _out{};
		const auto _result = ::getaddrinfo(_name, _service, &_hints, &_out);
		if (_result != 0)
		{
			return AddrInfo{};
		};
		return AddrInfo{ _out };
	};

	inline AddrList getaddrinfo(const char* _name, const char* _service)
	{
		addrinfo* _out{};
		const auto _result = ::getaddrinfo(_name, _service, nullptr, &_out);
		if (_result != 0)
		{
			return AddrInfo{};
		};
		return AddrInfo{ _out };
	};


};