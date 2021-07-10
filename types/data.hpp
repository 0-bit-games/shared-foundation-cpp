//
//  data.hpp
//  fart
//
//  Created by Kristian Trenskow on 17/08/2018.
//  Copyright © 2018 Kristian Trenskow. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../exceptions/exception.hpp"
#include "../memory/strong.hpp"
#include "./type.hpp"
#include "./array.hpp"
#include "../tools/math.hpp"

using namespace fart::memory;
using namespace fart::exceptions::types;
using namespace fart::tools;

namespace fart::types {

	class String;

	enum class IncludeSeparator {
		none,
		prefix,
		suffix,
		both
	};

	template<typename T = uint8_t>
	class Data : public Type {

	public:

		class Value: public Data {

		public:

			Value() : Data() {}
			Value(const T* items, size_t count) : Data(items, count) {}
			Value(size_t capacity) : Data(capacity) {}
			Value(const Value& other) : Data(other) {}
			Value(const Data& other) : Data(other) {}

			virtual ~Value() {}

		protected:

			virtual inline uint64_t hashForItem(const T& item) const override {
				return item;
			}

		};

		typedef function<bool(T item1, T item2)> Comparer;
		typedef function<bool(T item)> Tester;
		typedef function<bool(T item, const size_t idx)> TesterIndex;

		static const size_t blockSize = 4096;

		template<typename F>
		static Strong<Data<T>> fromCBuffer(const F& todo, size_t count = blockSize) {
			T buffer[count];
			size_t length = todo(buffer, count);
			return Strong<Data<T>>((T*)buffer, length);
		}

		Data(const T* items, size_t count) : Type(), _store(new Store()) {
			append(items, count);
		}

		Data() : Data(nullptr, 0) {}

		Data(size_t capacity) : Data() {
			this->_ensureStoreSize(capacity);
		}

		Data(size_t count, ...) : Data() {
			va_list args;
			va_start(args, count);
			for (size_t idx = 0 ; idx < count ; idx++) {
				append(va_arg(args, T));
			}
			va_end(args);
		}

		Data(const Data<T>& other) : _store(other._store->retain()) { }

		Data(Data<T>&& other) : Type(std::move(other)) {
			if (this->_store != nullptr) {
				Store::release(&this->_store);
			}
			this->_store = other._store;
			other->_store = nullptr;
		}

		virtual ~Data() {
			Store::release(&this->_store);
		}

		void append(const T* items, size_t count) {
			if (!count) return;
			this->insertItemsAtIndex(items, count, this->count());
		}

		inline void append(T element) {
			append(&element, 1);
		}

		void append(const Data<T>& data) {
			append(data.items(), data.count());
		}

		Strong<Data> appending(const Data<T>& other) const {
			Strong<Data> result = *this;
			result->append(other);
			return result;
		}

		const T removeItemAtIndex(size_t index) noexcept(false) {

			if (index >= this->count()) throw OutOfBoundException(index);

			this->_ensureStoreOwnership();

			T element = _store->pointer[index];

			for (size_t idx = index ; idx < this->count() - 1 ; idx++) {
				_store->pointer[idx] = _store->pointer[idx + 1];
			}

			this->_store->count--;
			_store->hashIsDirty = true;

			return element;

		}

		void moveItemAtIndex(size_t srcIndex, size_t dstIndex) noexcept(false) {

			if (srcIndex == dstIndex) return;

			if (srcIndex >= this->count()) throw OutOfBoundException(srcIndex);
			if (dstIndex >= this->count()) throw OutOfBoundException(dstIndex);

			this->ensureStoreOwnership();

			T src = _store->pointer[srcIndex];

			if (srcIndex < dstIndex) {
				for (size_t idx = srcIndex + 1 ; idx <= dstIndex ; idx++) {
					_store->pointer[idx - 1] = _store->pointer[idx];
				}
			} else {
				for (size_t idx = srcIndex ; idx > dstIndex ; idx--) {
					_store->pointer[idx] = _store->pointer[idx - 1];
				}
			}

			_store[dstIndex] = src;

		}

