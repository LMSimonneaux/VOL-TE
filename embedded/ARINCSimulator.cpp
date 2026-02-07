/**
 * @file ARINCSimulator.cpp
 * @brief Implémentation du simulateur ARINC 429
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 */

#include "ARINCSimulator.h"
#include "config.h"
#include <Arduino.h>

// ============================================================================
// CONSTRUCTEUR
// ============================================================================

ARINCSimulator::ARINCSimulator() 
    : messageCounter_(0U)
{
}

// ============================================================================
// INITIALISATION
// ============================================================================

void ARINCSimulator::begin(uint32_t baudrate) {
    Serial.begin(baudrate);
    
    // Attente stabilisation Serial
    delay(1000);
    
    messageCounter_ = 0U;
}

void ARINCSimulator::sendSystemBanner() {
    Serial.println(F(""));
    Serial.println(F("╔════════════════════════════════════════════════════════════════╗"));
    Serial.println(F("║    SAFRAN PW100 - SYSTÈME DE GESTION PUISSANCE HYBRIDE        ║"));
    Serial.println(F("╠════════════════════════════════════════════════════════════════╣"));
    Serial.print(F("║    Firmware: v"));
    Serial.print(FIRMWARE_VERSION_MAJOR);
    Serial.print(F("."));
    Serial.print(FIRMWARE_VERSION_MINOR);
    Serial.print(F("."));
    Serial.print(FIRMWARE_VERSION_PATCH);
    Serial.print(F(" - "));
    Serial.print(FIRMWARE_BUILD);
    Serial.println(F("                      ║"));
    Serial.println(F("║    Protocole: ARINC 429 (Simulé)                               ║"));
    Serial.print(F("║    Baudrate: "));
    Serial.print(SERIAL_BAUDRATE);
    Serial.println(F(" bps                                      ║"));
    Serial.println(F("╚════════════════════════════════════════════════════════════════╝"));
    Serial.println(F(""));
    Serial.println(F("[SYSTEM] Initialisation complète - Prêt pour vol"));
    Serial.println(F(""));
}

// ============================================================================
// ENVOI DE TRAMES
// ============================================================================

void ARINCSimulator::sendTotalPower(uint16_t power) {
    char labelStr[5];
    formatLabel(ARINC_LABEL_TOTAL_POWER, labelStr);
    
    Serial.print(F("[ARINC] "));
    Serial.print(labelStr);
    Serial.print(F(" | TOTAL_POWER: "));
    Serial.print(power);
    Serial.print(F(" Cv | SEQ: "));
    Serial.println(messageCounter_++);
}

void ARINCSimulator::sendElectricPower(uint16_t power) {
    char labelStr[5];
    formatLabel(ARINC_LABEL_ELECTRIC_POWER, labelStr);
    
    Serial.print(F("[ARINC] "));
    Serial.print(labelStr);
    Serial.print(F(" | ELEC_POWER: "));
    Serial.print(power);
    Serial.print(F(" Cv | SEQ: "));
    Serial.println(messageCounter_++);
}

void ARINCSimulator::sendThermalPower(uint16_t power) {
    char labelStr[5];
    formatLabel(ARINC_LABEL_THERMAL_POWER, labelStr);
    
    Serial.print(F("[ARINC] "));
    Serial.print(labelStr);
    Serial.print(F(" | THRM_POWER: "));
    Serial.print(power);
    Serial.print(F(" Cv | SEQ: "));
    Serial.println(messageCounter_++);
}

void ARINCSimulator::sendFlightMode(PowerDistribution::FlightMode mode) {
    char labelStr[5];
    formatLabel(ARINC_LABEL_FLIGHT_MODE, labelStr);
    
    Serial.print(F("[ARINC] "));
    Serial.print(labelStr);
    Serial.print(F(" | FLIGHT_MODE: "));
    Serial.print(getModeName(mode));
    Serial.print(F(" | SEQ: "));
    Serial.println(messageCounter_++);
}

