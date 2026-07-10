//
// null.hpp
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2025/01/22
// See license in LICENSE.
//

#ifndef foundation_null_hpp
#define foundation_null_hpp

#include "type.hpp"

namespace foundation::types {

	class Null: public Type {

	public:

		static bool is(
			const Type* value);

		Null();

		Null(
			nullptr_t);

		static Type::Kind typeKind();

		virtual Kind kind() const override;

		virtual Strong<Type> clone() const override;

		bool operator==(
			const Type& other
		) const = delete;

		virtual bool equals(
			const Type& other
		) const override;

	};

}

#endif /* foundation_null_hpp */
