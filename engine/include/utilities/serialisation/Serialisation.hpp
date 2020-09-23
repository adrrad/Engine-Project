#pragma once

#include "rendering/Quaternion.hpp"
#include "utilities/StdUtilities.hpp"
#include "utilities/StringUtilities.hpp"
#include "utilities/json/JSON.hpp"

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
// #define SERIALISABLE(C, type, member) \
// type member;// = Engine::Utilities::Serialisation::SerialiseProperty<C>(Offset(offsetof(C, member)), #member, member);

#define SERIALISE_PROPERTY(v) \
{ \
  \
    if(Engine::Utilities::Contains(#v, ".") || Engine::Utilities::Contains(#v, "->")) throw std::exception("Cannot serialise sub-members!");\
    Engine::Utilities::Serialisation::SerialiseProperty(this, #v, v); \
} 
#define STR(v) std::to_string(v)



namespace Engine::Utilities::Serialisation
{
template <class T>
class Serialisable 
{
friend class Serialiser;

protected:
public:
    inline virtual JSON::JSONValue* GetSerialised() final
    {
        return SerialiseObject(dynamic_cast<T*>(this));
    }

    inline std::string GetClassName() 
    { 
        static std::string className = typeid(T).name();
        return className; 
    }

    template<class B, class C>
    inline void SerialiseBaseClass()
    {
        std::string baseName = typeid(B).name();
        std::string className = typeid(C).name();
        if(Serialiser::SerialisedProps->contains(baseName)) return;
        auto sers = Serialiser::Serialisers->at(className);
        auto desers = Serialiser::Deserialisers->at(className);
        if(!Serialiser::Serialisers->contains(baseName)) Serialiser::Serialisers->insert({baseName, {}});
        if(!Serialiser::Deserialisers->contains(baseName)) Serialiser::Deserialisers->insert({baseName, {}});
        for(auto ser : sers) Serialiser::Serialisers->at(baseName).push_back(ser);
        for(auto deser : desers) Serialiser::Deserialisers->at(baseName).push_back(deser);
        Serialiser::SerialisedProps->insert(baseName);
    }

};



class Serialiser final
{
    static bool initialised;
public:

    static std::unordered_map<std::string, std::vector<std::function<JSON::JSONKeyValuePair(void*)>>>* Serialisers;
    static std::unordered_map<std::string, std::vector<std::function<void(void*, JSON::JSONValue&)>>>* Deserialisers;
    static std::set<std::string>* SerialisedProps;

    template<class C>
    inline static bool IsSerialised(std::string member)
    {
        if(!initialised)
        {
            SerialisedProps = new std::set<std::string>();
            Serialisers = new std::unordered_map<std::string, std::vector<std::function<JSON::JSONKeyValuePair(void*)>>>();
            Deserialisers = new std::unordered_map<std::string, std::vector<std::function<void(void*, JSON::JSONValue&)>>>();
            initialised = true;
        }
        std::string typeName = typeid(C).name();
        if(Serialiser::SerialisedProps->contains(typeName+member))
        {
            return true;
        }
        return false;
    }
};

template<class C>
inline void AddSerialiser(std::string member, std::function<JSON::JSONKeyValuePair(void*)> ser)
{
    std::string typeName = typeid(C).name();
    if(Serialiser::Serialisers->contains(typeName)) Serialiser::Serialisers->at(typeName).push_back(ser);
    else
    {
        auto classname = [typeName](void* ptr){
            return JSON::JSONKeyValuePair("Object Type", new JSON::JSONValue(typeName));
        };
        Serialiser::Serialisers->insert({typeName, {classname, ser}});
    } 
    Serialiser::SerialisedProps->insert(typeName+member);
}

template<class C>
inline void AddDeserialiser(std::string member, std::function<void(void*, JSON::JSONValue&)> deser)
{
    std::string typeName = typeid(C).name();
    if(Serialiser::Deserialisers->contains(typeName)) Serialiser::Deserialisers->at(typeName).push_back(deser);
    else Serialiser::Deserialisers->insert({typeName, {deser}});
}

template<class C>
glm::vec3& SerialiseProperty(Offset offset, std::string memberName, glm::vec3& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            glm::vec3* varloc = (glm::vec3*)((char*)objPtr+offset);
            glm::vec3& val = *varloc;
            auto array = new JSON::JSONValue({new JSON::JSONValue(val.x), new JSON::JSONValue(val.y), new JSON::JSONValue(val.z)});
            return JSON::JSONKeyValuePair(memberName, array);
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json) {
            glm::vec3& var = *((glm::vec3*)((char*)objPtr+offset));
            var.x = float(json[memberName]->Array[0]->Number);
            var.y = float(json[memberName]->Array[1]->Number);
            var.z = float(json[memberName]->Array[2]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    } 
    return glm::vec3();
}

template<class C>
Quaternion SerialiseProperty(Offset offset, std::string memberName, Quaternion member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            Quaternion& val = *((Quaternion*)((char*)objPtr+offset));
            auto array = new JSON::JSONValue({new JSON::JSONValue(val.w), new JSON::JSONValue(val.x), new JSON::JSONValue(val.y), new JSON::JSONValue(val.z)});
            return JSON::JSONKeyValuePair(memberName, array);
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json) {
            glm::vec3& var = *((glm::vec3*)((char*)objPtr+offset));
            var.x = float(json[memberName]->Array[0]->Number);
            var.y = float(json[memberName]->Array[1]->Number);
            var.z = float(json[memberName]->Array[2]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    } 
    return Quaternion();
}

template<class C>
std::string SerialiseProperty(Offset offset, std::string memberName, std::string& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            std::string* varloc = (std::string*)((char*)objPtr+offset);
            std::string& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(val));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            std::string& var = *((std::string*)((char*)objPtr+offset));
            var = json[memberName]->String;
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return std::string();
}

template<class C>
int SerialiseProperty(Offset offset, std::string memberName, int& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            int* varloc = (int*)((char*)objPtr+offset);
            int& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(double(val)));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            int& var = *((int*)((char*)objPtr+offset));
            var = int(json[memberName]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return int();
}

template<class C>
uint32_t SerialiseProperty(Offset offset, std::string memberName, uint32_t& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            uint32_t* varloc = (uint32_t*)((char*)objPtr+offset);
            uint32_t& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(double(val)));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            uint32_t& var = *((uint32_t*)((char*)objPtr+offset));
            var = uint32_t(json[memberName]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return uint32_t();
}

template<class C>
uint64_t SerialiseProperty(Offset offset, std::string memberName, uint64_t& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            uint64_t* varloc = (uint64_t*)((char*)objPtr+offset);
            uint64_t& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(double(val)));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            uint64_t& var = *((uint64_t*)((char*)objPtr+offset));
            var = uint64_t(json[memberName]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return uint64_t();
}


template<class C>
float SerialiseProperty(Offset offset, std::string memberName, float& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            float* varloc = (float*)((char*)objPtr+offset);
            float& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(double(val)));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            float& var = *((float*)((char*)objPtr+offset));
            var = float(json[memberName]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return float();
}

template<class C>
double SerialiseProperty(Offset offset, std::string memberName, double& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            double* varloc = (double*)((char*)objPtr+offset);
            double& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(double(val)));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            double& var = *((double*)((char*)objPtr+offset));
            var = double(json[memberName]->Number);
        };
        AddSerialiser<C>(memberName, serialiser);
        AddDeserialiser<C>(memberName, deserialiser);
    }
    return double();
}

