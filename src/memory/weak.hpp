//
// weak.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/01/22
// See license in LICENSE.
//

#ifndef foundation_weak_hpp
#define foundation_weak_hpp

#include <type_traits>
#include <atomic>

#include "../parallelism/mutex.hpp"

#include "./allocator.hpp"

namespace foundation::memory {

	class Object;

	template<typename T>
	class Strong;

	template<typename T = Object>
	class Weak : public Allocator
	{

		friend class Object;

		template<typename U>
		friend class Weak;

	private:
		std::atomic<T*> _object;

		void _setObjectLocked(T* object) {

			if (object != nullptr) {
				object->addWeakReference(this);
			}

			T* previous = _object.load();

			_object = object;

			if (previous != nullptr) {
				previous->removeWeakReference(this);
			}

		}

		void _setObject(T* object) {
			T::weakReferencesMutex().locked([&]() {
				_setObjectLocked(object);
			});
		}

		void _copyFrom(const Weak<T>& other) {
			T::weakReferencesMutex().locked([&]() {
				_setObjectLocked(other._object.load());
			});
		}

		void _moveFrom(Weak<T>& other) {
			T::weakReferencesMutex().locked([&]() {
				T* object = other._object.load();
				_setObjectLocked(object);
				other._setObjectLocked(nullptr);
			});
		}

	public:
		Weak() : Weak(nullptr) {
			static_assert(std::is_base_of<Object, T>::value);
		}

		Weak(T& object) : Weak(&object) {
			static_assert(std::is_base_of<Object, T>::value);
		}

		Weak(T* object) : _object(nullptr) {

			static_assert(std::is_base_of<Object, T>::value);

			_setObject(object);

		}

		Weak(std::nullptr_t) : _object(nullptr) {

			static_assert(std::is_base_of<Object, T>::value);

			_setObject(nullptr);

		}

		Weak(const Strong<T>& other) : _object(nullptr) {

			static_assert(std::is_base_of<Object, T>::value);

			_setObject(other.operator T*());

		}

		Weak(const Weak<T>& other) : _object(nullptr) {

			static_assert(std::is_base_of<Object, T>::value);

			_copyFrom(other);

		}

		Weak(Weak<T>&& other) : _object(nullptr) {

			static_assert(std::is_base_of<Object, T>::value);

			_moveFrom(other);

		}

		~Weak() {
			_setObject(nullptr);
		}

		bool isNull() const {
			return _object == nullptr;
		}

		operator T&() const {
			return *_object;
		}

		operator T*() const {
			return _object;
		}

		Weak<T>& operator =(T& object) {
			_setObject(&object);
			return *this;
		}

		Weak<T>& operator =(const Weak<T>& other) {
			if (this == &other) return *this;
			_copyFrom(other);
			return *this;
		}

		Weak<T>& operator =(Weak<T>&& other) {
			if (this == &other) return *this;
			_moveFrom(other);
			return *this;
		}

		Weak<T>& operator =(T* object) {
			_setObject(object);
			return *this;
		}

		Weak<T>& operator =(const Strong<T>& object) {
			_setObject(object.operator T*());
			return *this;
		}

		T* operator ->() const {
			return _object;
		}

		bool operator==(std::nullptr_t) const = delete;
		bool operator!=(std::nullptr_t) const  = delete;

		bool equals(std::nullptr_t) const {
			return this->_object == nullptr;
		}

		template<typename O>
		Weak<O> as() const {
			Weak<O> result;
			T::weakReferencesMutex().locked([&]() {
				result._setObjectLocked((O*)_object.load());
			});
			return result;
		}

	};

}

#endif /* foundation_weak_hpp */
