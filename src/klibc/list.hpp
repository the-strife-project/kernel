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

public:
	void push_front(const T& x) {
		Node* node = (Node*)alloc(sizeof(Node), _VISIBILITY);
		node->next = first;
		node->tag = x;
		first = node;
		if(!last)
			last = node;
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
	}

	inline void pop_front() {
		Node* node = first;
		first = first->next;
		free(node, sizeof(Node), _VISIBILITY);
	}

	inline void clear() {
		while(first)
			pop_front();
		last = nullptr;
	}

	inline ~List() { clear(); }

	// --- Iterators ---
	class iterator {
	private:
		Node* node = nullptr;
		iterator(Node* n) : node(n) {}
		friend class List<T, _VISIBILITY>;

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
		friend class List<T, _VISIBILITY>;

	public:
		const_iterator() {}

		inline void operator++() { node = node->next; }
		inline const T& operator*() const { return node->tag; }
		inline bool operator==(const const_iterator& other) const { return node == other.node; }
		inline bool operator!=(const const_iterator& other) const { return node == other.node; }
	};

	iterator begin() { return first; }
	iterator end() { return nullptr; }

	const_iterator begin() const { return first; }
	const_iterator end() const { return nullptr; }

	const_iterator cbegin() const { return first; }
	const_iterator cend() const { return nullptr; }
};

#endif
