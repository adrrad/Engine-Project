#pragma once

#include "components/BaseComponent.hpp"


class StatsComponent : Engine::Components::BaseComponent
{
private:
    uint32_t m_baseVitality = 0;
    uint32_t m_baseStrength = 0;
    uint32_t m_baseAgility = 0;
    uint32_t m_baseEnergy = 0;
    uint32_t m_baseHealth = 0;

public:

    uint32_t CalculateMaxHealth();


};