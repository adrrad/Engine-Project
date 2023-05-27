#pragma once

namespace GLSL
{

class GLSLParser
{
private:
    enum Token
    {
        // Symbols
        LEFT_BRACKET, RIGHT_SQRBRACKET, 
        LEFT_SQRBRACKET, RIGHT_BRACKET, 
        LEFT_PAREN, RIGHT_PAREN,
        COMMA, COLON, DOT, HASHTAG, 
        GREATER, LESS,
        // Literals
        NUMBER, TRUE, FALSE,
        // Types
        VOID,
        BOOL, INT, UINT, FLOAT, DOUBLE,
        BVEC2, BVEC3, BVEC4, 
        IVEC2, IVEC3, IVEC4, 
        UVEC2, UVEC3, UVEC4, 
        VEC2, VEC3, VEC4, 
        DVEC2, DVEC3, DVEC4, 
        MAT2X2, MAT2X3, MAT2X4,
        MAT3X2, MAT3X3, MAT3X4,
        MAT4X2, MAT4X3, MAT4X4,
        // Opaques
        SAMPLER2D, SAMPLERCUBE,
        // Keywords
        STRUCT, UNIFORM, 
        // Macros
        VERSION, DEFINE
    };


};

} // namespace GLSL