template<class C>
bool SerialiseProperty(Offset offset, std::string memberName, bool& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            bool* varloc = (bool*)((char*)objPtr+offset);
            bool& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(val));
        };
        auto deserialiser = [offset, memberName](void* objPtr, JSON::JSONValue& json){
            bool& var = *((bool*)((char*)objPtr+offset));
            var = bool(json[memberName]->Boolean);
        };
        AddSerialiser<C>(memberName, serialiser);
    }
    return true;
}

template<class C, class T>
std::vector<T*> SerialiseProperty(Offset offset, std::string memberName, std::vector<T*>& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            std::vector<T*>* varloc = (std::vector<T*>*)((char*)objPtr+offset);
            std::vector<T*>& vals = *varloc;
            std::vector<JSON::JSONValue*> array;
            for(auto tval : vals) array.push_back(SerialiseObject(tval));
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(array));
        };
        std::string typeName = typeid(T).name();
        auto deserialiser = [offset, memberName, typeName](void* objPtr, JSON::JSONValue& json){
            T* var = (T*)((char*)objPtr+offset);
            DeserialiseObject(var, *json[memberName]);
        };
        AddSerialiser<C>(memberName, serialiser);
    } 
    return std::vector<T*>();
}

template<class C, class T>
std::vector<T*>* SerialiseProperty(Offset offset, std::string memberName, std::vector<T*>* member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            std::vector<T*>* varloc = *((std::vector<T*>**)((char*)objPtr+offset));
            std::vector<T*>& vals = *varloc;
            std::vector<JSON::JSONValue*> array;
            for(auto tval : vals) array.push_back(SerialiseObject(tval));
            return JSON::JSONKeyValuePair(memberName, new JSON::JSONValue(array));
        };
        std::string typeName = typeid(T).name();
        auto deserialiser = [offset, memberName, typeName](void* objPtr, JSON::JSONValue& json){
            T* var = (T*)((char*)objPtr+offset);
            DeserialiseObject(var, *json[memberName]);
        };
        AddSerialiser<C>(memberName, serialiser);
    } 
    return member;
}


