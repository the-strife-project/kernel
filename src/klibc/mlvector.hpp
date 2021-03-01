#ifndef MLVECTOR_HPP
#define MLVECTOR_HPP

#include "allocators/allocators.hpp"

/*
	Multilevel vector.

	This is a data structure. It behaves like a vector, except it
	uses levels to access the contents. This means that we have chunks
	(pages in this case) of two types:
	- Branch chunks: contain pointers to either branch chunks of the next
		level or leaf chunks.
	- Leaf chunks: contain the actual structures of type T.

	It starts with one level, a leaf one. Let's take sizeof(T) to be 16 and chunk size
	of 4096. Then, there can be 256 elements in it. Once the limit is reached, a new root
	branch chunk is created, with its first pointer pointing to the leaf chunk.

	This means that in under 256 elements, it is an array. Then, over 256 and under
	8192, there are two levels, so it's an array of arrays. Get it?

	The advantage is that there's no need to allocate consecutive pages, something
	that would be hard the way this kernel is structured.

	Does this data structure already exist and has a better name? Open an issue.
*/

/*
	Implementation details:
	- Chunks are pages.
	- There is no copy constructor.
	- An element ID follows a similar structure to paging, where (counting bits from
	  right to left) there is an offset in the leaf chunk (12 bits), followed a series
	  of fields of 9 bits. This way, getting an element is really fast as operations are
	  kept in the bitshifting area. This must be taken into account when using it,
	  as mlvector[1] only makes sense if sizeof(T) == 1.
*/

template<typename T, alloc_t alloc, free_t free> class MLvector {
public:
	typedef uint64_t ID;

private:
	static const size_t POINTERS_PER_PAGE = PAGE_SIZE / sizeof(void*);
	const size_t ELEMENTS_PER_PAGE = PAGE_SIZE / sizeof(T);

	size_t levels = 0;
	void* data = nullptr;
	size_t _size = 0;
	ID _next = 0;	// Next ID to use

	void _destroy(uint64_t* ptr, size_t level) {
		if(level == 1) {
			delete ptr;
			return;
		}

		for(size_t i=0; i<POINTERS_PER_PAGE; ++i) {
			_destroy((uint64_t*)(ptr[i]), level-1);
			ptr[i] = 0;
		}
	}

	// How many elements with l levels?
	size_t maxlevelsize(size_t l) {
		if(l == 0)
			return 0;

		size_t ret = 1 << (9*(l-1));
		ret *= ELEMENTS_PER_PAGE;
		return ret;
	}

	inline void upgrade() {
		uint64_t* ptr = (uint64_t*)PMM::calloc();
		ptr[0] = (uint64_t)data;
		data = ptr;
		++levels;
	}

public:
	MLvector() {
		if(sizeof(T) > PAGE_SIZE)
			panic(Panic::MLVECTOR_TOO_BIG);
	};

	inline MLvector(MLvector&& other) { *this = other; }
	inline MLvector& operator=(MLvector&& other) {
		if(this != &other) {
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}

	void destroy() {
		if(data)
			_destroy((uint64_t*)data, levels);
	}
	//inline ~MLvector() { destroy(); }

	inline size_t size() const { return _size; }

	ID next() {
		// Need a new level?
		if(_size == maxlevelsize(levels))
			upgrade();

		ID ret = _next;
		// Does another element fit?
		if(((ret & 0xFFF) + sizeof(T)) > (PAGE_SIZE - sizeof(T))) {
			// Doesn't fit, need to increment levels
			_next = ((ret >> 12)+1) << 12;
			// Offset (12 lower bits) are left as zero
		} else {
			// It's fine to just increment the offset
			_next += sizeof(T);
		}

		++_size;
		return ret;
	}

	T& operator[](ID id) {
		uint64_t* level = (uint64_t*)data;
		for(size_t i=levels-1; i>0; --i) {
			uint64_t offset = _next >> (12+9*(i-1));
			level = (uint64_t*)(level[offset]);
		}

		return (T&)level[id & 0xFFF];
	}
};

template<typename T> using PrivMLvector = MLvector<T, VMM::Private::alloc, VMM::Private::free>;
template<typename T> using PubMLvector = MLvector<T, VMM::Public::alloc, VMM::Public::free>;

#endif
