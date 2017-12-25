#pragma once
#include <memory>

template<typename T>
struct Container;


namespace
{
template <typename T, bool IsConst>
class Iterator;

template <typename T, bool IsConst>
class WrappedValue
{
	using container_pointer = std::conditional_t <
		IsConst
		, std::add_pointer_t<std::add_const_t<Container<T>>>
		, std::add_pointer_t <Container<T>>
	>;
	container_pointer _container;
	std::size_t _index;
public:
	WrappedValue(container_pointer container, std::size_t index) :
		_container(container),
		_index(index){}
	operator T const&() const 
	{ 
		return *(_container->_ptr + _index);
	}
	typename Container<T>::value_type& operator=(T const& value)
	{  
		_container->allocate(_container->_size);
		*(_container->_ptr + _index) = value;
		return*this;
	}
};

template <typename T, bool IsConst>
class Iterator
{ 
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = typename Container<T>::difference_type;
	using size_type = typename Container<T>::size_type;
	using value_type = std::conditional_t<
		IsConst
		, std::add_const_t<typename Container<T>::value_type>
		, typename Container<T>::value_type>;

	using pointer = value_type * ;
	using reference = value_type &;
	using iterator = std::conditional_t<
		IsConst
		, typename Container<T>::const_iterator
		, typename Container<T>::iterator
	>;

	using container_pointer = std::conditional_t <
		IsConst
		, std::add_pointer_t<std::add_const_t<Container<T>>>
		, std::add_pointer_t <Container<T>>
	>;
	//using container_pointer = std::add_pointer_t <Container<T>>;
private:
	container_pointer _container;
	WrappedValue<T, IsConst> _wrapped_value;
	std::size_t _index;

public:
	Iterator(container_pointer container, std::size_t index):
		_container(container)
		, _wrapped_value(_container, index)
		, _index(index)
	{

	}
	typename iterator& operator=(typename iterator const& Other)
	{
		_index = Other._index;
		_container = Other._container;
		_wrapped_value = WrappedValue<T, IsConst>(_container, _index);
		return *this;
	}

	//pre increment; ++a:
	typename iterator& operator++(int)
	{
		operator=(_container->get_iterator_at_index(_index + 1));
		return *this;
	}

	//post increment; a++;
	typename iterator operator++()
	{
		auto copy = *this;
		operator=(_container->get_iterator_at_index(_index + 1));
		return copy;
	}
	//pre decrement; --a:
	typename iterator& operator--(int)
	{
		operator=(_container->get_iterator_at_index(_index - 1));
		return *this;
	}
	//post decrement; a--;
	typename iterator operator--()
	{
		auto copy = *this;
		operator=(_container->get_iterator_at_index(_index - 1));
		return copy;
	}

	bool operator!=(typename iterator const& it)
	{
		return !(*this == it);
	}
	bool operator==(typename iterator const& it)
	{
		return _index == it._index; //check container here?
	}

	reference operator*()
	{
		return _wrapped_value;
	}

	friend WrappedValue<T, IsConst>;
};
}

template <typename T>
struct Container
{
	using difference_type = ptrdiff_t;
	using size_type = std::size_t;
	using value_type = WrappedValue<T, false>;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator = Iterator<T, false>;
	using const_iterator = Iterator<T, true>;
private:
	const_iterator get_iterator_at_index(std::size_t index) const
	{
		return const_iterator(this, index); //const_cast<Container<T>*>(
	}
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

	friend iterator;
	friend value_type;


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

	const_iterator begin() const
	{
		return get_iterator_at_index(0);
	}

	const_iterator end() const
	{
		return get_iterator_at_index(_size);
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