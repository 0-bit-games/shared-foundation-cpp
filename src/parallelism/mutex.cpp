//
// mutex.cpp
// foundation
//
// Created by Kristian Trenskow on 2025/06/26.
// See license in LICENSE.
//

#include "./mutex.hpp"

using namespace foundation::parallelism;

Mutex::Mutex() {

	PlatformMutex_Init(&_mutex);
	PlatformCondition_Init(&_condition);

}

Mutex::~Mutex() {
	PlatformCondition_Destroy(&_condition);
	PlatformMutex_Destroy(&_mutex);
}

void Mutex::lock() const {
	PlatformMutex_Lock(&_mutex);
}

void Mutex::unlock() const {
	PlatformMutex_Unlock(&_mutex);
}

void Mutex::locked(
	std::function<void()> function
) const {
	(void)this->locked<void*>([&]() {
		function();
		return nullptr;
	});
}

void Mutex::wait() {
	PlatformCondition_Wait(&_condition, &_mutex);
}

void Mutex::notify() {
	PlatformCondition_NotifyOne(&_condition);
}

void Mutex::broadcast() {
	PlatformCondition_NotifyAll(&_condition);
}
