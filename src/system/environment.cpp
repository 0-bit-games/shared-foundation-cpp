//
// environment.cpp
// foundation
//
// Created by Kristian Trenskow on 2025/01/22.
// See license in LICENSE.
//

#include "environment.hpp"

using namespace foundation::system;

String Environment::getVariable(const String& name) {
#ifdef _WIN32
	return name.mapCString<String>([](const char* name) {
		char* buffer = nullptr;
		size_t size = 0;
		if (_dupenv_s(&buffer, &size, name) == 0 && buffer != nullptr) {
			String result(buffer);
			free(buffer);
			return result;
		}
		return String();
	});
#else
	return String(name.mapCString<const char*>([](const char* name) {
		return getenv(name);
	}));
#endif
}