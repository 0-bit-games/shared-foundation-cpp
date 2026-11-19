//
// thread.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/06/26
// See license in LICENSE.
//

#ifndef foundation_parallelism_thread_hpp
#define foundation_parallelism_thread_hpp

#include <functional>

#include "../platform/thread.hpp"

#include "../memory/object.hpp"
#include "../types/string.hpp"

using namespace foundation::memory;
using namespace foundation::types;

namespace foundation::parallelism {

	class Thread: public Object {

		public:

			Thread(
				const String name,
				std::function<void()> function);

			Thread(const Thread&) = delete;
			Thread(Thread&&) = delete;

			virtual ~Thread();

			void detach();

			static uint64_t currentId();

		private:

			PlatformThread _thread;
			bool _detached;

	};

}

#endif // foundation_parallelism_thread_hpp
