/**
 * @file PowerDistribution.h
 * @brief Gestion de la distribution de puissance hybride électrique/thermique
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 * 
 * Réplique exacte de la logique du fichier interface.html
 */

#ifndef POWER_DISTRIBUTION_H
#define POWER_DISTRIBUTION_H

#include <stdint.h>

/**
 * @brief Classe de gestion de la distribution de puissance
 * 
 * Implémente la logique de répartition électrique/thermique
 * selon le mode de vol actif (Décollage, Normal, Urgence)
 */
class PowerDistribution {
public:
    /**
     * @brief Structure de sortie des puissances calculées
     */
    struct PowerOutput {
        uint16_t electric;   ///< Puissance électrique (Cv)
        uint16_t thermal;    ///< Puissance thermique (Cv)
        uint16_t total;      ///< Puissance totale (Cv)
    };

    /**
     * @brief Énumération des modes de vol
     */
    enum class FlightMode : uint8_t {
        DECOLLAGE = 0U,  ///< Mode décollage
        NORMAL = 1U,     ///< Mode normal (croisière)
        URGENCE = 2U     ///< Mode urgence
    };

    /**
     * @brief Constructeur par défaut
     */
    PowerDistribution();

    /**
     * @brief Calcule la distribution de puissance pour le mode DÉCOLLAGE
     * 
     * Logique:
     * - Électrique: min(totalPower, 1000 Cv)
     * - Thermique: min(totalPower - electric, 2250 Cv)
     * 
     * @param totalPower Puissance totale demandée (Cv)
     * @return PowerOutput Structure avec electric/thermal/total
     */
    PowerOutput calculateDecollage(uint16_t totalPower) const;

    /**
     * @brief Calcule la distribution de puissance pour le mode NORMAL
     * 
     * Logique:
     * - Électrique: 0 Cv (thermique seul)
     * - Thermique: min(totalPower, 2750 Cv)
     * 
     * @param totalPower Puissance totale demandée (Cv)
     * @return PowerOutput Structure avec electric/thermal/total
     */
    PowerOutput calculateNormal(uint16_t totalPower) const;

    /**
     * @brief Calcule la distribution de puissance pour le mode URGENCE
     * 
     * Logique:
     * - Électrique: min(totalPower, 1000 Cv)
     * - Thermique: min(totalPower - electric, 2750 Cv)
     * 
     * @param totalPower Puissance totale demandée (Cv)
     * @return PowerOutput Structure avec electric/thermal/total
     */
    PowerOutput calculateUrgence(uint16_t totalPower) const;

    /**
     * @brief Calcule la distribution selon le mode actif
     * 
     * @param mode Mode de vol
     * @param totalPower Puissance totale demandée (Cv)
     * @return PowerOutput Structure avec electric/thermal/total
     */
    PowerOutput calculate(FlightMode mode, uint16_t totalPower) const;

    /**
     * @brief Convertit une puissance en Cv vers Watts
     * 
     * @param cv Puissance en chevaux
     * @return Puissance en Watts (float)
     */
    static float cvToWatts(uint16_t cv);

    /**
     * @brief Convertit une puissance en Watts vers Cv
     * 
     * @param watts Puissance en Watts
     * @return Puissance en chevaux (uint16_t)
     */
    static uint16_t wattsToCv(float watts);

private:
    /**
     * @brief Fonction utilitaire min() type-safe
     * 
     * @param a Premier paramètre
     * @param b Deuxième paramètre
     * @return Minimum des deux paramètres
     */
    static uint16_t min(uint16_t a, uint16_t b);
};

#endif // POWER_DISTRIBUTION_H
