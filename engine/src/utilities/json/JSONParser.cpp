#include "utilities/json/JSONParser.hpp"

#include "utilities/StringUtilities.hpp"

#include <iostream>
#include <cassert>
#include <memory>

namespace Engine::Utilities::JSON
{

inline bool IsDigit(const char& c)
{
    return c >= '0' && c <= '9';
}

void JSONParser::HandleSymbol()
{
    switch (symbol)
    {
    case '\n':
        line++;
        column = 0;
        break;
    case ' ':
    case '\t':
    case '\r':
        break;
    case '[':
        PushToken(LEFT_SQRBRACKET);
        break;
    case ']':
        PushToken(RIGHT_SQRBRACKET);
        break;
    case '{':
        PushToken(LEFT_BRACKET);
        break;
    case '}':
        PushToken(RIGHT_BRACKET);
        break;
    case ',':
        PushToken(COMMA);
        break;
    case ':':
        PushToken(COLON);
        break;
    case '\"':
    {
        //FIXME: Does not accept '\"'
        std::string value = SplitOnFirst(str.substr(pos+1), "\"", true);
        PushToken(STRING);
        values.push(value);
        pos += uint32_t(value.size()+1);
        column += uint32_t(value.size()+1);
        break;
    }
    default:
        if(ContainsAtIndex(str, "null", pos))
        {
            PushToken(JSON_NULL);
            pos += 3;
            column += 3;
        }
        else if(ContainsAtIndex(str, "true", pos))
        {
            PushToken(TRUE);
            pos += 3;
            column += 3;
        }
        else if(ContainsAtIndex(str, "false", pos))
        {
            PushToken(FALSE);
            pos += 4;
            column += 4;
        } 
        else if(IsDigit(symbol) || symbol == '-')
        {
            std::string value = NextDouble(str, pos);
            PushToken(NUMBER);
            values.push(value);
            pos += uint32_t(value.size()-1);
            column += uint32_t(value.size()-1);
        }
        else
        {
            std::cerr << "JSON lexer error! Unexpected symbol '" << symbol << "' on line " << line+1 << ", column " << column+1 << std::endl;
            throw "Failed";
        }
        break;
    }
}

void JSONParser::Tokenise()
{
    do
    {
        HandleSymbol();
    }
    while(NextSymbol());
}

std::shared_ptr<JSONValue> JSONParser::ParseArray()
{
    AssertToken(LEFT_SQRBRACKET);
    std::vector<std::shared_ptr<JSONValue>> values;
    if(PeekNextToken() == RIGHT_SQRBRACKET)
    {
        PopNextToken();
        return JSONValue::AsArray(values);
    }
    do
    {
        values.push_back(ParseValue());
        PopNextToken();
    }
    while(token == COMMA);
    AssertToken(RIGHT_SQRBRACKET);
    return JSONValue::AsArray(values);
}

std::shared_ptr<JSONValue> JSONParser::ParseValue()
{
    PopNextToken();
    switch(token)
    {
        case NUMBER:
            return JSONValue::AsNumber(std::stod(PopNextValue()));
        case TRUE:
            return JSONValue::AsBoolean(true);
        case FALSE:
            return JSONValue::AsBoolean(false);
        case STRING:
            return JSONValue::AsString(PopNextValue());
        case JSON_NULL:
            return JSONValue::AsNull();
        case LEFT_SQRBRACKET:
            return ParseArray();
        case LEFT_BRACKET:
            return ParseObject();
        default:
            UnexpectedTokenError();
    }
}

JSONKeyValuePair JSONParser::ParseKeyValuePair()
{
    PopNextToken();
    AssertToken(STRING);
    auto name = PopNextValue();
    PopNextToken();
    AssertToken(COLON);
    auto value = ParseValue();
    return {name, value};
}

std::shared_ptr<JSONValue> JSONParser::ParseObject()
{
    AssertToken(LEFT_BRACKET);
    std::vector<JSONKeyValuePair> kvpairs;
    do
    {
        kvpairs.push_back(ParseKeyValuePair());
        PopNextToken();
    }
    while(token == COMMA);
    AssertToken(RIGHT_BRACKET);
    return std::make_shared<JSONValue>(kvpairs);
}


JSONParser::JSONParser(std::string jsonString)
{
    str = jsonString;
    pos = 0;
    line = 0;
    column = 0;
    symbol = str.at(pos);
    strSize = uint32_t(str.size());
    Tokenise();
}

std::shared_ptr<JSONValue> JSONParser::ParseToObject()
{
    PopNextToken();
    return ParseObject();
}


} // namespace Engine::Utilities::JSON