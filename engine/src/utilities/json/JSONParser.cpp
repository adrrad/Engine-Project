#include "utilities/json/JSONParser.hpp"

#include "utilities/StringUtilities.hpp"

#include <iostream>
#include <cassert>

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
            pos += 3;
            column += 3;
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

JSONValue* JSONParser::ParseArray()
{
    AssertToken(LEFT_SQRBRACKET);
    std::vector<JSONValue*> values;
    if(PeekNextToken() == RIGHT_SQRBRACKET)
    {
        PopNextToken();
        return new JSONValue(values);
    }
    do
    {
        values.push_back(ParseValue());
        PopNextToken();
    }
    while(token == COMMA);
    AssertToken(RIGHT_SQRBRACKET);
    return new JSONValue(values);
}

JSONValue* JSONParser::ParseValue()
{
    PopNextToken();
    switch(token)
    {
        case NUMBER:
            return new JSONValue(std::stod(PopNextValue()));
        case TRUE:
            return new JSONValue(true);
        case FALSE:
            return new JSONValue(false);
        case STRING:
            return new JSONValue(PopNextValue());
        case JSON_NULL:
            return new JSONValue();
        case LEFT_SQRBRACKET:
            return ParseArray();
        case LEFT_BRACKET:
            return new JSONValue(ParseObject());
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

JSONObject* JSONParser::ParseObject()
{
    AssertToken(LEFT_BRACKET);
    std::unordered_map<std::string, JSONValue*> kvpairs;
    do
    {
        JSONKeyValuePair pair = ParseKeyValuePair();
        kvpairs.insert({pair.Name, pair.Value});
        PopNextToken();
    }
    while(token == COMMA);
    AssertToken(RIGHT_BRACKET);
    return new JSONObject(kvpairs);
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

JSONObject* JSONParser::ParseToObject()
{
    PopNextToken();
    return ParseObject();
}


} // namespace Engine::Utilities::JSON