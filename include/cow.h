#pragma once
#include <memory>

template <typename T> 
struct MM_COW;

template <typename T>
struct W_Prot;

template <typename T>
struct WriteProtection
{
	W_Prot<T>* _it;
	WriteProtection(W_Prot<T>* it) :
		_it(it)
	{}
	

	WriteProtection operator=(T const& other)
	{
		if (it->cow->_allocated_ptr)
		{
			*v = other;
			return *this;
		}
		else
		{
			it->cow->allocate(it->cow->_current_size);
			(*it->cow)[idx] = other;
		}

		*this = (*it->cow)[idx];
		return *this;
	}

	operator T&() const { return *v; }
};


template <typename T>
struct W_Prot
{
	using iterator=W_Prot<T>;
	using value_type = WriteProtection<T>;
	using reference = value_type& ;
	using pointer = value_type* ;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	MM_COW<T>* cow;
	value_type v;
	std::size_t idx;
	T* _i;
	W_Prot(T* i, std::size_t index, MM_COW<T>* coww) :
		cow(coww)
		, v(this)
		, idx(index)
		, _i(i)
	{}
	


	bool operator==(iterator const & other)
	{
		return idx == other.idx; 
	}
	bool operator!=(iterator const & other)
	{ 
		return !(*this == other); 
	}


	iterator operator++()
	{ 
		*this = (*cow)[idx+1];
		return *this;
	}
	iterator operator--()
	{
		*this = (*cow)[idx-1];
		return *this;
	}

	reference operator*() { return v; }
	operator reference() const { return v; }
};

template<typename T>
struct MM_COW
{
	using iterator=W_Prot<T>;
	using difference_type = ptrdiff_t;
	using size_type=size_t ;
	using value_type = T;
	using pointer = T*;
	using reference = T & ;


	T* _ptr;
	std::size_t  _current_size;
	std::unique_ptr<T[]> _allocated_ptr;

	MM_COW(T* ptr):
		_ptr(ptr)
		,_current_size(1)
	{
	}

	MM_COW(T* start_ptr, T* end_ptr):
		_ptr(start_ptr)
		,_current_size(end_ptr - start_ptr)
	{
	}
	 
	iterator begin() { return iterator(_ptr, 0, this); }

	iterator end() { return iterator(_ptr + (_current_size-1), _current_size-1, this); }

	iterator operator[](std::size_t index)
	{
		if (index >= _current_size)
		{
			allocate(index + 1);
		}
		return iterator(_ptr + index, index, this);
	}

	void allocate(std::size_t new_size)
	{
		auto tmp_ptr = std::make_unique<T[]>(new_size);

		std::memcpy(tmp_ptr.get(),_ptr, _current_size * sizeof(T));
		_current_size = new_size;
		_allocated_ptr = std::move(tmp_ptr);		
		_ptr = _allocated_ptr.get();
	}
};
