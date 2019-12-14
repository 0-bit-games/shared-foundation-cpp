//
//  socket.hpp
//  fart
//
//  Created by Kristian Trenskow on 19/09/2018.
//  Copyright © 2018 Kristian Trenskow. All rights reserved.
//

#ifndef Socket_hpp
#define Socket_hpp

#include <thread>

#include "../memory/object.hpp"
#include "../memory/strong.hpp"
#include "../memory/weak.hpp"
#include "../threading/thread.hpp"
#include "../threading/mutex.hpp"
#include "../types/data.hpp"
#include "../types/data.hpp"
#include "./endpoint.hpp"

using namespace fart::memory;
using namespace fart::threading;
using namespace std;

namespace fart::network {
    
    enum SocketState {
        SocketStateClosed = 0,
        SocketStateListening,
        SocketStateConnected
    };
    
    class Socket : public Object, public Hashable {
        
    private:
        
        bool _isUDP;
        
        int _socket;
        SocketState _state;
        
        Strong<Endpoint> _localEndpoint;
        Strong<Endpoint> _remoteEndpoint;
                    
        Thread _listenThread;
        Thread _receiveThread;
        
        Mutex _mutex;
        
        void _read(function<void()> setup, function<void(Strong<Data<uint8_t>>, Strong<Endpoint> endpoint)> readCallback);
        
    public:
        
        Socket(int socket);
        Socket(bool isUDP = false);
        virtual ~Socket();
        
        virtual const uint64_t getHash() const;
        
        void bind(Strong<Endpoint> endpoint);
        void listen(function<void(Strong<Socket> incoming)> acceptCallback);
        void accept(function<void(Strong<Data<uint8_t>>, Strong<Endpoint>)> readCallback);
        void connect(Strong<Endpoint> endpoint, function<void(Strong<Data<uint8_t>>, Strong<Endpoint>)> readCallback);
        const size_t send(Strong<Data<uint8_t>> data) const;
        const size_t sendTo(Strong<Endpoint> endpoint, Strong<Data<uint8_t>> data) const;
        void close();
        
        Strong<Endpoint> getLocalEndpoint() const;
        Strong<Endpoint> getRemoteEndpoint() const;
        
        const bool getIsUDP() const;
        const SocketState getSocketState() const;
        
        void awaitClose() const;
        
    };

}

#endif /* socket_hpp */
