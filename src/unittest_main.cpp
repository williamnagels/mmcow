#define BOOST_TEST_MODULE Linker unittests
#include <boost/test/unit_test.hpp> 
#include <include/cow.h>

#include <iostream>
#include <fstream>
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

BOOST_AUTO_TEST_CASE(swap_issue)
{
	uint16_t simple[100];
	MMap::Container<uint16_t> container(std::begin(simple), std::end(simple));
	container[0] = 0;
	container[1] = 1;

	container[0] = container[1];
	BOOST_CHECK_EQUAL(container[0], 1);
	BOOST_CHECK_EQUAL(container[1], 1);

	container[0] = 0;

	auto tmp = container[0]; 
	// tmp is more or less a pointer and not an actually value. Creating a
	// backup is not supported as of now.
	container[0] = container[1];
	container[1] = tmp;
	BOOST_CHECK_EQUAL(container[0], 1); 
	BOOST_CHECK_EQUAL(container[1], 1);

	container[0] = 0;

	auto tmp2 = (uint16_t)container[0];
	// tmp is more or less a pointer and not an actually value. Creating a
	// backup is not supported as of now.
	container[0] = container[1];
	container[1] = tmp2;
	BOOST_CHECK_EQUAL(container[0], 1);
	BOOST_CHECK_EQUAL(container[1], 0);
}

BOOST_AUTO_TEST_CASE(other_type_reinterpret_cast_single_ptr)
{
	uint8_t simple[100];
	simple[0] = 100;
	simple[1] = 120;

	struct A
	{
		uint8_t should_be_100;
		uint8_t should_be_120;
	};

	MMap::Container<A> container(std::begin(simple), std::end(simple));


	BOOST_CHECK_EQUAL(get(container, &A::should_be_100), 100);
	BOOST_CHECK_EQUAL(get(container, &A::should_be_120), 120);
	BOOST_CHECK_EQUAL(container.get_size(), 50);

	MMap::Container<A> container2(std::begin(simple));
	BOOST_CHECK_EQUAL(get(container2, &A::should_be_100), 100);
	BOOST_CHECK_EQUAL(get(container2, &A::should_be_120), 120);
	BOOST_CHECK_EQUAL(container2.get_size(), 1);
}

BOOST_AUTO_TEST_CASE(copy_ctor)
{
	uint8_t simple[100];
	simple[0] = 100;
	simple[1] = 120;

	MMap::Container<uint8_t> container(std::begin(simple), std::end(simple));
	MMap::Container<uint8_t> container2(container);

	BOOST_CHECK_EQUAL(container[0], 100);
	BOOST_CHECK_EQUAL(container[1], 120);
	BOOST_CHECK_EQUAL(container2[0], 100);
	BOOST_CHECK_EQUAL(container2[1], 120);

	container[0] = 110;

	BOOST_CHECK_EQUAL(container[0], 110);
	BOOST_CHECK_EQUAL(container2[0], 100);

	container2[0] = 144;

	BOOST_CHECK_EQUAL(container[0], 110);
	BOOST_CHECK_EQUAL(container2[0], 144);

	MMap::Container<uint8_t> container3(container);

	BOOST_CHECK_EQUAL(container[0], 110);
	BOOST_CHECK_EQUAL(container2[0], 144);
	BOOST_CHECK_EQUAL(container3[0], 110);
}
BOOST_AUTO_TEST_CASE(move_ctor)
{
	uint8_t simple[100];
	simple[0] = 100;
	simple[1] = 120;
	MMap::Container<uint8_t> container(std::begin(simple), std::end(simple));
	container[0] = 13;

	MMap::Container<uint8_t> container2(std::move(container));

	BOOST_CHECK_EQUAL(container2[0], 13);
}

BOOST_AUTO_TEST_CASE(simple_default_ctor)
{
	MMap::Container<uint8_t> container;
	container[100] = 13;
	BOOST_CHECK_EQUAL(container[100], 13);
	BOOST_CHECK_EQUAL(container.get_size(), 101);
}

BOOST_AUTO_TEST_CASE(POD_default_ctor)
{
	struct A
	{
		uint8_t should_be_100;
		uint8_t should_be_120;
	};

	MMap::Container<A> container;
	A a;
	a.should_be_100 = 100;
	a.should_be_120 = 120;
	container[10] = a;


	BOOST_CHECK_EQUAL(get(container, &A::should_be_100, 10), 100);
	BOOST_CHECK_EQUAL(get(container, &A::should_be_120, 10), 120);
	BOOST_CHECK_EQUAL(container.get_size(), 11);
}

bool is_little_endian()
{
	uint16_t some_value = 10;

	uint8_t* some_ptr = reinterpret_cast<uint8_t*>(&some_value);

	return *some_ptr == some_value;
}

BOOST_AUTO_TEST_CASE(stream_overload)
{
	uint16_t simple[50];

	std::iota(std::begin(simple), std::end(simple), 100); //from 100 to 149

	MMap::Container<uint16_t> container(std::begin(simple), std::end(simple));

	BOOST_CHECK_EQUAL(container.get_size(), 50);
	std::ofstream stream;
	stream.open("dump_function");

	stream << container;

	stream.close();

	std::ifstream ifs("dump_function", std::ifstream::in);

	uint16_t expected_value = 100;
	do
	{
		uint8_t first_byte = ifs.get();
		uint8_t last_byte = ifs.get();

		uint16_t value = 0;

		if (is_little_endian())
		{
			value=first_byte | (last_byte << 8);
		}
		else
		{
			value = last_byte | (first_byte << 8);
		}
		BOOST_CHECK_EQUAL(value, expected_value++);
	} 
	while (expected_value <= 149);
}
BOOST_AUTO_TEST_CASE(file_size)
{
	constexpr std::size_t number_of_elements = 2;
	uint16_t simple[number_of_elements];

	std::iota(std::begin(simple), std::end(simple), 100); //from 100 to 149

	MMap::Container<uint16_t> container(std::begin(simple), std::end(simple));

	BOOST_CHECK_EQUAL(container.get_size(), number_of_elements);
	std::ofstream stream;
	stream.open("dump_function");
	stream << container;
	stream.close();
	std::ifstream ifs("dump_function", std::ifstream::in);

	ifs.seekg(0, std::ios_base::end);
	std::streampos pos = ifs.tellg();

	BOOST_CHECK_EQUAL(number_of_elements * sizeof(uint16_t), pos);
}
BOOST_AUTO_TEST_CASE(compare_ptr_to_const_ptr)
{
	constexpr std::size_t number_of_elements = 2;
	uint8_t simple[number_of_elements];

	std::iota(std::begin(simple), std::end(simple), 100); //from 100 to 149

	MMap::Container<uint8_t> container(std::begin(simple), std::end(simple));

	uint8_t* t = &(*std::begin(container));

	MMap::Container<uint8_t> const& container2(container);

	uint8_t const* t2 = &(*std::begin(container2));

	BOOST_CHECK_EQUAL(t, t2);
}
BOOST_AUTO_TEST_CASE(assignment)
{
	constexpr std::size_t number_of_elements = 2;
	uint8_t simple[number_of_elements];

	std::iota(std::begin(simple), std::end(simple), 100); //from 100 to 149

	MMap::Container<uint8_t> container(std::begin(simple), std::end(simple));

	MMap::Container<uint8_t> container2 = container;
	auto counter = 100;
	std::for_each(std::cbegin(container2), std::cend(container2), [&](auto const& it)
	{
		BOOST_CHECK_EQUAL((uint16_t)counter++, it);
	});

}
BOOST_AUTO_TEST_SUITE_END()