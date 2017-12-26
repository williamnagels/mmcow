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

		Container<T> _cow(std::begin(simple), std::end(simple));

		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			BOOST_CHECK_EQUAL(_cow[idx], simple[idx]);
			BOOST_CHECK_EQUAL(&(T)(_cow[idx]), simple + idx);
		}
 
		std::iota(std::begin(_cow), std::end(_cow), 100);
		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			BOOST_CHECK_EQUAL((uint8_t)_cow[idx], 100 + idx);
		}

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
			BOOST_CHECK_EQUAL(_cow[idx], idx*2);
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

	Container<uint16_t> _cow(std::begin(simple), std::end(simple));

	uint16_t counter = 0;
	std::for_each(std::cbegin(_cow), std::cend(_cow), [&](auto const& it)
	{
		BOOST_CHECK_EQUAL((uint16_t)counter++, it);
	});
}


BOOST_AUTO_TEST_SUITE_END()