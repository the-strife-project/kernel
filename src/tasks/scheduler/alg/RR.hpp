#include <klibc/klibc.hpp>

class RoundRobin {
private:
	PubList<PID> list;

public:
	inline size_t size() const { return list.size(); }
	inline void push_front(PID pid) { list.push_front(pid); }
	inline void push(PID pid) { list.push_back(pid); }
	inline PID pop() {
		if(!size())
			return 0;

		PID ret = list.front();
		list.pop_front();
		return ret;
	}
};
