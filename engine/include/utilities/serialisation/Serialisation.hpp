#pragma once
#include "utilities/StdUtilities.hpp"

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

class Serialiser
{
public:
    // Mapping from object type to list of functions serializing the object properties
    static std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>> Serialisers;// = new std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>>();
    static std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>> Deserialisers;// = new std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>>();
    static std::set<std::string> SerializedProps;// = new std::set<std::string>();
};


inline void AddSerializer(std::string typeName, std::function<std::string(void*)> ser)
{
    if(Serialiser::Serialisers.contains(typeName)) Serialiser::Serialisers.at(typeName).push_back(ser);
    else
    {
        Serialiser::Serialisers.insert({typeName, {ser}});
        int q = Serialiser::Serialisers.size();
        std::cout << &Serialiser::Serialisers << std::endl;
        q = 0;
    } 
}

inline bool IsSerialised(std::string typeName, std::string member)
{
    if(Serialiser::SerializedProps.contains(typeName+member))
    {
        std::cerr << "Member '" + member + "' of '" + typeName + "' was already serialized. Skipping!" << std::endl;
        return true;
    }
    return false;
}

template<class C>
void SerialiseProperty(C* object, const std::string& name, const glm::vec3& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;
    auto serializer = [offset, name](void* objPtr){
        glm::vec3* varloc = (glm::vec3*)((char*)objPtr+offset);
        glm::vec3& val = *varloc;
        return "\"" + name + "\" " + STR(val.x) + " " + STR(val.y) + " " + STR(val.z);
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
    auto serializer = [offset, name](void* objPtr){
        std::string* varloc = (std::string*)((char*)objPtr+offset);
        std::string& val = *varloc;
        return "\"" + name + "\" " + val;
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}

template<class C, typename T>
void SerialiseProperty(C* object, const std::string& name, const T& value)
{
    std::string typeName = typeid(C).name();
    if(IsSerialised(typeName, name)) return;
    __int64 offset = (char*)&value - (char*)object;

    auto serializer = [offset, name](void* objPtr){
        T* varloc = (T*)((char*)objPtr+offset);
        T& val = *varloc;
        return "\"" + name + "\" " + std::to_string(val);
    };
    AddSerializer(typeName,serializer);
    Serialiser::SerializedProps.insert(typeName+name);
}


template<class C> 
std::string SerializeObject(C* object)
{
    std::string typeName = typeid(C).name();
    std::cout << &Serialiser::Serialisers << std::endl;
    if(!Serialiser::Serialisers.contains(std::string(typeName)))
    {
        auto msg = "Cannot serialize '" + typeName + "'! No object properties were serialized!";
        throw std::exception(msg.c_str());
    }
    auto& serializerSet = Serialiser::Serialisers.at(typeName);
    std::string out = typeName + "\n";
    for(auto& propertySerizalizer : serializerSet)
    {
        out += propertySerizalizer(object) + "\n";
    }
    return out;
}



} // namespace Utilities
