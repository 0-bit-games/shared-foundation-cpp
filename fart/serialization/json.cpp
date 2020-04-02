//
//  json.cpp
//  fart
//
//  Created by Kristian Trenskow on 01/04/2020.
//  Copyright © 2020 Kristian Trenskow. All rights reserved.
//

#include <cmath>
#include <sstream>

#include "../exceptions/exception.hpp"
#include "../types/number.hpp"
#include "../types/null.hpp"
#include "../types/string.hpp"
#include "../types/dictionary.hpp"

#include "json.hpp"

using namespace fart::serialization;
using namespace fart::exceptions::serialization;

Strong<Type> _parse(const String& string, size_t* idx, size_t* line, size_t* character);
Strong<Type> _parseString(const String& string, size_t* idx, size_t* line, size_t* character);

void _ensureLength(const String& string, size_t* idx, size_t length) {
    if (string.getLength() < *idx + length) {
        throw JSONUnexpectedEndOfDataException();
    }
}

void _ensureData(const String& string, size_t* idx) {
    return _ensureLength(string, idx, 1);
}

void _offsetWhiteSpaces(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    // First we ignore any whitespaces.
    while (string.getLength() > *idx && (string[*idx] == 0x20 || string[*idx] == 0x09 || string[*idx] == 0x0A || string[*idx] == 0x0D)) {
        if (string[*idx] == 0x0A) {
            (*line)++;
            (*character) = 0;
        } else {
            (*character)++;
        }
        (*idx)++;
    }
    
    _ensureData(string, idx);
    
}

Strong<Type> _parseDictionary(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    if (string[*idx] != '{') throw JSONMalformedException(*line, *character);

    (*idx)++;
    (*character)++;
    
    _ensureData(string, idx);

    Strong<Dictionary<String, Type>> result;
    
    while (true) {
        _offsetWhiteSpaces(string, idx, line, character);
        if (string[*idx] == '}') break;
        if (string[*idx] == ',') {
            if (result->getCount() == 0) throw JSONMalformedException(*line, *character);
            (*idx)++;
            (*character)++;
            _offsetWhiteSpaces(string, idx, line, character);
        }
        Strong<String> key = _parseString(string, idx, line, character).as<String>();
        _offsetWhiteSpaces(string, idx, line, character);
        if (string[*idx] != ':') throw JSONMalformedException(*line, *character);
        (*idx)++;
        (*character)++;
        _offsetWhiteSpaces(string, idx, line, character);
        Strong<Type> value = _parse(string, idx, line, character);
        result->set(key, value);
        _offsetWhiteSpaces(string, idx, line, character);
        if (string[*idx] != ',' && string[*idx] != '}') throw JSONMalformedException(*line, *character);
    }
    
    (*idx)++;
    (*character)++;
        
    return result.as<Type>();
    
}

Strong<Type> _parseArray(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    if (string[*idx] != '[') throw JSONMalformedException(*line, *character);
    
    (*idx)++;
    (*character)++;
    
    _ensureData(string, idx);
    
    Strong<Array<Type>> result;
    
    while (true) {
        _offsetWhiteSpaces(string, idx, line, character);
        if (string[*idx] == ']') break;
        if (string[*idx] == ',') {
            if (result->getCount() == 0) throw JSONMalformedException(*line, *character);
            (*idx)++;
            (*character)++;
            _offsetWhiteSpaces(string, idx, line, character);
        } else if (result->getCount() > 0) throw JSONMalformedException(*line, *character);
        result->append(_parse(string, idx, line, character));
        _offsetWhiteSpaces(string, idx, line, character);
        if (string[*idx] != ',' && string[*idx] != ']') throw JSONMalformedException(*line, *character);
    }
    
    (*idx)++;
    (*character)++;
        
    return result.as<Type>();
    
}

