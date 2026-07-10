//
// comparison.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/01/22
// See license in LICENSE.
//

#ifndef foundation_comparison_hpp
#define foundation_comparison_hpp

namespace foundation::types {

	enum class Comparison : int8_t {
		lessThan = -1,
		equals = 0,
		biggerThan = 1
	};

};

#endif /* foundation_comparison_hpp */
