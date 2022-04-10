#include "task.hpp"

[[noreturn]] void Task::kill(size_t reason) {
	printf("Should kill now because of 0x%x\n", reason);
	hlt();
	while(true);
}

// exit() would be here