		void swapItemsAtIndexes(size_t index1, size_t index2) noexcept(false) {

			if (index1 == index2) return;

			if (index1 >= this->count()) throw OutOfBoundException(index1);
			if (index2 >= this->count()) throw OutOfBoundException(index2);

			this->ensureStoreOwnership();

			T reg = _store->pointer[index1];
			_store->pointer[index1] = _store->pointer[index2];
			_store->pointer[index2] = reg;

		}

		void insertItemsAtIndex(const T* items, size_t count, size_t dstIndex) noexcept(false) {

			if (dstIndex > this->count()) throw OutOfBoundException(dstIndex);

			this->_ensureStoreSize(this->count() + count);

			for (size_t idx = this->count() ; idx > dstIndex ; idx--) {
				_store->pointer[idx + count - 1] = _store->pointer[idx - 1];
			}

			for (size_t idx = 0 ; idx < count ; idx++) {
				_store->pointer[idx + dstIndex] = items[idx];
			}

			this->_store->count += count;

		}

		void insertItemAtIndex(T item, size_t dstIndex) noexcept(false) {
			this->insertItemsAtIndex(&item, 1, dstIndex);
		}

		size_t count() const {
			if (this->_store == nullptr) return 0;
			return this->_store->count;
		}

		T itemAtIndex(const size_t index) const noexcept(false) {
			if (index >= this->count()) throw OutOfBoundException(index);
			return _store->pointer[index];
		}

		const T* items() const {
			return _store->pointer;
		}

		T operator[](const size_t index) const noexcept(false) {
			return itemAtIndex(index);
		}

		T first() const noexcept(false) {
			if (this->count() == 0) throw NotFoundException();
			return this->_store->pointer[0];
		}

		T last() const noexcept(false) {
			if (this->count() == 0) throw NotFoundException();
			return this->_store->pointer[this->count() - 1];
		}

		size_t indexOf(const Data<T>& other, const size_t offset = 0) const {
			for (size_t hidx = offset ; hidx < this->count() ; hidx++) {
				bool found = true;
				for (size_t nidx = 0 ; nidx < other.count() ; nidx++) {
					if (this->_store->pointer[hidx + nidx] != other._store->pointer[nidx]) {
						found = false;
						break;
					}
				}
				if (found) return hidx;
			}
			return NotFound;
		}

		size_t indexOf(const T other, const size_t offset = 0) const {
			return indexOf(Data<T>(&other, 1), offset);
		}

		T replace(T element, const size_t index) {
			this->_ensureStoreOwnership();
			T removed = _store->pointer[index];
			_store->pointer[index] = element;
			return removed;
		}

		Strong<Data<T>> subdata(const size_t offset, const size_t length = NotFound) const {
			return Strong<Data<T>>(&_store->pointer[offset], math::min(this->count() - offset, length));
		}

		Strong<Data<T>> remove(const size_t offset, const size_t length) {

			if (offset + length > this->count()) throw OutOfBoundException(offset + length);

			this->_ensureStoreOwnership();

			Strong<Data<uint8_t>> result(&_store->pointer[offset], length);

			size_t moveCount = this->count() - (offset + length);

			for (size_t idx = 0 ; idx < moveCount ; idx++) {
				_store->pointer[offset + idx] = _store->pointer[offset + length + idx];
			}

			this->_store->count -= length;

			return result;

		}

		Strong<Data<T>> reversed() const {
			Strong<Data<T>> result;
			for (size_t idx = this->count() ; idx > 0 ; idx--) {
				result->append(this->_store->pointer[idx - 1]);
			}
			return result;
		}

		void drain() {
			Store::release(&this->_store);
			_store = new Store();
		};

		size_t copy(void* bytes, size_t count, size_t offset = 0) {
			if (offset > this->count()) return 0;
			this->ensureStoreOwnership();
			count = math::min(count, this->count() - offset);
			memcpy(bytes, _store->pointer, sizeof(T) * count);
			return count;
		}

		Strong<Array<Data<T>>> split() const {
			return this->mapToArray<Data<T>>([](T item) {
				return Strong<Data<T>>(&item, 1);
			});
		}

