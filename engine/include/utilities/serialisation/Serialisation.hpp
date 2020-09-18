#pragma once
#include "utilities/StdUtilities.hpp"
#include "utilities/StringUtilities.hpp"

#include "platform/io/File.hpp"

#include <glm/glm.hpp>

#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

#define SERIALISE_PROPERTY(v) \
{ \
  \
    if(Engine::Utilities::Contains(#v, ".") || Engine::Utilities::Contains(#v, "->")) throw std::exception("Cannot serialize sub-members!");\
    Engine::Utilities::Serialisation::SerialiseProperty(this, #v, v); \
} 
#define STR(v) std::to_string(v)



namespace Engine::Utilities::Serialisation
{
class Serialisable 
{
public:
    virtual void SerialiseProperties() = 0;
};

class Serialiser final
{
public:
    // Mapping from object type to list of functions serializing the object properties
    static std::unordered_map<std::string, std::vector<std::function<std::string(void*, int)>>> Serialisers;// = new std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>>();
    // static std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>> Deserialisers;// = new std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>>();
    static std::set<std::string> SerializedProps;// = new std::set<std::string>();
};


inline void AddSerializer(std::string typeName, std::function<std::string(void*, int)> ser)
{
    if(Serialiser::Serialisers.contains(typeName)) Serialiser::Serialisers.at(typeName).push_back(ser);
    else
    {
        Serialiser::Serialisers.insert({typeName, {ser}});
    } 
}

inline static bool IsSerialised(std::string typeName, std::string member)
{
    if(Serialiser::SerializedProps.contains(typeName+member))
    {
        std::cerr << "Member '" + member + "' of '" + typeName + "' was already serialized. Skipping!" << std::endl;
        return true;
    }
    return false;
}

inline static std::string JSONObject(std::vector<std::string> keyvals, int indent = 0)
{
    std::string array = std::string("    ", indent) + "{\n" + std::string("    ", indent) + keyvals[0];
    for(uint32_t i = 1; i < keyvals.size(); i++) array += ",\n" + std::string("    ", indent+1) + keyvals[i];
    array += std::string("    ", indent) + " }";
    return array;
}

inline static std::string JSONArray(std::vector<std::string> values, int indent = 0)
{
    if(values.size() == 0) return "[]";
    std::string array = "[ " + values[0];
    for(uint32_t i = 1; i < values.size(); i++) array += ", " + values[i];
    array += " ]";
    return std::string("    ", indent) + array;
}

inline static std::string KeyValuePair(std::string key, std::string value, int indent = 0)
{
    return std::string("    ", indent) + "\"" + key + "\" : " + value;
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const glm::vec3& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;
    auto serializer = [offset, name](void* objPtr, int indent){
        glm::vec3* varloc = (glm::vec3*)((char*)objPtr+offset);
        glm::vec3& val = *varloc;
        return KeyValuePair(name, JSONArray({STR(val.x), STR(val.y), STR(val.z)}), indent);
    };
    Serialiser::Serialisers[typeName].push_back(serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const std::string& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;
    auto serializer = [offset, name](void* objPtr, int indent){
        std::string* varloc = (std::string*)((char*)objPtr+offset);
        std::string& val = *varloc;
        return KeyValuePair(name,  "\"" + val + "\"", indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const int& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        int* varloc = (int*)((char*)objPtr+offset);
        int& val = *varloc;
        return KeyValuePair(name, std::to_string(val), indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const uint32_t& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        uint32_t* varloc = (uint32_t*)((char*)objPtr+offset);
        uint32_t& val = *varloc;
        return KeyValuePair(name, std::to_string(val), indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const float& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        float* varloc = (float*)((char*)objPtr+offset);
        float& val = *varloc;
        return KeyValuePair(name, std::to_string(val), indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const double& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        double* varloc = (double*)((char*)objPtr+offset);
        double& val = *varloc;
        return KeyValuePair(name, std::to_string(val), indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const bool& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        bool* varloc = (bool*)((char*)objPtr+offset);
        bool& val = *varloc;
        return KeyValuePair(name, std::to_string(val), indent);
    };
    AddSerializer(typeName, serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C, typename T>
void SerialiseProperty(C* object, const std::string& name, const T& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr, int indent){
        T* varloc = (T*)((char*)objPtr+offset);
        T& val = *varloc;
        return KeyValuePair(name, SerializeObject(varloc, false, indent), indent);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

// Cannot use this generic funtion as to_string is not and cannot be overloaded for custom types
// template<class C, typename T>
// void SerialiseProperty(C* object, const std::string& name, const T& value)
// {
//     std::string typeName = typeid(C).name();
//     if(IsSerialised(typeName, name)) return;
//     __int64 offset = (char*)&value - (char*)object;

//     auto serializer = [offset, name](void* objPtr){
//         T* varloc = (T*)((char*)objPtr+offset);
//         T& val = *varloc;
//         return "\"" + name + "\" " + std::to_string(val);
//     };
//     AddSerializer(typeName,serializer);
//     Serialiser::SerializedProps.insert(typeName+name);
// }


template<class C> 
std::string SerializeObject(C* object, bool includeTypeName = true, int indent = 0)
{
    std::string typeName = typeid(C).name();
    if(!Serialiser::Serialisers.contains(std::string(typeName)))
    {
        auto msg = "Cannot serialize '" + typeName + "'! No object properties were serialized!";
        throw std::exception(msg.c_str());
    }
    auto& serializerSet = Serialiser::Serialisers.at(typeName);
    int numProperties = int(serializerSet.size());
    std::string out;// = (includeTypeName ? typeName : "");
    out += "\n"+ std::string("    ", indent) + "{\n";// + serializerSet[0](object, indent+1) + (numProperties > 1 ? ",\n" : "\n");
    for(int i = 0; i < numProperties; i++)
    {
        out += std::string("    ", indent+1) + serializerSet[i](object, indent+1) + (i < numProperties-1 ? ",\n" : "\n");
    }
    out += std::string("    ", indent) + "}\n";
    return (includeTypeName ? KeyValuePair(typeName, out) : out);
}



} // namespace Utilities
