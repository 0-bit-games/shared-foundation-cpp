//
// thread.cpp
// foundation
//
// Created by Kristian Trenskow on 2025/06/26.
// See license in LICENSE.
//

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include "./thread.hpp"

using namespace foundation::parallelism;
using namespace foundation::types;

Thread::Thread(
#if defined(__APPLE__) || defined(__linux__)
	const String name,
#else
	const String,
#endif
	std::function<void()> function
) : foundation::memory::Object(),
    _function([=]() {
#if defined(__APPLE__) || defined(__linux__)
    	name.withCString([&](const char* cString) {
    		PlatformThread_SetName(cString);
    	});
#endif
    	function();
    }) {
	
	PlatformThread_Create(
		&this->_thread, 
		this->_function);

}

Thread::~Thread() {
	PlatformThread_Join(this->_thread);
	this->_function = nullptr;
}
