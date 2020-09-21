#pragma once
#include "utilities/json/JSON.hpp"

#include <string>
#include <queue>
#include <iostream>

namespace Engine::Utilities::JSON
{

class JSONParser
{
private:
    enum Token {
        LEFT_BRACKET, RIGHT_SQRBRACKET, 
        LEFT_SQRBRACKET, RIGHT_BRACKET, 
        COMMA, COLON, JSON_NULL, NUMBER, 
        TRUE, FALSE, STRING };
    
    inline std::string ToString(Token t)
    {
        switch (t)
        {
        case LEFT_BRACKET:
            return "LEFT_BRACKET";
        case RIGHT_BRACKET:
            return "RIGHT_BRACKET";
        case LEFT_SQRBRACKET:
            return "LEFT_SQRBRACKET";
        case RIGHT_SQRBRACKET:
            return "RIGHT_SQRBRACKET";
        case COMMA:
            return "COMMA";
        case COLON:
            return "COLON";
        case JSON_NULL:
            return "JSON_NULL";
        case NUMBER:
            return "NUMBER";
        case TRUE:
            return "TRUE";
        case FALSE:
            return "FALSE";
        case STRING:
            return "STRING";
        default:
            throw "Uknown token!";
            break;
        }
    }

    std::string str;
    std::queue<Token> tokens;
    std::queue<std::string> values;
    std::queue<std::pair<int,int>> traceback;
    uint32_t strSize;
    uint32_t pos;
    uint32_t line;
    uint32_t column;
    char symbol;
    Token token;
    
    uint32_t tbLine, tbColumn;

    inline void PushToken(Token t)
    {
        tokens.push(t);
        traceback.push({line+1, column});
    }

    inline bool NextSymbol()
    {
        pos++;
        bool hasNext = strSize > pos;
        if(hasNext)
        {
            symbol = str.at(pos);
            column++;
        }
        return hasNext;
    }

    void HandleSymbol();

    void Tokenise();

    inline Token PeekNextToken() { return tokens.front(); }

    inline void PopNextToken() 
    {
        token = tokens.front();
        auto [ l, c ] = traceback.front();
        tbLine = l; tbColumn = c;
        tokens.pop();
        traceback.pop();
    }

    inline std::string PopNextValue() 
    {
        std::string t = values.front();
        values.pop();
        return t;
    }

    inline void AssertToken(Token assertion)
    {
        if(token != assertion)
        {
            std::cerr << "JSON parser error! Unexpected token '" << ToString(token) 
                    << "' On line " << tbLine << ", column " << tbColumn << ". '" 
                    << ToString(assertion) << "' expected." << std::endl;
            throw "Failed"; 
        }
    }

    inline void UnexpectedTokenError()
    {
        std::cerr << "JSON parser error: unexpected symbol '" 
            << ToString(token) << "' on line "
            << tbLine << ", column " << tbColumn 
            << " while parsing JSONValue" 
            << std::endl;
        throw "Failed";
    }

    JSONValue* ParseArray();

    JSONValue* ParseValue();

    JSONKeyValuePair ParseKeyValuePair();

    JSONObject* ParseObject();

public:
    JSONParser(std::string jsonString);

    JSONObject* ParseToObject();

};

} // namespace Engine::Utilities::JSON
