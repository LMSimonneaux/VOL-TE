/**
 * @file FlightMode.cpp
 * @brief Implémentation de la gestion des modes de vol
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 */

#include "FlightMode.h"
#include "config.h"

// ============================================================================
// CONSTRUCTEUR
// ============================================================================

FlightMode::FlightMode() 
    : currentMode_(PowerDistribution::FlightMode::DECOLLAGE)
    , totalPower_(DecollageConfig::INITIAL_POWER)
{
    // Initialisation mode DÉCOLLAGE
}

// ============================================================================
// GESTION DU MODE
// ============================================================================

void FlightMode::setMode(PowerDistribution::FlightMode newMode) {
    currentMode_ = newMode;
    applyPowerConstraints();
}

PowerDistribution::FlightMode FlightMode::getMode() const {
    return currentMode_;
}

const char* FlightMode::getModeName() const {
    switch (currentMode_) {
        case PowerDistribution::FlightMode::DECOLLAGE:
            return "DECOLLAGE";
        
        case PowerDistribution::FlightMode::NORMAL:
            return "NORMAL";
        
        case PowerDistribution::FlightMode::URGENCE:
            return "URGENCE";
        
        default:
            return "UNKNOWN";
    }
}

void FlightMode::nextMode() {
    switch (currentMode_) {
        case PowerDistribution::FlightMode::DECOLLAGE:
            setMode(PowerDistribution::FlightMode::NORMAL);
            break;
        
        case PowerDistribution::FlightMode::NORMAL:
            setMode(PowerDistribution::FlightMode::URGENCE);
            break;
        
        case PowerDistribution::FlightMode::URGENCE:
            setMode(PowerDistribution::FlightMode::DECOLLAGE);
            break;
        
        default:
            setMode(PowerDistribution::FlightMode::DECOLLAGE);
            break;
    }
}

// ============================================================================
// GESTION DE LA PUISSANCE
// ============================================================================

void FlightMode::setTotalPower(uint16_t power) {
    uint16_t minPower = getMinPower();
    uint16_t maxPower = getMaxPower();
    
    // Contraindre la puissance dans les limites
    if (power < minPower) {
        totalPower_ = minPower;
    } else if (power > maxPower) {
        totalPower_ = maxPower;
    } else {
        totalPower_ = power;
    }
}

void FlightMode::increasePower(uint16_t increment) {
    uint16_t newPower = totalPower_ + increment;
    
    // Prévention dépassement entier
    if (newPower < totalPower_) {
        newPower = getMaxPower();
    }
    
    setTotalPower(newPower);
}

void FlightMode::decreasePower(uint16_t decrement) {
    uint16_t newPower = 0U;
    
    // Prévention underflow
    if (totalPower_ > decrement) {
        newPower = totalPower_ - decrement;
    }
    
    setTotalPower(newPower);
}

uint16_t FlightMode::getTotalPower() const {
    return totalPower_;
}

// ============================================================================
// LIMITES DU MODE
// ============================================================================

uint16_t FlightMode::getMinPower() const {
    switch (currentMode_) {
        case PowerDistribution::FlightMode::DECOLLAGE:
            return DecollageConfig::MIN_POWER;
        
        case PowerDistribution::FlightMode::NORMAL:
            return NormalConfig::MIN_POWER;
        
        case PowerDistribution::FlightMode::URGENCE:
            return UrgenceConfig::MIN_POWER;
        
        default:
            return 0U;
    }
}

uint16_t FlightMode::getMaxPower() const {
    switch (currentMode_) {
        case PowerDistribution::FlightMode::DECOLLAGE:
            return DecollageConfig::MAX_POWER;
        
        case PowerDistribution::FlightMode::NORMAL:
            return NormalConfig::MAX_POWER;
        
        case PowerDistribution::FlightMode::URGENCE:
            return UrgenceConfig::MAX_POWER;
        
        default:
            return 0U;
    }
}

// ============================================================================
// UTILITAIRES PRIVÉS
// ============================================================================

void FlightMode::applyPowerConstraints() {
    // Re-contraindre la puissance aux nouvelles limites du mode
    setTotalPower(totalPower_);
}
