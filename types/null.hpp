//
// null.hpp
// fart
//
// Created by Kristian Trenskow on 2020/04/01.
// See license in LICENSE.
//

#ifndef null_hpp
#define null_hpp

#include "type.hpp"

namespace fart::types {

	class Null: public Type {

	public:

		Null() : Type() { }
		Null(nullptr_t) : Type() { }

		static Type::Kind typeKind() {
			return Type::Kind::null;
		}

		virtual Kind kind() const override {
			return Kind::null;
		}

		virtual bool operator==(const Type& other) const override {
			return other.kind() == Kind::null;
		}

	};

}

#endif /* null_hpp */
