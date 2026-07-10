//
// cloneable.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/03/03
// See license in LICENSE.
//

#ifndef foundation_cloneable_hpp
#define foundation_cloneable_hpp

#include "../memory/strong.hpp"

#include "./comparison.hpp"

using namespace foundation::memory;

namespace foundation::types {

	template<typename T>
	class Cloneable {

		public:

			virtual Strong<T> clone() const = 0;

	};

}

#endif /* foundation_cloneable_hpp */
