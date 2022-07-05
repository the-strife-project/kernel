#ifndef LIST_HPP
#define LIST_HPP

#include "allocators/allocators.hpp"

// Singly linked list

template<typename T, bool _VISIBILITY=PRIVATE> class List {
private:
	struct Node {
		T tag;
		Node* next;
	};

	Node* first = nullptr;
	Node* last = nullptr;
	size_t _size = 0;

public:
	void push_front(const T& x) {
		Node* node = (Node*)alloc(sizeof(Node), _VISIBILITY);
		node->next = first;
		node->tag = x;
		first = node;
		if(!last)
			last = node;
		++_size;
	}

	void push_back(const T& x) {
		Node* node = (Node*)alloc(sizeof(Node), _VISIBILITY);
		node->next = nullptr;
		node->tag = x;
		if(last)
			last->next = node;
		last = node;
		if(!first)
			first = node;
		++_size;
	}

	inline void pop_front() {
		Node* node = first;
		first = first->next;
		free(node, sizeof(Node), _VISIBILITY);
		--_size;
	}

	inline void clear() {
		while(first)
			pop_front();
		last = nullptr;
	}

	inline size_t size() const { return _size; }
	inline T& front() { return first->tag; }
	inline T& back() { return last->tag; }
	inline const T& front() const { return first->tag; }
	inline const T& back() const { return last->tag; }

	// -- Class stuff ---

	List() = default;
	inline List(const List& other) { *this = other; }
	inline List(List&& other) { *this = other; }

	List& operator=(const List& other) {
		first = last = nullptr;
		_size = 0;
		for(auto const& x : other)
			push_back(x);
		return *this;
	}

	List& operator=(List&& other) {
		if(this != &other) {
			first = other.first;
			other.first = nullptr;
			last = other.last;
			other.last = nullptr;
			_size = other._size;
			other._size = 0;
		}
		return *this;
	}

	inline ~List() { clear(); }

	// --- Iterators ---
	class iterator {
	private:
		Node* node = nullptr;
		iterator(Node* n) : node(n) {}
		friend class List;

	public:
		iterator() {}

		inline void operator++() { node = node->next; }
		inline T& operator*() { return node->tag; }
		inline bool operator==(const iterator& other) const { return node == other.node; }
		inline bool operator!=(const iterator& other) const { return node != other.node; }
	};

	class const_iterator {
	private:
		const Node* node = nullptr;
		const_iterator(const Node* n) : node(n) {}
		friend class List;

	public:
		const_iterator() {}

		inline void operator++() { node = node->next; }
		inline const T& operator*() const { return node->tag; }
		inline bool operator==(const const_iterator& other) const { return node == other.node; }
		inline bool operator!=(const const_iterator& other) const { return node != other.node; }
	};

	iterator begin() { return first; }
	iterator end() { return nullptr; }

	const_iterator begin() const { return first; }
	const_iterator end() const { return nullptr; }

	const_iterator cbegin() const { return first; }
	const_iterator cend() const { return nullptr; }
};

template<typename T> using PubList = List<T, PUBLIC>;
template<typename T> using PrivList = List<T, PRIVATE>;

#endif
