//
//  object.hpp
//  fart
//
//  Created by Kristian Trenskow on 17/08/2018.
//  Copyright © 2018 Kristian Trenskow. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include <assert.h>
#include <unistd.h>
#include <atomic>

#include "../threading/mutex.hpp"
#include "./weak.hpp"
#include "./allocator.hpp"

using namespace fart::threading;
using namespace fart::exceptions::memory;

namespace fart::types {
	template<class T>
	class Array;
}

namespace fart::memory {

	class Object
#ifdef FART_ALLOW_MANUAL_HEAP
	: public Allocator
#else
	: protected Allocator
#endif
	{

		template<class T>
		friend class Strong;

		template<class T>
		friend class Weak;

		template<class T>
		friend class ::fart::types::Array;

	private:

		mutable std::atomic<size_t> _retainCount;
		mutable void** _weakReferences;
		mutable size_t _weakReferencesSize;
		mutable size_t _weakReferencesCount;
		mutable Mutex _mutex;

		void addWeakReference(void* weakReference) const {
			_mutex.locked([this,weakReference]() {
				if (_weakReferencesSize < _weakReferencesCount + 1) {
					_weakReferencesSize = calculateBufferLength(_weakReferencesCount + 1);
					_weakReferences = (void**)realloc(_weakReferences, _weakReferencesSize);
					_weakReferences[_weakReferencesCount++] = weakReference;
				}
			});
		}

		void removeWeakReference(void* weakReference) const {
			_mutex.locked([this,weakReference]() {
				for (size_t idx = 0 ; idx < _weakReferencesCount ; idx++) {
					if (_weakReferences[idx] == weakReference) {
						for (size_t midx = idx ; midx <= _weakReferencesCount - 1 ; midx++) {
							_weakReferences[midx] = _weakReferences[midx + 1];
						}
						_weakReferencesCount--;
					}
				}
			});
		}

	public:

		Object() : _retainCount(0), _weakReferences(nullptr), _weakReferencesSize(0), _weakReferencesCount(0) {}

		Object(const Object& other) : Object() {}
		Object(Object&& other) : Object() { }

		virtual ~Object() {
			assert(_retainCount == 0);
			for (size_t idx = 0 ; idx < _weakReferencesCount ; idx++) {
				((Weak<Object>*)_weakReferences[0])->_object = nullptr;
			}
		}

		Object& operator=(const Object&) {
			return *this;
		}

		Object& operator=(Object&& other) {
			return *this;
		}

#ifdef FART_ALLOW_MANUAL_HEAP
	public:
#else
	private:
#endif

		void retain() const {
			this->_retainCount++;
		}

		void release() const {
			if (--this->_retainCount == 0) delete this;
		}

		size_t retainCount() const {
			return this->_retainCount;
		}

	};

}

#endif /* object_hpp */
