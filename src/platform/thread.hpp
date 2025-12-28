//
// thread.hpp
// foundation
//
// Created by Kristian Trenskow on 2025/12/28.
// See license in LICENSE.
//

#ifndef foundation_platform_thread_hpp

#include <functional>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <process.h>

typedef CRITICAL_SECTION PlatformMutex;
typedef HANDLE PlatformThread;
typedef CONDITION_VARIABLE PlatformCondition;

#else

#include <pthread.h>

typedef pthread_mutex_t PlatformMutex;
typedef pthread_t PlatformThread;
typedef pthread_cond_t PlatformCondition;

#endif

void PlatformMutex_Init(
	PlatformMutex* mutex);

void PlatformMutex_Lock(
	PlatformMutex* mutex);

void PlatformMutex_Unlock(
	PlatformMutex* mutex);

void PlatformMutex_Destroy(
	PlatformMutex* mutex);

void PlatformThread_Create(
	PlatformThread* thread,
	std::function<void()> function);

void PlatformThread_SetName(
	PlatformThread thread,
	const char* name);

void PlatformThread_Join(
	PlatformThread thread);

void PlatformCondition_Init(
	PlatformCondition* condition);

void PlatformCondition_Wait(
	PlatformCondition* condition,
	PlatformMutex* mutex);

void PlatformCondition_NotifyOne(
	PlatformCondition* condition);

void PlatformCondition_NotifyAll(
	PlatformCondition* condition);

void PlatformCondition_Destroy(
	PlatformCondition* condition
);

#endif /* foundation_platform_thread_hpp */
