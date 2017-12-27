MMCow (Memory mapped copy-on-write)
=================================
Originally required as part of the development of an **elf linker**[^linker], this module  provides copy-on-write access to an address range consisting of N elements of type T.
```
	MMap::Container<T> container(std::begin(array), std::end(array));
```

When writing to an element of the container, the original will persist in memory and is not altered.
A copy of the full array is created and the value is replaced in the copied array.

```
	T t;
	container[0] = t;
	bool b = (container[0] == array[0]); //b =false
	b = (container[0] == t); //b = true
```
Swap semantics are not supported. Elements indexed in the array cannot be assigned to another element.

```
	T a;
	T b;
	container[0] = a;
	container[1] = b;
	
	container[0] = container[1]; //expected is that container[0] = b
	
	bool b = (container[0] == container[1]); //b = false
	b = (container[0] == a); //b == true;
	b = (container[1] == b); //b == true
	
```
----------
> **Note:**
> - Copying the full range of elements only occurs upon the first write attempt. Additional writes will work
> directly on the copied array.

  [^linker]: https://github.com/williamnagels/linker