void ARINCSimulator::sendFullStatus(
    PowerDistribution::FlightMode mode,
    uint16_t totalPower,
    uint16_t electricPower,
    uint16_t thermalPower
) {
    Serial.println(F(""));
    Serial.println(F("┌────────────────────────────────────────────────────────────┐"));
    Serial.print(F("│ MODE: "));
    Serial.print(getModeName(mode));
    
    // Padding pour alignement
    const char* modeName = getModeName(mode);
    uint8_t modeLen = 0;
    while (modeName[modeLen] != '\0') modeLen++;
    
    for (uint8_t i = modeLen; i < 50; i++) {
        Serial.print(F(" "));
    }
    Serial.println(F("│"));
    
    Serial.println(F("├────────────────────────────────────────────────────────────┤"));
    Serial.print(F("│ Puissance Totale:      "));
    Serial.print(totalPower);
    Serial.print(F(" Cv"));
    
    // Padding
    uint16_t digits = 0;
    uint16_t temp = totalPower;
    do {
        digits++;
        temp /= 10;
    } while (temp > 0);
    
    for (uint8_t i = 0; i < (30 - digits); i++) {
        Serial.print(F(" "));
    }
    Serial.println(F("│"));
    
    Serial.print(F("│ Puissance Électrique:  "));
    Serial.print(electricPower);
    Serial.print(F(" Cv"));
    
    // Padding
    digits = 0;
    temp = electricPower;
    do {
        digits++;
        temp /= 10;
    } while (temp > 0);
    
    for (uint8_t i = 0; i < (30 - digits); i++) {
        Serial.print(F(" "));
    }
    Serial.println(F("│"));
    
    Serial.print(F("│ Puissance Thermique:   "));
    Serial.print(thermalPower);
    Serial.print(F(" Cv"));
    
    // Padding
    digits = 0;
    temp = thermalPower;
    do {
        digits++;
        temp /= 10;
    } while (temp > 0);
    
    for (uint8_t i = 0; i < (30 - digits); i++) {
        Serial.print(F(" "));
    }
    Serial.println(F("│"));
    
    Serial.println(F("└────────────────────────────────────────────────────────────┘"));
    Serial.println(F(""));
}

void ARINCSimulator::sendDashboard(
    PowerDistribution::FlightMode mode,
    uint16_t totalPower,
    uint16_t electricPower,
    uint16_t thermalPower
) {
    Serial.println(F(""));
    Serial.println(F("╔════════════════════════════════════════════════════════════════╗"));
    Serial.print(F("║  MODE: "));
    Serial.print(getModeName(mode));
    
    // Padding
    const char* modeName = getModeName(mode);
    uint8_t modeLen = 0;
    while (modeName[modeLen] != '\0') modeLen++;
    
    for (uint8_t i = modeLen; i < 52; i++) {
        Serial.print(F(" "));
    }
    Serial.println(F("║"));
    
    Serial.println(F("╠════════════════════════════════════════════════════════════════╣"));
    
    // Barre de puissance totale
    Serial.print(F("║  TOTAL  ["));
    
    uint8_t barLength = (totalPower * 40U) / 4000U;  // Échelle sur 4000 Cv max
    for (uint8_t i = 0; i < 40; i++) {
        if (i < barLength) {
            Serial.print(F("█"));
        } else {
            Serial.print(F("░"));
        }
    }
    
    Serial.print(F("] "));
    Serial.print(totalPower);
    Serial.println(F(" Cv ║"));
    
    // Barre de puissance électrique
    Serial.print(F("║  ELEC   ["));
    
    barLength = (electricPower * 40U) / 1000U;  // Échelle sur 1000 Cv max
    for (uint8_t i = 0; i < 40; i++) {
        if (i < barLength) {
            Serial.print(F("█"));
        } else {
            Serial.print(F("░"));
        }
    }
    
    Serial.print(F("] "));
    Serial.print(electricPower);
    Serial.println(F(" Cv  ║"));
    
    // Barre de puissance thermique
    Serial.print(F("║  THRM   ["));
    
    barLength = (thermalPower * 40U) / 2750U;  // Échelle sur 2750 Cv max
    for (uint8_t i = 0; i < 40; i++) {
        if (i < barLength) {
            Serial.print(F("█"));
        } else {
            Serial.print(F("░"));
        }
    }
    
    Serial.print(F("] "));
    Serial.print(thermalPower);
    Serial.println(F(" Cv ║"));
    
    Serial.println(F("╚════════════════════════════════════════════════════════════════╝"));
    Serial.println(F(""));
}

void ARINCSimulator::sendError(const char* errorMsg) {
    Serial.print(F("[ERROR] "));
    Serial.print(errorMsg);
    Serial.print(F(" | SEQ: "));
    Serial.println(messageCounter_++);
}

// ============================================================================
// UTILITAIRES PRIVÉS
// ============================================================================

void ARINCSimulator::formatLabel(uint16_t label, char* buffer) const {
    // Format: 0x270 → "270"
    buffer[0] = '0' + ((label >> 8) & 0x0F);
    buffer[1] = '0' + ((label >> 4) & 0x0F);
    buffer[2] = '0' + (label & 0x0F);
    buffer[3] = '\0';
}

uint8_t ARINCSimulator::calculateChecksum(const uint8_t* data, size_t length) const {
    uint8_t checksum = 0U;
    
    for (size_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

const char* ARINCSimulator::getModeName(PowerDistribution::FlightMode mode) const {
    switch (mode) {
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
