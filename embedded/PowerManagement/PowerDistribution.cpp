/**
 * @file PowerDistribution.cpp
 * @brief Implémentation de la distribution de puissance hybride
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 */

#include "PowerDistribution.h"
#include "config.h"

// ============================================================================
// CONSTRUCTEUR
// ============================================================================

PowerDistribution::PowerDistribution() {
    // Pas d'initialisation nécessaire (classe sans état)
}

// ============================================================================
// CALCULS PAR MODE
// ============================================================================

PowerDistribution::PowerOutput PowerDistribution::calculateDecollage(uint16_t totalPower) const {
    PowerOutput output;
    
    // Mode DÉCOLLAGE: Électrique plafonné à 1000 Cv, thermique à 2250 Cv
    output.electric = min(totalPower, DecollageConfig::ELECTRIC_MAX);
    
    // Calcul du reste pour le thermique
    uint16_t remaining = 0U;
    if (totalPower > output.electric) {
        remaining = totalPower - output.electric;
    }
    
    output.thermal = min(remaining, DecollageConfig::THERMAL_MAX);
    output.total = output.electric + output.thermal;
    
    return output;
}

PowerDistribution::PowerOutput PowerDistribution::calculateNormal(uint16_t totalPower) const {
    PowerOutput output;
    
    // Mode NORMAL: Thermique seul (pas d'électrique)
    output.electric = 0U;
    output.thermal = min(totalPower, NormalConfig::THERMAL_MAX);
    output.total = output.thermal;
    
    return output;
}

PowerDistribution::PowerOutput PowerDistribution::calculateUrgence(uint16_t totalPower) const {
    PowerOutput output;
    
    // Mode URGENCE: Électrique plafonné à 1000 Cv, thermique à 2750 Cv
    output.electric = min(totalPower, UrgenceConfig::ELECTRIC_MAX);
    
    // Calcul du reste pour le thermique
    uint16_t remaining = 0U;
    if (totalPower > output.electric) {
        remaining = totalPower - output.electric;
    }
    
    output.thermal = min(remaining, UrgenceConfig::THERMAL_MAX);
    output.total = output.electric + output.thermal;
    
    return output;
}

PowerDistribution::PowerOutput PowerDistribution::calculate(FlightMode mode, uint16_t totalPower) const {
    switch (mode) {
        case FlightMode::DECOLLAGE:
            return calculateDecollage(totalPower);
        
        case FlightMode::NORMAL:
            return calculateNormal(totalPower);
        
        case FlightMode::URGENCE:
            return calculateUrgence(totalPower);
        
        default:
            // Mode par défaut (sécurité)
            return calculateNormal(totalPower);
    }
}

// ============================================================================
// CONVERSIONS
// ============================================================================

float PowerDistribution::cvToWatts(uint16_t cv) {
    // 100% = 2200W (configuration système)
    return (static_cast<float>(cv) / 100.0F) * POWER_CONVERSION_FACTOR;
}

uint16_t PowerDistribution::wattsToCv(float watts) {
    // Conversion inverse
    return static_cast<uint16_t>((watts * 100.0F) / POWER_CONVERSION_FACTOR);
}

// ============================================================================
// UTILITAIRES
// ============================================================================

uint16_t PowerDistribution::min(uint16_t a, uint16_t b) {
    return (a < b) ? a : b;
}