		Strong<Array<Data<T>>> split(const Array<Data<T>>& separators, IncludeSeparator includeSeparator = IncludeSeparator::none, size_t max = 0) const {
			Strong<Array<Data<T>>> result;
			size_t idx = 0;
			while (result->count() < max - 1) {
				if (!separators.some([this,&idx,&result,&includeSeparator](const Data<T>& separator) {
					size_t next;
					switch (includeSeparator) {
						case IncludeSeparator::none:
						case IncludeSeparator::suffix:
							next = indexOf(separator, idx);
							break;
						case IncludeSeparator::prefix:
						case IncludeSeparator::both:
							next = indexOf(separator, idx + separator.count());
							break;
					}
					if (next == NotFound) return false;
					switch (includeSeparator) {
						case IncludeSeparator::none:
						case IncludeSeparator::prefix:
							result->append(subdata(idx, next - idx));
							break;
						case IncludeSeparator::suffix:
						case IncludeSeparator::both:
							result->append(subdata(idx, next - idx + separator.count()));
							break;
					}
					switch (includeSeparator) {
						case IncludeSeparator::none:
						case IncludeSeparator::suffix:
							idx = next + separator.count();
							break;
						case IncludeSeparator::prefix:
						case IncludeSeparator::both:
							idx = next;
							break;
					}
					return true;
				})) break;
			}
			result->append(subdata(idx, count() - idx));
			return result;
		}

		Strong<Array<Data<T>>> split(Strong<Data<T>> separator, IncludeSeparator includeSeparator = IncludeSeparator::none, size_t max = 0) const {
			return split(Array<Data<T>>(separator, 1), includeSeparator, max);
		}

		Strong<Array<Data<T>>> split(const T* seperator, size_t length, IncludeSeparator includeSeparator = IncludeSeparator::none, size_t max = 0) const {
			return split(Strong<Data<T>>(seperator, length), includeSeparator, max);
		}

		static Strong<Data<T>> join(const Array<Data<T>>& datas, const Data<T>* seperator) {
			return datas.reduce(Strong<Data<T>>(), [datas, seperator](Strong<Data<T>> result, const Data<T>& value, const size_t idx) {
				result->append(datas.itemAtIndex(idx));
				if (seperator != nullptr && idx != datas.count() - 1) result->append(*seperator);
				return result;
			});
		}

		static Strong<Data<T>> join(const Array<Data<T>>& datas) {
			return join(datas, nullptr);
		}

		static Strong<Data<T>> join(const Array<Data<T>>& datas, const Data<T>& seperator) {
			return join(datas, &seperator);
		}

		template<typename O>
		Strong<Data<O>> as() const {
			return Strong<Data<O>>((const O*)this->_store->pointer, (this->count() * sizeof(T)) / sizeof(O));
		}

		void forEach(function<void(T& item)> todo) const {
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				todo(this->_store->pointer[idx]);
			}
		}

