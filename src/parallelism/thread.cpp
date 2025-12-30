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
	const String name,
	std::function<void()> function
) : foundation::memory::Object(),
    _function([=]() {
    	name.withCString([&](const char* cString) {
    		PlatformThread_SetName(cString);
    	});
    	function();
    }) {
	
	PlatformThread_Create(
		&this->_thread, 
		&this->_function);

}

Thread::~Thread() {
	PlatformThread_Join(this->_thread);
	this->_function = nullptr;
}
