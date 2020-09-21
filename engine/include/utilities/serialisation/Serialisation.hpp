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

#define SERIALISABLE(C, type, member) \
type member = Engine::Utilities::Serialisation::SerialiseProperty<C>(Offset(offsetof(C, member)), #member, member);

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
friend class Serialiser;
private:
    static std::vector<Serialisable*> Serialisables;

protected:

    Index SerialisableID;
public:
    inline Serialisable()
    {
        Serialisables.push_back(this);
    }

    virtual std::string GetSerialised(int indent) = 0;
};

class Serialiser final
{
    static bool initialised;
public:
    // Mapping from object type to list of functions serializing the object properties
    static std::unordered_map<std::string, Index> IDMapping;
    static Index SerialisableIDCounter;
    static std::unordered_map<std::string, std::vector<std::function<std::string(void*, int)>>>* Serialisers;
    static std::set<std::string>* SerializedProps;

    template<class C>
    inline static bool IsSerialised(std::string member)
    {
        if(!initialised)
        {
            SerializedProps = new std::set<std::string>();
            Serialisers = new std::unordered_map<std::string, std::vector<std::function<std::string(void*, int)>>>();
            initialised = true;
        }
        std::string typeName = typeid(C).name();
        if(Serialiser::SerializedProps->contains(typeName+member))
        {
            return true;
        }
        return false;
    }

};


// inline void AddSerializer<C>(std::string typeName, std::function<std::string(void*, int)> ser)
// {
    
//     if(Serialiser::Serialisers->contains(typeName)) Serialiser::Serialisers->at(typeName).push_back(ser);
//     else
//     {
//         Serialiser::Serialisers->insert({typeName, {ser}});
//     } 
// }

// template<class C>
// inline void AddSerializer<C>(C* object, std::string member, std::function<std::string(void*, int)> ser)
// {
//     std::string typeName = typeid(C).name();
//     if(Serialiser::Serialisers->contains(typeName)) Serialiser::Serialisers->at(typeName).push_back(ser);
//     else
//     {
//         Serialiser::Serialisers->insert({typeName, {ser}});
//     }
// }

template<class C>
inline void AddSerializer(std::string member, std::function<std::string(void*, int)> ser)
{
    std::string typeName = typeid(C).name();
    if(Serialiser::Serialisers->contains(typeName)) Serialiser::Serialisers->at(typeName).push_back(ser);
    else
    {
        Serialiser::Serialisers->insert({typeName, {ser}});
    }
    // std::cout << "Serialising " << member << " of " << typeName << std::endl;
    Serialiser::SerializedProps->insert(typeName+member);
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

inline static std::string KeyValuePairObject(std::string key, std::string value, int indent = 0)
{
    return std::string("    ", indent) + "{ \"" + key + "\" : " + value + " }";
}

template<class C>
glm::vec3& SerialiseProperty(Offset offset, std::string memberName, glm::vec3& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            glm::vec3* varloc = (glm::vec3*)((char*)objPtr+offset);
            glm::vec3& val = *varloc;
            return KeyValuePair(memberName, JSONArray({STR(val.x), STR(val.y), STR(val.z)}), indent);
        };
        AddSerializer<C>(memberName, serializer);
    } 
    return glm::vec3();
}

template<class C>
std::string SerialiseProperty(Offset offset, std::string memberName, std::string& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            std::string* varloc = (std::string*)((char*)objPtr+offset);
            std::string& val = *varloc;
            return KeyValuePair(memberName,  "\"" + val + "\"", indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return std::string();
}

template<class C>
int SerialiseProperty(Offset offset, std::string memberName, int& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            int* varloc = (int*)((char*)objPtr+offset);
            int& val = *varloc;
            return KeyValuePair(memberName, std::to_string(val), indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return int();
}

template<class C>
uint32_t SerialiseProperty(Offset offset, std::string memberName, uint32_t& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, uint32_t indent){
            uint32_t* varloc = (uint32_t*)((char*)objPtr+offset);
            uint32_t& val = *varloc;
            return KeyValuePair(memberName, std::to_string(val), indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return uint32_t();
}

template<class C>
float SerialiseProperty(Offset offset, std::string memberName, float& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            float* varloc = (float*)((char*)objPtr+offset);
            float& val = *varloc;
            return KeyValuePair(memberName, std::to_string(val), indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return float();
}

template<class C>
double SerialiseProperty(Offset offset, std::string memberName, double& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            double* varloc = (double*)((char*)objPtr+offset);
            double& val = *varloc;
            return KeyValuePair(memberName, std::to_string(val), indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return double();
}

template<class C>
bool SerialiseProperty(Offset offset, std::string memberName, bool& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            bool* varloc = (bool*)((char*)objPtr+offset);
            bool& val = *varloc;
            return KeyValuePair(memberName, std::to_string(val), indent);
        };
        AddSerializer<C>(memberName, serializer);
    }
    return true;
}

template<class C, class T>
T SerialiseProperty(Offset offset, std::string memberName, T& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serializer = [offset, memberName](void* objPtr, int indent){
            T* varloc = (T*)((char*)objPtr+offset);
            T& val = *varloc;
            return KeyValuePair(memberName, SerializeObject(varloc, indent+1), indent);
        };
        AddSerializer<C>(memberName, serializer);
    } 
    return T();
}

template<class C> 
std::string SerializeObject(C* object, int indent = 0)
{
    std::string typeName = typeid(C).name();
    if(!Serialiser::Serialisers->contains(typeName))
    {
        std::cerr << "Cannot serialize '" + typeName + "'! No object properties were serialized!" << std::endl;
        return "";
        
    }
    auto& serializerSet = Serialiser::Serialisers->at(typeName);
    int numProperties = int(serializerSet.size());
    std::string out;
    out += "\n"+ std::string("    ", indent) + "{\n";
    for(int i = 0; i < numProperties; i++)
    {
        out += std::string("    ", indent+1) + serializerSet[i](object, indent+1) + (i < numProperties-1 ? ",\n" : "\n");
    }
    out += std::string("    ", indent) + "}";
    return out;
}



} // namespace Utilities
