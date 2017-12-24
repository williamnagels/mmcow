#pragma once
#include <memory>

template<typename T>
struct Container;


namespace
{
template <typename T>
class Iterator;

template <typename T>
class WrappedValue
{
	typename Container<T>::iterator* _iterator;
public:
	WrappedValue(typename Container<T>::iterator* iterator) :
		_iterator(iterator) {}
	operator T const&() const 
	{ 
		return *(_iterator->_container->_ptr + _iterator->_index)
	}
	typename Container<T>::value_type& operator=(T const& value)
	{  
		_iterator->_container->allocate(_iterator->_container->_size);
		*(_iterator->_container->_ptr + _iterator->_index) = value;
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

	Iterator(Container<T>* container, std::size_t index):
		_container(container)
		, _wrapped_value(this)
		, _index(index)
	{

	}

	typename Container<T>::iterator& operator=(typename Container<T>::iterator const& Other)
	{
		_index = Other._index;
		_wrapped_value = WrappedValue<T>(this);
		_container = Other._container;
		return *this;
	}

	//pre increment; ++a:
	typename Container<T>::iterator& operator++(int)
	{
		operator=(_container->get_iterator_at_index(_index + 1));
		return *this;
	}

	//pre decrement; --a:
	typename Container<T>::iterator& operator--(int)
	{
		operator=(container->get_iterator_at_index(_index - 1));
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

	friend WrappedValue<T>;
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
		return iterator(this, index);
	}
	bool operator==(Container const& other)
	{
		return this == &other;
	}

	T* _ptr;
	uint64_t _size;
	std::unique_ptr<T[]> _allocated_ptr;

	friend Iterator<T>;
	friend WrappedValue<T>;


	void allocate(uint64_t new_size)
	{
		if (_allocated_ptr && new_size == _size)
		{
			return;
		}

		auto tmp = std::make_unique<T[]>(new_size);
		std::memcpy(tmp.get(), _ptr, _size*sizeof(T));
		_allocated_ptr = std::move(tmp);
		_ptr = _allocated_ptr.get();
		_size = new_size;
	}
public:
	Container(T* begin_ptr, T* end_ptr):
		_ptr(begin_ptr)
		, _size(end_ptr - begin_ptr)
		, _allocated_ptr()
	{
	}
	Container(T* begin_ptr) :
		_ptr(begin_ptr)
		, _size(1)
		, _allocated_ptr()
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
		if (index >= _size)
		{
			allocate(index + 1);
		}

		return *(get_iterator_at_index(index));
	}
};