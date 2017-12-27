#define BOOST_TEST_MODULE Linker unittests
#include <boost/test/unit_test.hpp> 
#include <include/cow.h>

#include <numeric>
#include <algorithm>
BOOST_AUTO_TEST_SUITE(mmcow)


template <class T, std::size_t size>
struct ftor
{
	ftor()
	{
		(*this)();
	}
	void operator()()
	{
		T simple[size];
		std::iota(std::begin(simple), std::end(simple), 0);

		MMap::Container<T> _cow(std::begin(simple), std::end(simple));
		
		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			BOOST_CHECK_EQUAL(_cow[idx], simple[idx]);
			_cow[idx] = size-idx;
		}
		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			BOOST_CHECK_EQUAL(_cow[idx], size-idx);
		}
		
		std::iota(std::begin(_cow), std::end(_cow), 0);
		
		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			BOOST_CHECK_EQUAL(_cow[idx], idx);
		}
		std::iota(std::begin(_cow), std::end(_cow), 100);

		uint64_t counter = 100;
		std::for_each(
			std::begin(_cow),
			std::end(_cow),
			[&](auto const& val) 
			{
				BOOST_CHECK_EQUAL((uint8_t)val, counter++);
			});

		for (auto idx = 0; idx < size; idx++)
		{
			_cow[idx] = idx * 2;
			BOOST_CHECK_EQUAL(_cow[idx], idx * 2);
		}

	}
};

BOOST_AUTO_TEST_CASE(fundamental_types)
{
	ftor<uint8_t,  20>();
	ftor<uint16_t, 20>();
	ftor<uint32_t, 20>();
	ftor<uint64_t, 20>();
	ftor<int8_t, 20>();
	ftor<int16_t, 20>();
	ftor<int32_t, 20>();
	ftor<int64_t, 20>();
}

BOOST_AUTO_TEST_CASE(const_iterator_support)
{
	uint16_t simple[100];
	std::iota(std::begin(simple), std::end(simple), 0);

	MMap::Container<uint16_t> _cow(std::begin(simple), std::end(simple));

	uint16_t counter = 0;

	std::for_each(std::cbegin(_cow), std::cend(_cow), [&](auto const& it)
	{
		BOOST_CHECK_EQUAL((uint16_t)counter++, it);
	});
}

BOOST_AUTO_TEST_CASE(aggregate_POD_array)
{
	struct A
	{
		uint64_t value1;
		uint32_t value2;
	};
	A simple[100];
	simple[0].value1 = 100;
	simple[0].value2 = 139;
	MMap::Container<A> _cow(std::begin(simple), std::end(simple));

	BOOST_CHECK_EQUAL(((A)_cow[0]).value1, 100);

	A a;
	a.value1 = 101;
	a.value2 = 150;
	_cow[0] = a;

	BOOST_CHECK_EQUAL(((A)_cow[0]).value1, 101);
	BOOST_CHECK_EQUAL(((A)_cow[0]).value2, 150);

}


BOOST_AUTO_TEST_CASE(aggregate_POD_single_resize)
{
	struct A
	{
		uint16_t value1;
		uint32_t value2;
	};
	A simple;
	MMap::Container<A> _cow(&simple);

	A a;
	a.value1 = 33;
	a.value2 = 34;
	_cow[0] = a;

	BOOST_CHECK_EQUAL(((A)_cow[0]).value1, 33);
	BOOST_CHECK_EQUAL(((A)_cow[0]).value2, 34);

	BOOST_CHECK_EQUAL(1, _cow.get_size());
	_cow.resize(_cow.get_size()+1);
	BOOST_CHECK_EQUAL(2, _cow.get_size());
	a.value1 = 99;
	a.value2 = 100;
	_cow[1] = a;
	BOOST_CHECK_EQUAL(((A)_cow[1]).value1, 99);
	BOOST_CHECK_EQUAL(((A)_cow[1]).value2, 100);

	//auto resize using []
	a.value1 = 150;
	a.value2 = 151;
	_cow[2] = a;
	BOOST_CHECK_EQUAL(((A)_cow[2]).value1, 150);
	BOOST_CHECK_EQUAL(((A)_cow[2]).value2, 151);
}
BOOST_AUTO_TEST_CASE(set_get_POD_A)
{
	struct A
	{
		uint16_t value1;
		uint32_t value2;
	};
	A simple;
	MMap::Container<A> _cow(&simple);
	A a;
	a.value1 = 33;
	a.value2 = 34;
	_cow[0] = a;

	set(_cow, &A::value1, 10);
	auto v = get(_cow, &A::value1);
	BOOST_CHECK_EQUAL(v, 10);
}
BOOST_AUTO_TEST_SUITE_END()