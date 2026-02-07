/**
 * @file FlightMode.h
 * @brief Gestion des modes de vol et transitions
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 */

#ifndef FLIGHT_MODE_H
#define FLIGHT_MODE_H

#include <stdint.h>
#include "PowerDistribution.h"

/**
 * @brief Classe de gestion des modes de vol
 * 
 * Gère les transitions entre modes et maintient l'état système
 */
class FlightMode {
public:
    /**
     * @brief Constructeur
     */
    FlightMode();

    /**
     * @brief Définit le mode de vol actif
     * 
     * @param newMode Nouveau mode à activer
     */
    void setMode(PowerDistribution::FlightMode newMode);

    /**
     * @brief Retourne le mode de vol actif
     * 
     * @return Mode actif
     */
    PowerDistribution::FlightMode getMode() const;

    /**
     * @brief Retourne le nom du mode actif
     * 
     * @return Pointeur vers chaîne constante
     */
    const char* getModeName() const;

    /**
     * @brief Définit la puissance totale demandée
     * 
     * Respecte les limites min/max du mode actif
     * 
     * @param power Puissance demandée (Cv)
     */
    void setTotalPower(uint16_t power);

    /**
     * @brief Augmente la puissance totale
     * 
     * @param increment Incrément (Cv)
     */
    void increasePower(uint16_t increment);

    /**
     * @brief Diminue la puissance totale
     * 
     * @param decrement Décrément (Cv)
     */
    void decreasePower(uint16_t decrement);

    /**
     * @brief Retourne la puissance totale demandée
     * 
     * @return Puissance totale (Cv)
     */
    uint16_t getTotalPower() const;

    /**
     * @brief Retourne la puissance minimale du mode actif
     * 
     * @return Puissance minimale (Cv)
     */
    uint16_t getMinPower() const;

    /**
     * @brief Retourne la puissance maximale du mode actif
     * 
     * @return Puissance maximale (Cv)
     */
    uint16_t getMaxPower() const;

    /**
     * @brief Passe au mode suivant (rotation cyclique)
     * 
     * DÉCOLLAGE → NORMAL → URGENCE → DÉCOLLAGE
     */
    void nextMode();

private:
    PowerDistribution::FlightMode currentMode_;  ///< Mode actif
    uint16_t totalPower_;                        ///< Puissance totale demandée (Cv)

    /**
     * @brief Applique les contraintes min/max au changement de mode
     * 
     * Préserve la puissance actuelle si dans les limites,
     * sinon ajuste à la limite la plus proche
     */
    void applyPowerConstraints();
};

#endif // FLIGHT_MODE_H
