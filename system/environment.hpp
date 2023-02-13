//
//  environment.hpp
//  fart
//
//  Created by Kristian Trenskow on 2021/01/16.
//  Copyright © 2018-2023 Kristian Trenskow. All rights reserved.
//

#ifndef environment_hpp
#define environment_hpp

#include <stdlib.h>

#include "../types/string.hpp"

namespace fart::system {

	class Environment {

	public:
		static String getVariable(const String& name) {
			return String(name.mapCString<const char*>([](const char* name) {
				return getenv(name);
			}));
		}

	};

}

#endif /* environment_hpp */
