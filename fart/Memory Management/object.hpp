//
//  object.hpp
//  fart
//
//  Created by Kristian Trenskow on 17/08/2018.
//  Copyright © 2018 Kristian Trenskow. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include "mutex.hpp"
#include "exception.hpp"

#include <unistd.h>

namespace fart {
    
    using namespace threading;
    using namespace exceptions::memory;
    
    namespace memory {
        
        class Object {
            
            template<class T>
            friend class Strong;
            
            template<class T>
            friend class Weak;

        private:
            mutable size_t _retainCount;
            mutable void** _weakReferences;
            mutable size_t _weakReferencesSize;
            mutable size_t _weakReferencesCount;
            mutable Mutex _mutex;
            
            void *operator new(size_t size) throw(AllocationException);
            
            void addWeakReference(void* weakReference) const;
            void removeWeakReference(void* weakReference) const;
            
        protected:
            void operator delete(void *ptr) throw();
            
        public:
            
            Object();
            Object(const Object& other);
            virtual ~Object();
            
            void retain() const;
            void release() const;
            
            size_t getRetainCount() const;
            
        };
        
    }
    
}

#endif /* object_hpp */