Strong<Type> _parseNumber(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    size_t consumed;
    
    double multiplier = 1;
    
    if (string[*idx] == '-') {
        multiplier = -1;
        _ensureLength(string, idx, 1);
        (*idx)++;
        (*character)++;
    }

    double full = string.parseNumber(*idx, &consumed);
    double fragment = 0;
    double exponent = 1;
    
    (*idx) += consumed;
    (*character) += consumed;;

    if (string[*idx] == '.') {
        _ensureLength(string, idx, 1);
        (*idx)++;
        (*character)++;
        fragment = string.parseNumber(*idx, &consumed);
        (*idx) += consumed;
        (*character) += consumed;
        while (fragment > 1) {
            fragment /= 10;
        }
    }
    
    if (string[*idx] == 'e' || string[*idx] == 'E') {
        _ensureLength(string, idx, 1);
        (*idx)++;
        (*character)++;
        exponent = pow(10, string.parseNumber(*idx, &consumed));
        (*idx) += consumed;
        (*character) += consumed;
    }
    
    const double value = (full + fragment) * exponent * multiplier;
    
    if (fragment == 0) {
        return Strong<Integer>(value).as<Type>();
    }
    
    return Strong<Number<double>>(value).as<Type>();
    
}

Strong<Type> _parseString(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    if (string[*idx] != '"') throw JSONMalformedException(*line, *character);
    
    (*idx)++;
    (*character)++;
    
    Data<uint16_t> stringBytes;

    while (string.getLength() > *idx && string[*idx] != '"') {
        
        switch (string[*idx]) {
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                throw JSONMalformedException(*line, *character);
            case '\\':
                (*idx)++;
                (*character)++;
                _ensureData(string, idx);
                switch (string[*idx]) {
                    case 'b':
                        stringBytes.append('\b');
                        break;
                    case 'f':
                        stringBytes.append('\f');
                        break;
                    case 'n':
                        stringBytes.append('\n');
                        break;
                    case 'r':
                        stringBytes.append('\r');
                        break;
                    case 't':
                        stringBytes.append('\t');
                        break;
                    case '"':
                        stringBytes.append('"');
                        break;
                    case '\\':
                        stringBytes.append('\\');
                        break;
                    case '/':
                        stringBytes.append('/');
                        break;
                    case 'U': {
                        _ensureLength(string, idx, 4);
                        String code = string.substring(*idx + 1, 4);
                        stringBytes.append(Endian::toSystemVariant(code.getHexData()->to<uint16_t>()->getItemAtIndex(0), Endian::Variant::big));
                        (*idx) += 4;
                        (*character) += 4;
                        break;
                    }
                    default:
                        throw JSONMalformedException(*line, *character);
                }
                break;
            default:
                stringBytes.append(string[*idx]);
        }
        
        (*idx)++;
        (*character)++;

    }
    
    (*idx)++;
    (*character)++;
    
    return Strong<String>(stringBytes, Endian::getSystemVariant()).as<Type>();
    
}

Strong<Type> _parseLiteral(const String& string, size_t* idx, size_t* line, size_t* character) {
    
    static const String trueLiteral = "true";
    static const String falseLiteral = "false";
    static const String nullLiteral = "null";
    
    switch (string[*idx]) {
        case 't':
            _ensureLength(string, idx, 4);
            if (*string.substring(*idx, 4) != trueLiteral) {
                throw JSONMalformedException(*line, *character);
            }
            (*idx) += 4;
            (*character) += 4;
            return Strong<Boolean>(true).as<Type>();
        case 'f':
            _ensureLength(string, idx, 5);
            if (*string.substring(*idx, 5) != falseLiteral) {
                throw JSONMalformedException(*line, *character);
            }
            (*idx) += 5;
            (*character) += 5;
            return Strong<Boolean>(false).as<Type>();
        case 'n':
            _ensureLength(string, idx, 4);
            if (*string.substring(*idx, 4) != nullLiteral) {
                throw JSONMalformedException(*line, *character);
            }
            (*idx) += 4;
            (*character) += 4;
            return Strong<Null>().as<Type>();
        default:
            throw JSONMalformedException(*line, *character);
    }
}

