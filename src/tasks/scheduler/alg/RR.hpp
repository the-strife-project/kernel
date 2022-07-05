#include <klibc/klibc.hpp>

class RoundRobin {
private:
	// It's a good idea that this is a list,
	//   because it's never iterated, so all used operations are O(1).
	// Pretty much zero cache misses, which is the main problem of lists.
	List<PID> list;
	Spinlock lock;

public:
	// Approximate size. It's not guaranteed to be correct, as it doesn't lock.
	inline size_t approxSize() {
		return list.size();
	}

	inline void push(PID pid) {
		lock.acquire();
		list.push_back(pid);
		lock.release();
	}

	inline PID pop() {
		lock.acquire();

		if(!list.size()) {
			lock.release();
			return 0;
		}

		PID ret = list.front();
		list.pop_front();
		lock.release();
		return ret;
	}
};
