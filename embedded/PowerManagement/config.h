/**
 * @file config.h
 * @brief Configuration globale du système de gestion de puissance hybride
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 * 
 * Configuration inspirée des standards aéronautiques DO-178C
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// ============================================================================
// CONFIGURATION MATÉRIELLE
// ============================================================================

/** @brief Baudrate communication série (ARINC 429 simulé) */
#define SERIAL_BAUDRATE 115200U

/** @brief Pin encodeur rotatif - CLK */
#define ENCODER_CLK_PIN 2

/** @brief Pin encodeur rotatif - DT */
#define ENCODER_DT_PIN 3

/** @brief Pin bouton mode NORMAL */
#define BUTTON_NORMAL_PIN 4

/** @brief Pin bouton mode URGENCE */
#define BUTTON_URGENCE_PIN 5

/** @brief LED status système */
#define LED_STATUS_PIN LED_BUILTIN

// ============================================================================
// PARAMÈTRES DE VOL
// ============================================================================

/** @brief Configuration mode DÉCOLLAGE */
namespace DecollageConfig {
    constexpr uint16_t MIN_POWER = 0U;      ///< Puissance minimale (Cv)
    constexpr uint16_t MAX_POWER = 3250U;   ///< Puissance maximale (Cv)
    constexpr uint16_t INITIAL_POWER = 50U; ///< Puissance initiale (Cv)
    constexpr uint16_t ELECTRIC_MAX = 1000U; ///< Plafond électrique (Cv)
    constexpr uint16_t THERMAL_MAX = 2250U;  ///< Plafond thermique (Cv)
}

/** @brief Configuration mode NORMAL */
namespace NormalConfig {
    constexpr uint16_t MIN_POWER = 0U;      ///< Puissance minimale (Cv)
    constexpr uint16_t MAX_POWER = 2750U;   ///< Puissance maximale (Cv)
    constexpr uint16_t INITIAL_POWER = 50U; ///< Puissance initiale (Cv)
    constexpr uint16_t THERMAL_MAX = 2750U;  ///< Plafond thermique (Cv)
}

/** @brief Configuration mode URGENCE */
namespace UrgenceConfig {
    constexpr uint16_t MIN_POWER = 0U;      ///< Puissance minimale (Cv)
    constexpr uint16_t MAX_POWER = 3750U;   ///< Puissance maximale (Cv)
    constexpr uint16_t INITIAL_POWER = 50U; ///< Puissance initiale (Cv)
    constexpr uint16_t ELECTRIC_MAX = 1000U; ///< Plafond électrique (Cv)
    constexpr uint16_t THERMAL_MAX = 2750U;  ///< Plafond thermique (Cv)
}

// ============================================================================
// PARAMÈTRES SYSTÈME
// ============================================================================

/** @brief Intervalle watchdog (ms) */
#define WATCHDOG_INTERVAL 1000U

/** @brief Intervalle mise à jour affichage (ms) */
#define DISPLAY_UPDATE_INTERVAL 100U

/** @brief Intervalle transmission ARINC (ms) */
#define ARINC_TX_INTERVAL 50U

/** @brief Timeout boutons anti-rebond (ms) */
#define BUTTON_DEBOUNCE_TIME 50U

/** @brief Pas de l'encodeur (Cv par clic) */
#define ENCODER_STEP 10U

// ============================================================================
// CONSTANTES DE CONVERSION
// ============================================================================

/** @brief Conversion puissance : 100% = 2200W */
#define POWER_CONVERSION_FACTOR 2200.0F

/** @brief Turbine constante : 80% = 1760W */
#define TURBINE_CONSTANT_PERCENT 80U
#define TURBINE_CONSTANT_WATTS 1760.0F

// ============================================================================
// CODES ARINC 429 (Simulés)
// ============================================================================

/** @brief Label ARINC - Puissance totale */
#define ARINC_LABEL_TOTAL_POWER 0x270

/** @brief Label ARINC - Puissance électrique */
#define ARINC_LABEL_ELECTRIC_POWER 0x271

/** @brief Label ARINC - Puissance thermique */
#define ARINC_LABEL_THERMAL_POWER 0x272

/** @brief Label ARINC - Mode de vol */
#define ARINC_LABEL_FLIGHT_MODE 0x273

/** @brief Label ARINC - Status système */
#define ARINC_LABEL_SYSTEM_STATUS 0x274

// ============================================================================
// VERSION FIRMWARE
// ============================================================================

#define FIRMWARE_VERSION_MAJOR 1
#define FIRMWARE_VERSION_MINOR 0
#define FIRMWARE_VERSION_PATCH 0
#define FIRMWARE_BUILD "HACKATHON-2026"

#endif // CONFIG_H
