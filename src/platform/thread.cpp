//
// thread.cpp
// foundation
//
// Created by Kristian Trenskow on 2025/12/28.
// See license in LICENSE.
//

#include "./thread.hpp"

#if defined(_WIN32)

void PlatformMutex_Init(
	PlatformMutex* mutex
) {
	InitializeCriticalSection(mutex);
}

void PlatformMutex_Lock(
	PlatformMutex* mutex
) {
	EnterCriticalSection(mutex);
}

void PlatformMutex_Unlock(
	PlatformMutex* mutex
) {
	LeaveCriticalSection(mutex);
}

void PlatformMutex_Destroy(
	PlatformMutex* mutex
) {
	DeleteCriticalSection(mutex);
}

void PlatformThread_Create(
	PlatformThread* thread,
	std::function<void()>* function
) {
	*thread = (HANDLE)_beginthreadex(
		nullptr,
		0,
		[](void* arg) -> unsigned {
			auto func = static_cast<std::function<void()>*>(arg);
			(*func)();
			delete func;
			return 0;
		},
		new std::function<void()>(*function),
		0,
		nullptr
	);
}

void PlatformThread_SetName(
	const char*
) {
	// Not implemented on Windows
}

void PlatformThread_Join(
	PlatformThread thread
) {
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}

void PlatformCondition_Init(
	PlatformCondition* condition
) {
	InitializeConditionVariable(condition);
}

void PlatformCondition_Wait(
	PlatformCondition* condition,
	PlatformMutex* mutex
) {
	SleepConditionVariableCS(condition, mutex, INFINITE);
}

void PlatformCondition_NotifyOne(
	PlatformCondition* condition
) {
	WakeConditionVariable(condition);
}

void PlatformCondition_NotifyAll(
	PlatformCondition* condition
) {
	WakeAllConditionVariable(condition);
}

void PlatformCondition_Destroy(
	PlatformCondition*
) {
	// No destruction needed for CONDITION_VARIABLE
}

#else

void PlatformMutex_Init(
	PlatformMutex* mutex
) {
	pthread_mutex_init(mutex, nullptr);
}

void PlatformMutex_Lock(
	PlatformMutex* mutex
) {
	pthread_mutex_lock(mutex);
}

void PlatformMutex_Unlock(
	PlatformMutex* mutex
) {
	pthread_mutex_unlock(mutex);
}

void PlatformMutex_Destroy(
	PlatformMutex* mutex
) {
	pthread_mutex_destroy(mutex);
}

void PlatformThread_Create(
	PlatformThread* thread,
	std::function<void()>* function
) {
	pthread_create(thread, nullptr, [](void* arg) -> void* {
		auto func = static_cast<std::function<void()>*>(arg);
		(*func)();
		return nullptr;
	}, function);
}

void PlatformThread_SetName(
	const char* name
) {
#if defined(__APPLE__)
	pthread_setname_np(name);
#elif defined(__linux__)
	pthread_setname_np(pthread_self(), name);
#endif
}

void PlatformThread_Join(
	PlatformThread thread
) {
	pthread_join(thread, nullptr);
}

void PlatformCondition_Init(
	PlatformCondition* condition
) {
	pthread_cond_init(condition, nullptr);
}

void PlatformCondition_Wait(
	PlatformCondition* condition,
	PlatformMutex* mutex
) {
	pthread_cond_wait(condition, mutex);
}

void PlatformCondition_NotifyOne(
	PlatformCondition* condition
) {
	pthread_cond_signal(condition);
}

void PlatformCondition_NotifyAll(
	PlatformCondition* condition
) {
	pthread_cond_broadcast(condition);
}

void PlatformCondition_Destroy(
	PlatformCondition* condition
) {
	pthread_cond_destroy(condition);
}

#endif
