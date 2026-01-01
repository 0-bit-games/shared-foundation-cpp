//
// object.hpp
// foundation
//
// Created by Kristian Trenskow on 2018/08/17.
// See license in LICENSE.
//

#ifndef foundation_object_hpp
#define foundation_object_hpp

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <atomic>

#include "./allocator.hpp"

namespace foundation::memory {

	class Object : public Allocator {

		template<typename T>
		friend class Strong;

		template<typename T>
		friend class Weak;

	private:

#if defined(_WIN32)
		mutable size_t _retainCount;
#else
		mutable std::atomic<size_t> _retainCount;
#endif
		mutable void** _weakReferences;
		mutable size_t _weakReferencesSize;
		mutable size_t _weakReferencesCount;

		void addWeakReference(void* weakReference) const;

		void removeWeakReference(void* weakReference) const;

	public:

		virtual ~Object();

		Object& operator=(
			const Object&);

		Object& operator=(
			Object&&);

		void retain() const;
		void release() const;

		size_t retainCount() const;

	protected:

		Object();

		Object(
			const Object&);

		Object(
			Object&&);

	};

}

#endif /* foundation_object_hpp */
