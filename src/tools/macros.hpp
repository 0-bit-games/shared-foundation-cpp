//
// macros.hpp
// foundation
//
// Created by Kristian Trenskow on 2026/01/01.
// See license in LICENSE.
//

#if defined(_WIN32)
#define CONSTRUCT_IN_PLACE(PLACE, TYPE, ...) noexcept(new (&PLACE) TYPE(__VA_ARGS__))
#else
#define CONSTRUCT_IN_PLACE(PLACE, TYPE, ...) new (&PLACE) TYPE(__VA_ARGS__)
#endif
