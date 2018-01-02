#pragma once
#include <memory>
#include <ostream>
namespace MMap
{
	template <typename T>
	class Container;
}


namespace
{
	template <typename T>
	class WrappedValue
	{
		std::size_t _index;
		MMap::Container<T>* _container;
		friend MMap::Container<T>;
	
	public:
		WrappedValue(std::size_t index, MMap::Container<T>* container):
			_index(index)
			,_container(container)
		{

		}

		operator T const&() const
		{
			return *(_container->_ptr + _index);
		}
		WrappedValue<T>& operator=(T const& t) &
		{
			_container->set_value_at_index(_index, t);

			return *this;
		}
		WrappedValue<T>& operator=(T const& t) &&
		{
			operator=(t);
			return *this;
		}
		WrappedValue<T>& operator=(WrappedValue<T> const& value) &
		{
			operator=((T)value);
			return *this;
		}
	};

	template <typename T, bool IsConst>
	class Iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = typename MMap::Container<T>::difference_type;
		using size_type = typename MMap::Container<T>::size_type;

		using const_value_type = std::add_const_t<typename MMap::Container<T>::value_type>;
		using non_const_value_type = typename MMap::Container<T>::value_type;
		using value_type = std::conditional_t<
			IsConst
			, const_value_type
			, non_const_value_type>;
		using pointer = value_type * ;
		using reference = value_type & ;
		using iterator = std::conditional_t<
			IsConst
			, typename MMap::Container<T>::const_iterator
			, typename MMap::Container<T>::iterator
		>;
	private:
		MMap::Container<T>* _container;
		std::size_t _index;

	public:
		Iterator(MMap::Container<T>* container, std::size_t index) :
			_container(container)
			, _index(index)
		{

		}

		typename iterator& operator=(typename iterator const& Other)
		{
			_index = Other._index;
			_container = Other._container;
			return *this;
		}

		//pre increment; ++a:
		typename iterator& operator++()
		{
			operator=(_container->get_iterator_at_index<iterator>(_index + 1));
			return *this;
		}

		//post increment; a++;
		typename iterator operator++(int)
		{
			auto copy = *this;
			operator=(_container->get_iterator_at_index<iterator>(_index + 1));
			return copy;
		}
		//pre decrement; --a:
		typename iterator& operator--()
		{
			operator=(_container->get_iterator_at_index<iterator>(_index - 1));
			return *this;
		}
		//post decrement; a--;
		typename iterator operator--(int)
		{
			auto copy = *this;
			operator=(_container->get_iterator_at_index<iterator>(_index - 1));
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

		value_type operator*()
		{
			return WrappedValue<T>(_index, _container);
		}
		const_value_type operator*() const
		{
			return WrappedValue<T>(_index, _container);
		}
	};
};
namespace MMap
{
template <typename T>
class Container
{
	T* _ptr;
	std::size_t _size;
	std::unique_ptr<T[]> _allocated_ptr;

	void allocate(uint64_t new_size)
	{
		if (_allocated_ptr && new_size == _size)
		{
			return;
		}

		auto tmp = std::make_unique<T[]>(new_size);
		std::memcpy(tmp.get(), _ptr, _size * sizeof(T));
		_allocated_ptr = std::move(tmp);
		_ptr = _allocated_ptr.get();
		_size = new_size;
	}

	void set_value_at_index(std::size_t index, T const& value) { allocate(_size); *(_ptr + index) = value; }

	using iterator = Iterator<T, false>;
	using const_iterator = Iterator<T, true>;
	using difference_type = ptrdiff_t;
	using size_type = std::size_t;
	using value_type = WrappedValue<T>;

	friend iterator;
	friend const_iterator;

	template <typename ItTy>
	ItTy get_iterator_at_index(std::size_t index) const
	{
		return ItTy(const_cast<MMap::Container<T>*>(this), index);
	}
	friend WrappedValue<T>;
public:

	Container(T* begin_address, T* end_address) :
		_ptr(begin_address)
		, _size(end_address - begin_address)
	{

	}
	Container(T* begin_address) :
		_ptr(begin_address)
		, _size(1)
	{

	}

	template <typename V>
	Container(V * const ptr):
		Container(reinterpret_cast<T*>(ptr))
	{

	}

	template <typename V>
	Container(V* const begin_address, V* const end_address) :
		Container(reinterpret_cast<T*>(begin_address), reinterpret_cast<T*>(end_address))
	{

	}

	Container(Container<T> const& other_container)
	{
		if (!other_container._allocated_ptr)
		{
			_ptr = other_container._ptr;
			_size = other_container._size;
		}
		else
		{
			_ptr = other_container._ptr;
			_size = other_container._size;
			allocate(other_container._size);
		}
	}
	Container(Container<T>&& other_container)
	{
		_ptr = other_container._ptr;
		_size = other_container._size;
		_allocated_ptr = std::move(other_container._allocated_ptr);
	}

	Container() :
		_ptr(nullptr)
		,_size(0)
		,_allocated_ptr()
	{
	}
	void resize(std::size_t size)
	{
		allocate(size);
	}

	std::size_t get_size() const
	{
		return _size;
	}

	WrappedValue<T> operator[](std::size_t index)
	{
		if (index >= _size)
		{
			allocate(index + 1);
		}
		return WrappedValue<T>(index, this);
	}

	const WrappedValue<T> operator[](std::size_t index) const
	{
		return WrappedValue<T>(index, const_cast<MMap::Container<T>*>(this));
	}
	iterator begin()
	{
		return get_iterator_at_index<iterator>(0);
	}

	iterator end()
	{
		return get_iterator_at_index<iterator>(_size);
	}
	const_iterator begin() const
	{
		return get_iterator_at_index<const_iterator>(0);
	}

	const_iterator end() const
	{
		return get_iterator_at_index<const_iterator>(_size);
	}
	friend std::ostream& operator<<(std::ostream& os, MMap::Container<T> const& container)
	{
		os.write(reinterpret_cast<char const*const>(container._ptr), container.get_size() * sizeof(T));
		return os;
	}
};
}

template <typename T, typename MemberType>
MemberType get(MMap::Container<T> const& cow, MemberType T::* _member_ptr, std::size_t index=0)
{
	return ((T)cow[index]).*_member_ptr;
}

template <typename T, typename MemberType, typename ParamType>
void set(MMap::Container<T>& cow, MemberType T::* _member_ptr, ParamType val, std::size_t index = 0)
{
	T t = cow[index];
	t.*_member_ptr = val;
	cow[index] = t;
}

