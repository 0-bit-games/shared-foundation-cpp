//
//  strong.hpp
//  fart
//
//  Created by Kristian Trenskow on 17/08/2018.
//  Copyright © 2018 Kristian Trenskow. All rights reserved.
//

#ifndef strong_hpp
#define strong_hpp

#include <type_traits>

#include "./allocator.hpp"
#include "./object.hpp"

using namespace std;

namespace fart::memory {

	template<class T>
	class Weak;

	template<class T>
	class Strong
#ifdef FART_ALLOW_MANUAL_HEAP
	: public Allocator
#else
	: public NoAllocator
#endif
	{

		static_assert(std::is_base_of<Object, T>::value);

	private:

		T* _object;

		void _setObject(T* object, bool newObject = false) {
			if (_object != nullptr) {
				_object->release();
				_object = nullptr;
			}
			if (object) {
				// If object is allocated on the stack, we make a copy on the heap.
				if (!newObject && object->retainCount() == 0) {
					_object = new T(*object);
				} else {
					_object = object;
				}
				_object->retain();
			}
		}

	public:

		Strong(nullptr_t) : _object(nullptr) {};
		Strong(T& object) : Strong(&object) {};

		Strong(const T& object) : Strong(nullptr) {
			_setObject(new T(object), true);
		}

		Strong(T* object) : _object(nullptr) {
			_setObject(object);
		}

		Strong(const Strong<T>& other) : _object(nullptr) {
			_setObject(other._object);
		}

		Strong(Strong<T>&& other) {
			this->_object = other._object;
			other._object = nullptr;
		}

		Strong(const Weak<T>& other) : _object(other) {}

		template<typename... Args>
		explicit Strong(Args&&... args) : _object(nullptr) {
			_setObject(new T(std::forward<Args>(args)...), true);
		}

		~Strong() {
			_setObject(nullptr);
		};

		operator T&() const {
			return *_object;
		}

		operator T*() const {
			return _object;
		}

		Strong<T>& operator =(T& object) {
			_setObject(&object);
			return *this;
		}

		Strong<T>& operator =(T* object) {
			_setObject(object);
			return *this;
		}

		Strong<T>& operator =(Weak<T> object) {
			_setObject(object);
			return *this;
		}

		Strong<T>& operator =(const Strong<T>& object) {
			_setObject(object._object);
			return *this;
		}

		Strong<T>& operator =(Strong<T>&& other) {
			this->_setObject(nullptr);
			this->_object = other._object;
			other._object = nullptr;
			return *this;
		}

		T* operator ->() const {
			return _object;
		}

		bool operator==(std::nullptr_t) {
			return this->_object == nullptr;
		}

		bool operator!=(std::nullptr_t) {
			return !(this->_object == nullptr);
		}

		template<class O>
		Strong<O> as() {
			return Strong<O>((O*)_object);
		}

		template<typename O>
		Strong<O> map(function<const Strong<O>(T& unwrapped)> todo) const {
			if (_object == nullptr) return nullptr;
			return todo(*this);
		}

	};

}

#endif /* strong_hpp */
