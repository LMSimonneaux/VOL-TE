/**
 * @file ARINCSimulator.h
 * @brief Simulateur de protocole ARINC 429 sur Serial
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 * 
 * Simule le format ARINC 429 pour un affichage professionnel
 */

#ifndef ARINC_SIMULATOR_H
#define ARINC_SIMULATOR_H

#include <stdint.h>
#include "PowerDistribution.h"

/**
 * @brief Classe de simulation ARINC 429
 * 
 * Formate les données de vol en messages ARINC-like sur Serial
 */
class ARINCSimulator {
public:
    /**
     * @brief Constructeur
     */
    ARINCSimulator();

    /**
     * @brief Initialise le simulateur ARINC
     * 
     * @param baudrate Vitesse de communication série
     */
    void begin(uint32_t baudrate);

    /**
     * @brief Envoie le banner système au démarrage
     */
    void sendSystemBanner();

    /**
     * @brief Envoie une trame de puissance totale
     * 
     * @param power Puissance totale (Cv)
     */
    void sendTotalPower(uint16_t power);

    /**
     * @brief Envoie une trame de puissance électrique
     * 
     * @param power Puissance électrique (Cv)
     */
    void sendElectricPower(uint16_t power);

    /**
     * @brief Envoie une trame de puissance thermique
     * 
     * @param power Puissance thermique (Cv)
     */
    void sendThermalPower(uint16_t power);

    /**
     * @brief Envoie une trame de changement de mode
     * 
     * @param mode Nouveau mode
     */
    void sendFlightMode(PowerDistribution::FlightMode mode);

    /**
     * @brief Envoie le statut système complet
     * 
     * @param mode Mode de vol actif
     * @param totalPower Puissance totale (Cv)
     * @param electricPower Puissance électrique (Cv)
     * @param thermalPower Puissance thermique (Cv)
     */
    void sendFullStatus(
        PowerDistribution::FlightMode mode,
        uint16_t totalPower,
        uint16_t electricPower,
        uint16_t thermalPower
    );

    /**
     * @brief Envoie un tableau de bord formaté
     * 
     * @param mode Mode de vol actif
     * @param totalPower Puissance totale (Cv)
     * @param electricPower Puissance électrique (Cv)
     * @param thermalPower Puissance thermique (Cv)
     */
    void sendDashboard(
        PowerDistribution::FlightMode mode,
        uint16_t totalPower,
        uint16_t electricPower,
        uint16_t thermalPower
    );

    /**
     * @brief Envoie un message d'erreur
     * 
     * @param errorMsg Message d'erreur
     */
    void sendError(const char* errorMsg);

private:
    uint32_t messageCounter_;  ///< Compteur de messages (séquence)

    /**
     * @brief Formatte un label ARINC en hexadécimal
     * 
     * @param label Code label ARINC
     * @param buffer Buffer de sortie (min 4 chars)
     */
    void formatLabel(uint16_t label, char* buffer) const;

    /**
     * @brief Calcule un checksum simple (somme modulo 256)
     * 
     * @param data Données à checker
     * @param length Longueur des données
     * @return Checksum 8 bits
     */
    uint8_t calculateChecksum(const uint8_t* data, size_t length) const;

    /**
     * @brief Retourne le nom du mode de vol
     * 
     * @param mode Mode de vol
     * @return Chaîne constante
     */
    const char* getModeName(PowerDistribution::FlightMode mode) const;
};

#endif // ARINC_SIMULATOR_H