template<class C, class T>
T SerialiseProperty(Offset offset, std::string memberName, T& member)
{
    if(!Serialiser::IsSerialised<C>(memberName))
    {
        auto serialiser = [offset, memberName](void* objPtr){
            T* varloc = (T*)((char*)objPtr+offset);
            T& val = *varloc;
            return JSON::JSONKeyValuePair(memberName, SerialiseObject(varloc));
        };
        std::string typeName = typeid(T).name();
        auto deserialiser = [offset, memberName, typeName](void* objPtr, JSON::JSONValue& json){
            T* var = (T*)((char*)objPtr+offset);
            DeserialiseObject(var, *json[memberName]);
        };
        AddSerialiser<C>(memberName, serialiser);
    } 
    return T();
}


inline void DeserialiseObject(void* object, std::string className, JSON::JSONValue& json)
{
    auto& deserialiserSet = Serialiser::Deserialisers->at(className);
    int numProperties = int(deserialiserSet.size());
    for(int i = 0; i < numProperties; i++)
    {
        deserialiserSet[i](object, json);
    }
}

inline void DeserialiseObject(void* object, JSON::JSONValue& json)
{
    auto& deserialiserSet = Serialiser::Deserialisers->at(json["Object Type"]->String);
    int numProperties = int(deserialiserSet.size());
    for(int i = 0; i < numProperties; i++)
    {
        deserialiserSet[i](object, json);
    }
}

template<class C> 
void DeserialiseObject(C* object, JSON::JSONValue& json)
{
    std::string typeName = json["Object Type"]->String;//typeid(C).name();
    auto& deserialiserSet = Serialiser::Deserialisers->at(typeName);
    int numProperties = int(deserialiserSet.size());
    for(int i = 0; i < numProperties; i++)
    {
        deserialiserSet[i](object, json);
    }
}

template<class C> 
JSON::JSONValue* SerialiseObject(C* object)
{
    std::string typeName = typeid(C).name();
    if(!Serialiser::Serialisers->contains(typeName))
    {
        std::cerr << "Cannot serialise '" + typeName + "'! No object properties were serialised!" << std::endl;
        // throw "Failed";
    }
    auto& serialiserSet = Serialiser::Serialisers->at(typeName);
    int numProperties = int(serialiserSet.size());
    std::vector<JSON::JSONKeyValuePair> members;
    for(int i = 0; i < numProperties; i++)
    {
        members.push_back(serialiserSet[i](object));
    }
    return new JSON::JSONValue(new JSON::JSONObject(members));
}



} // namespace Engine::Utilities::Serialisation
