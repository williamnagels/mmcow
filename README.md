MMCow (Memory mapped copy-on-write)
=================================
Originally required as part of the development of an **elf linker**[^linker], this module  provides copy-on-write access to an address range consisting of N elements of type T.

Compiled on g++ 7.2.0
```c++
	MMap::Container<T> container(std::begin(array), std::end(array));
```

When writing to an element of the container, the original will persist in memory and is not altered.
A copy of the full array is created and the value is replaced in the copied array.

```c++
	T t;
	container[0] = t;
	BOOST_CHECK_NE(container[0], array[0]);
	BOOST_CHECK_EQUAL(container[0], t)
```
Swap semantics are not supported. 

```c++
	auto tmp = container[0]; 
	container[0] = container[1];
	container[1] = tmp;
	BOOST_CHECK_EQUAL(container[0], 1); 
	BOOST_CHECK_EQUAL(container[1], 1);
```

Swapping variables can be achieved by creating a temporary variable  of type T. container[0] does not return an
element of type T but a wrapper around an variable of type T. This is required in order to catch assigments to the element of the container.

```c++
	auto tmp = (T)container[0];
	container[0] = container[1];
	container[1] = tmp;
	BOOST_CHECK_EQUAL(container[0], 1);
	BOOST_CHECK_EQUAL(container[1], 0); //0 i.s.o. 1
```	

----------
> **Note:**
> - Copying the full range of elements only occurs upon the first write attempt. Additional writes will work
> directly on the copied array.

  [^linker]: https://github.com/williamnagels/linker

