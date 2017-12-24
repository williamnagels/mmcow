#define BOOST_TEST_MODULE Linker unittests
#include <boost/test/unit_test.hpp> 
#include <include/cow.h>

#include <numeric>
#include <algorithm>
BOOST_AUTO_TEST_SUITE(mmcow)


BOOST_AUTO_TEST_CASE(simple_uint8_t)
{
	using T = uint8_t;
	T simple[100];
	Container<T> _container(std::begin(simple), std::end(simple));
	auto val = 0;

	//auto it = std::cbegin(_container);
	auto it = std::begin(_container);
	*it = val;
	val++;
	it++;
	*it = val;

	int i = 0;
	int v = i++;
	v = ++i;

	/*
	for (; it != std::end(_container); it++)
	{
		*it = val;
	}*/
}

/*
BOOST_AUTO_TEST_CASE(simple_uint8_t)
{
	uint8_t simple = 3;
	MM_COW<uint8_t> _cow(&simple);
	BOOST_CHECK_EQUAL(simple, 3);
	BOOST_CHECK_EQUAL(_cow[0], 3);
	_cow[0] = 10;

	BOOST_CHECK_EQUAL(simple, 3);
	BOOST_CHECK_EQUAL(_cow[0], 10);
}

BOOST_AUTO_TEST_CASE(simple_uint8_t_array)
{
	uint8_t simple[100];
	std::iota(std::begin(simple), std::end(simple), 0);
	
	MM_COW<uint8_t> _cow(std::begin(simple), std::end(simple));

	BOOST_CHECK_EQUAL(*simple, 0);
	BOOST_CHECK_EQUAL(*(simple+1), 1);
	BOOST_CHECK_EQUAL(&(uint8_t)(_cow[0]), simple);
	BOOST_CHECK_EQUAL((uint8_t)_cow[0], 0);
	BOOST_CHECK_EQUAL(&(uint8_t)(_cow[1]), simple+1);
	BOOST_CHECK_EQUAL((uint8_t)_cow[1], 1);

	_cow[0] = 10;

	uint8_t* first_allocation_ptr = &(uint8_t)(_cow[0]);
	BOOST_CHECK_NE(first_allocation_ptr, simple);
	BOOST_CHECK_EQUAL((uint8_t)_cow[0], 10);
	BOOST_CHECK_EQUAL((uint8_t)_cow[1], 1);
	BOOST_CHECK_EQUAL((uint8_t)_cow[55], 55);
	_cow[100] = 100;
	_cow[55] = 88;
	uint8_t* second_allocation_ptr = &(uint8_t)(_cow[0]);
	BOOST_CHECK_EQUAL((uint8_t)_cow[55], 88);
	BOOST_CHECK_EQUAL((uint8_t)_cow[100], 100);
	BOOST_CHECK_NE(first_allocation_ptr, second_allocation_ptr);

}*/
/*
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

		MM_COW<T> _cow(std::begin(simple), std::end(simple));

		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			//BOOST_CHECK_EQUAL(_cow[idx], simple[idx]);
			//BOOST_CHECK_EQUAL(&(T)(_cow[idx]), simple + idx);
		}

		std::reverse(std::begin(_cow), std::end(_cow));
		//T* first_allocation_ptr = &(T)(_cow[0]);
		for (auto idx = 0; idx < size; idx++)
		{
			BOOST_CHECK_EQUAL(simple[idx], idx);
			//BOOST_CHECK_EQUAL(_cow[idx], simple[idx]);
			//BOOST_CHECK_NE(&(T)(_cow[idx]), simple + idx);
			//BOOST_CHECK_EQUAL(simple[idx], idx);
		}
		BOOST_CHECK_EQUAL((T)_cow[0], 10);
		BOOST_CHECK_EQUAL((T)_cow[1], 1);
		BOOST_CHECK_EQUAL((T)_cow[55], 55);
		BOOST_CHECK_EQUAL((T)_cow[13], 13);
		BOOST_CHECK_EQUAL((T)_cow[12], 12);
		BOOST_CHECK_EQUAL((T)_cow[11], 11);
		_cow[100] = 100;
		_cow[55] = 88;
		T* second_allocation_ptr = &(T)(_cow[0]);
		BOOST_CHECK_EQUAL((T)_cow[55], 88);
		BOOST_CHECK_EQUAL((T)_cow[100], 100);
		BOOST_CHECK_NE(first_allocation_ptr, second_allocation_ptr);
	}
};*/

BOOST_AUTO_TEST_CASE(simple_uint64_t_array)
{
	/*ftor<uint8_t,  5>();
	ftor<uint16_t, 100>;
	ftor<uint32_t, 100>;
	ftor<uint64_t, 100>;
	ftor<int8_t, 100>;
	ftor<int16_t, 100>;
	ftor<int32_t, 100>;
	ftor<int64_t, 100>;*/
}


BOOST_AUTO_TEST_SUITE_END()