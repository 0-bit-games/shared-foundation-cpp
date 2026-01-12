//
// equatable.hpp
// foundation
//
// Created by Kristian Trenskow on 2026/01/12.
// See license in LICENSE.
//

#ifndef foundation_equatable_hpp
#define foundation_equatable_hpp

namespace foundation::types {

	template<typename T>
	class Equatable {

		public:

			virtual bool equals(const T& other) const = 0;

	};

}

#endif /* foundation_equatable_hpp */
