//
//  head.cpp
//  fart
//
//  Created by Kristian Trenskow on 14/12/2019.
//  Copyright © 2019 Kristian Trenskow. All rights reserved.
//

#include "./head.hpp"
#include "../message.hpp"

using namespace fart::network::web;
using namespace fart::network::web::http;

Head::Head() {}

Head::Head(Data<uint8_t>& data) {
    
    Data<uint8_t> lineBreak = Message<Head>::dataForLineBreakMode(Message<Head>::determineLineBreakMode(data));
    Data<uint8_t> head = data.remove(0, data.indexOf(lineBreak) + lineBreak.count());
    
    head.remove(head.count() - lineBreak.count(), lineBreak.count());
    
    Array<Data<uint8_t>> parts = head.split((uint8_t*)" ", 1);
    
    if (parts.count() != 3) throw DataMalformedException();
    
    _parts = parts.map<String>([](Data<uint8_t>& current) {
        return String(current);
    });
    
}

Head::Head(const Head& other) : _parts(other._parts) {}

Head::~Head() {}

const Version Head::parseVersion(String &version) {
    if (version == "HTTP/1.0") return HTTP1_0;
    else if (version == "HTTP/1.1") return HTTP1_1;
    throw VersionNotSupportedException();
}

Strong<Data<uint8_t>> Head::versionData(Version version) {
    switch (version) {
        case HTTP1_0:
            return String("HTTP/1.0").UTF8Data();
        case HTTP1_1:
            return String("HTTP/1.1").UTF8Data();
    }
}
