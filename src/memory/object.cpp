//
// object.cpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/01/23
// See license in LICENSE.
//

#include <assert.h>
#include <new>

#include "../exceptions/exceptions.hpp"
#include "./weak.hpp"

#include "./object.hpp"

using namespace foundation::memory;
using namespace foundation::parallelism;
using namespace foundation::exceptions::memory;

Mutex& Object::weakReferencesMutex() {
	alignas(Mutex) static unsigned char storage[sizeof(Mutex)];
	static Mutex* mutex = new (storage) Mutex();
	return *mutex;
}

void Object::addWeakReference(
	void* weakReference
) const {

	if (_weakReferencesCount + 1 > _weakReferencesSize) {

		size_t size = Allocator::calculateBufferLength(_weakReferencesCount + 1);

		void** buffer = (void**)realloc(_weakReferences, size * sizeof(void*));

		if (buffer == nullptr) {
			throw AllocationException(size * sizeof(void*));
		}

		_weakReferences = buffer;
		_weakReferencesSize = size;

	}

	_weakReferences[_weakReferencesCount++] = weakReference;

}

void Object::removeWeakReference(
	void* weakReference
) const {

	for (size_t idx = 0 ; idx < _weakReferencesCount ; idx++) {
		if (_weakReferences[idx] == weakReference) {
			for (size_t midx = idx + 1 ; midx < _weakReferencesCount ; midx++) {
				_weakReferences[midx - 1] = _weakReferences[midx];
			}
			_weakReferencesCount--;
			break;
		}
	}

}

Object::~Object(
) {
	assert(_retainCount == 0);
	weakReferencesMutex().locked([&]() {
		for (size_t idx = 0 ; idx < _weakReferencesCount ; idx++) {
			((Weak<Object>*)_weakReferences[idx])->_object = nullptr;
		}
		_weakReferencesCount = 0;
	});
	if (_weakReferences != nullptr) {
		free(_weakReferences);
	}
}

Object& Object::operator=(
	const Object&
) {
	return *this;
}

Object& Object::operator=(
	Object&&
) {
	return *this;
}

void Object::retain(
) const {
	this->_retainCount++;
}

void Object::release(
) const {
	if (--this->_retainCount == 0) delete this;
}

size_t Object::retainCount(
) const {
	return this->_retainCount;
}

Object::Object(
) : _retainCount(0),
	_weakReferences(nullptr),
	_weakReferencesSize(0),
	_weakReferencesCount(0) { }

Object::Object(
	const Object&
) : Object() { }

Object::Object(
	Object&&
) : Object() { }
