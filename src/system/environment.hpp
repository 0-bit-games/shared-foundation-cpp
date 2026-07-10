//
// environment.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/01/22
// See license in LICENSE.
//

#ifndef foundation_environment_hpp
#define foundation_environment_hpp

#include <stdlib.h>

#include "../types/string.hpp"

using namespace foundation::types;

namespace foundation::system {

	class Environment {

	public:
		static String getVariable(
			const String& name);

	};

}

#endif /* foundation_environment_hpp */
