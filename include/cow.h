#pragma once
#include <memory>

template <typename T> 
struct MM_COW;

template <typename T>
struct W_Prot
{
	using iterator=W_Prot<T>;
	using value_type = T;
	using reference = T & ;
	using pointer = T * ;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;

	MM_COW<T>* cow;
	T* v;
	std::size_t idx;
	W_Prot(T* i, std::size_t index, MM_COW<T>* coww) :
		v(i)
		, cow(coww)
		, idx(index)
	{

	}
	

	T& operator*() { return *v; }
	bool operator==(iterator const & other)
	{
		return idx == other.idx; 
	}
	bool operator!=(iterator const & other)
	{ 
		return !(*this == other); 
	}
	operator T&() const { return *v; }
	iterator operator++()
	{ 
		return (*cow)[idx + 1]; 
	}



	iterator operator=(T const& other)
	{
		if (cow->_allocated_ptr)
		{
			*v = other;
			return *this;
		}
		else
		{
			cow->allocate(cow->_current_size);
			(*cow)[idx] = other;
		}

		*this = (*cow)[idx];

		return *this;
	}
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

/*
template <typename T>
typename MM_COW<T>::W_Prot begin(MM_COW<T> const& cow)
{
	return cow[0];
}

template <typename T>
typename MM_COW<T>::W_Prot end(MM_COW<T> const& cow)
{
	return cow[cow._current_size - 1];
}*/