		template<typename R, typename F>
		R reduce(R initial, F todo) const {
			R result = initial;
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				result = todo(result, this->_store->pointer[idx], idx);
			}
			return result;
		}

		Strong<Data<T>> filter(TesterIndex test) const {
			Strong<Data<T>> result;
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				if (test(this->_store->pointer[idx], idx)) result->append(this->_store->pointer[idx]);
			}
			return result;
		}

		Strong<Data<T>> filter(Tester test) const {
			return filter([&test](T item, const size_t idx) {
				return test(item);
			});
		}

		template<typename O>
		Strong<Data<O>> map(function<O(T item, const size_t idx)> transform) const {
			Strong<Data<O>> result;
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				result->append(transform(this->_store->pointer[idx], idx));
			}
			return result;
		}

		template<typename O>
		Strong<Data<O>> map(function<O(T item)> transform) const {
			return map<O>([&transform](T item, const size_t idx) {
				return transform(item);
			});
		}

		template<typename O>
		Strong<Array<O>> mapToArray(function<O(T item, const size_t idx)> transform) const {
			Strong<Array<O>> result;
			for (size_t idx = 0 ; idx < this->count(); idx++) {
				result->append(transform(this->_store->pointer[idx], idx));
			}
			return result;
		}

		template<typename O>
		Strong<Array<O>> mapToArray(function<O(T item)> transform) const {
			return mapToArray<O>([&transform](T item, const size_t idx) {
				return transform(item);
			});
		}

		bool some(TesterIndex test, bool def = false) const {
			if (this->count() == 0) return def;
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				if (test(this->_store->pointer[idx], idx)) return true;
			}
			return false;
		}

		bool some(Tester test, bool def = false) const {
			return some([&test](T item, const size_t idx) {
				return test(item);
			}, def);
		}

		bool every(TesterIndex test, bool def = true) const {
			if (this->count() == 0) return def;
			return !this->some([&test](const T item, const size_t idx) {
				return !test(item, idx);
			});
		}

		bool every(Tester test, bool def = true) const {
			return every([&test](T item, const size_t idx) {
				return test(item);
			}, def);
		}

		virtual uint64_t hash() const override {
			if (_store->hashIsDirty) {
				Hashable::Builder builder;
				for (size_t idx = 0 ; idx < this->count() ; idx++) {
					builder.add(this->hashForItem(_store->pointer[idx]));
				}
				_store->hash = builder;
				_store->hashIsDirty = false;
			}
			return _store->hash;
		}

		virtual Kind kind() const override {
			return Kind::data;
		}

		bool operator ==(const Data<T>& other) const {
			if (!Type::operator==(other)) return false;
			if (this->count() != other.count()) return false;
			for (size_t idx = 0 ; idx < this->count() ; idx++) {
				if (_store->pointer[idx] != other.itemAtIndex(idx)) return false;
			}
			return true;
		}

		Data& operator=(const Data<T>& other) {
			Store::release(&this->_store);
			this->_store = other._store->retain();
			Type::operator=(other);
			return *this;
		}

		Data& operator=(Data<T>&& other) {
			Store::release(&this->_store);
			this->_store = other._store;
			other._store = nullptr;
			Type::operator=(std::move(other));
			return *this;
		}

	protected:

		virtual inline uint64_t hashForItem(const T& item) const {
			return 0;
		}

	private:

		struct Store {

		public:
			T* pointer;
			size_t count;
			mutable uint64_t hash;
			mutable bool hashIsDirty;

			Store(size_t count = 0) : pointer(nullptr), count(0), hash(0), hashIsDirty(true), retainCount(1), storeCount(0) {
				this->ensureStoreSize(count);
			}

			~Store() {
				if (this->pointer != nullptr) {
					free(this->pointer);
				}
			}

			Store* retain() const {
				this->retainCount++;
				return (Store*)this;
			}

			static void own(Store** store) {
				if (*store == nullptr) return;
				*store = (*store)->own();
			}

			static void release(Store** store) {
				if (*store == nullptr) return;
				*store = (*store)->release();
			}

			void ensureStoreSize(size_t count) {
				if (this->storeCount < count) {
					this->storeCount = ((((sizeof(T) * count) / blockSize) + 1) * blockSize) / sizeof(T);
					this->pointer = (T*) realloc(this->pointer, sizeof(T) * this->storeCount);
				}
			}

		private:

			Store* own() const {
				if (this->retainCount == 1) return (Store*)this;
				this->release();
				return new Store(*this);
			}

			Store* release() const {
				this->retainCount--;
				if (this->retainCount == 0) {
					delete(this);
				}
				return nullptr;
			}

			mutable std::atomic<size_t> retainCount;
			size_t storeCount;

			Store(const Store& other) : Store() {

				this->ensureStoreSize(other.count);
				this->count = other.count;
				this->hash = other.hash;
				this->hashIsDirty = other.hashIsDirty;

				for (size_t idx = 0 ; idx < this->count ; idx++) {
					this->pointer[idx] = other.pointer[idx];
				}

			}

		};

		Store* _store;

		void _ensureStoreSize(size_t count) {
			if (this->_store == nullptr) this->_store = new Store(count);
			else {
				this->_ensureStoreOwnership();
				this->_store->ensureStoreSize(count);
			}
		}

		void _ensureStoreOwnership() {
			Store::own(&this->_store);
		}

	};

}

#endif /* data_hpp */