Strong<Type> _parse(const String& string, size_t* idx, size_t* line, size_t* character) {
    _offsetWhiteSpaces(string, idx, line, character);
    uint32_t chr = string[*idx];
    switch (chr) {
        case '{':
            return _parseDictionary(string, idx, line, character);
        case '[':
            return _parseArray(string, idx, line, character);
        case '"':
            return _parseString(string, idx, line, character);
        case 'n':
        case 't':
        case 'f':
            return _parseLiteral(string, idx, line, character);
            break;
        default:
            if (chr == '-' || (chr >= '0' && chr <= '9')) {
                return _parseNumber(string, idx, line, character);
            }
            throw JSONMalformedException(*line, *character);
    }
}

Strong<Type> JSON::parse(const String& string) {
    size_t idx = 0;
    size_t line = 0;
    size_t character = 0;
    return _parse(string, &idx, &line, &character);
}

Strong<String> JSON::stringify(const Type &data) {
    
    Strong<String> result;
    
    switch (data.getKind()) {
        case types::Type::Kind::dictionary: {
            result->append("{");
            auto dictionary = data.as<Dictionary<Type, Type>>();
            result->append(String::join(dictionary.getKeys()->map<String>([dictionary](Type& key) {
                if (key.getKind() != types::Type::Kind::string) throw EncoderTypeException();
                Strong<String> result;
                result->append(stringify(key));
                result->append(":");
                result->append(stringify(dictionary.get(key)));
                return result;
            }), ","));
            result->append("}");
            break;
        }
        case types::Type::Kind::array: {
            Array<Type>& array = data.as<Array<Type>>();
            result->append("[");
            result->append(String::join(array.map<String>([](Type& item) {
                return stringify(item);
            }), ","));
            result->append("]");
            break;
        }
        case types::Type::Kind::string: {
            auto bytes = data.as<String>().getUTF16Data(Endian::getSystemVariant());
            result->append("\"");
            for (size_t idx = 0 ; idx < bytes->getCount() ; idx++) {
                auto byte = bytes->getItemAtIndex(idx);
                switch (byte) {
                    case '\b':
                        result->append("\\b");
                        break;
                    case '\f':
                        result->append("\\f");
                        break;
                    case '\n':
                        result->append("\\n");
                        break;
                    case '\r':
                        result->append("\\r");
                        break;
                    case '\t':
                        result->append("\\t");
                        break;
                    case '\"':
                        result->append("\\\"");
                        break;
                    case '\\':
                        result->append("\\\\");
                        break;
                    default:
                        if (!((byte >= 0x20 && byte <= 0x21) || (byte >= 0x23 && byte <= 0x5B) || byte >= 0x5D)) {
                            uint16_t beByte = Endian::fromSystemVariant(byte, system::Endian::big);
                            auto beByteData = Data<uint16_t>(&beByte, 1).to<uint8_t>();
                            result->append("\\U");
                            result->append(String::fromHex(beByteData));
                        } else {
                            result->append(byte);
                        }
                        break;
                }
            }
            result->append("\"");
            break;
        }
        case types::Type::Kind::number: {
            switch (data.as<Number<uint64_t>>().getSubType()) {
                case types::boolean:
                    result->append(data.as<Boolean>().getValue() ? "true" : "false");
                    break;
                case types::integer:
                    result->append(String::format("%lld", data.as<Integer>().getValue()));
                    break;
                case types::floatingPoint: {
                    double value = data.as<Float>().getValue();
                    std::ostringstream stream;
                    stream << value;
                    result->append(stream.str().c_str());
                    break;
                }
            }
            break;
        }
        case types::Type::Kind::data:
            throw EncoderTypeException();
            break;
        case types::Type::Kind::null:
            result->append("null");
            break;
    }
    
    return result;
    
}
