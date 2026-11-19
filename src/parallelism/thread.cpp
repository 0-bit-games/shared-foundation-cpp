//
// thread.cpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/06/26
// See license in LICENSE.
//

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include "./thread.hpp"

#include "../exceptions/exceptions.hpp"

using namespace foundation::parallelism;
using namespace foundation::types;

Thread::Thread(
	const String name,
	std::function<void()> function
) : foundation::memory::Object(),
    _thread(),
    _detached(false) {

	std::function<void()>* threadFunction = new std::function<void()>([=]() {
		name.withCString([&](const char* cString) {
			PlatformThread_SetName(cString);
		});
		function();
	});

	int error = PlatformThread_Create(
		&this->_thread,
		threadFunction);

	if (error != 0) {

		delete threadFunction;

		throw foundation::exceptions::parallelism::ThreadCreationException(error);
	}

}

uint64_t Thread::currentId() {
	return (uint64_t)PlatformThread_CurrentId();
}

void Thread::detach() {
	if (this->_detached) return;
	this->_detached = true;
	PlatformThread_Detach(this->_thread);
}

Thread::~Thread() {
	if (!this->_detached) {
		PlatformThread_Join(this->_thread);
	}
}
