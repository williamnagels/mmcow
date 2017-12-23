#pragma once
#include <memory>

template<typename T>
struct Container;

namespace
{
template <typename T>
class WrappedValue
{
	T* _value;
public:
	WrappedValue(T* value) :
		_value(value) {}
	operator T const&() const { return *_value; }
	WrappedValue& operator=(T const& value)
	{ 
		*_value = value; 
		return*this;
	}
};

template <typename T>
class Iterator
{
	Container<T>* _container;
	WrappedValue<T> _wrapped_value;
	std::size_t _index;

public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = typename Container<T>::difference_type;
	using size_type = typename Container<T>::size_type;
	using value_type = typename Container<T>::value_type;
	using pointer = typename Container<T>::pointer;
	using reference = typename Container<T>::reference;
	using iterator = typename Container<T>::iterator;

	Iterator(Container<T>* container, T* value, std::size_t index):
		_container(container)
		, _wrapped_value(value)
		, _index(index)
	{

	}

	typename Container<T>::iterator& operator=(typename Container<T>::iterator const& Other)
	{
		_index = Other._index;
		_wrapped_value = Other._wrapped_value;
		_container = Other._container;
		return *this;
	}

	//pre increment; ++a:
	typename Container<T>::iterator& operator++(int)
	{
		*this = _container->get_iterator_at_index(_index+1);
		return *this;
	}

	//pre decrement; --a:
	typename Container<T>::iterator& operator--(int)
	{
		*this = _container->get_iterator_at_index(_index-1);
		return *this;
	}

	//post increment; a++;
	typename Container<T>::iterator operator++()
	{
		auto copy = *this;
		*this =  _container->get_iterator_at_index(_index+1);
		return copy;
	}
	
	//post decrement; a--;
	typename Container<T>::iterator operator--()
	{
		auto copy = *this;
		return _container->get_iterator_at_index(_index-1);
		return copy;
	}

	bool operator!=(typename Container<T>::iterator const& it)
	{
		return !(*this == it);
	}
	bool operator==(typename Container<T>::iterator const& it)
	{
		return _index == it._index; //check container here?
	}

	typename Container<T>::value_type& operator*()
	{
		return _wrapped_value;
	}
};
}

template <typename T>
struct Container
{
	using difference_type = ptrdiff_t;
	using size_type = std::size_t;
	using value_type = WrappedValue<T>;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator = Iterator<T>;
private:
	iterator get_iterator_at_index(std::size_t index)
	{
		return iterator(this, _ptr + index, index);
	}
	bool operator==(Container const& other)
	{
		return this == &other;
	}

	T* _ptr;
	uint64_t _size;

	friend Iterator<T>;
	friend WrappedValue<T>;

public:
	Container(T* begin_ptr, T* end_ptr):
		_ptr(begin_ptr)
		, _size(end_ptr - begin_ptr)
	{
	}

	iterator begin()
	{
		return get_iterator_at_index(0);
	}

	iterator end()
	{
		return get_iterator_at_index(_size); ///one after valid region [0, _size-1] is indexable.
	}

	value_type at(std::size_t index)
	{
		if (index >= _size)
		{
			throw std::exception("");
		}

		return *(get_iterator_at_index(index));
	}

	value_type operator[](std::size_t index)
	{
		return *(get_iterator_at_index(index));
	}
};