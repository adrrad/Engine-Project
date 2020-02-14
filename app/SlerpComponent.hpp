#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Transform.hpp"
class SlerpComponent : public Components::BaseComponent
{
    
    Rendering::Transform* t1;
    Rendering::Transform* t2;
    bool flip = false;
    float time = 0.0f;
public:
    SlerpComponent()
    {

    }

    void SetTransforms(Rendering::Transform* t1, Rendering::Transform* t2)
    {
        this->t1 = t1;
        this->t2 = t2;
    }

    void Update(float deltaTime)
    {
        
        Quaternion q = Quaternion::FromEuler(t1->rotation);
        Quaternion p = Quaternion::FromEuler(t2->rotation);
        Quaternion r = Quaternion::Slerp(q, p, time);
        sceneObject->transform.rotation = r.ToEuler();
        if(time > 1.0f) flip = true;
        if(time < 0.0f) flip = false;
        time += flip ? -deltaTime : deltaTime;
        
    }
};