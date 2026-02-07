/**
 * @file PowerManagement.ino
 * @brief Programme principal du système de gestion de puissance hybride
 * @author Hackathon FlyImpulse - Safran PW100
 * @date 2026-02-07
 * 
 * Système embarqué STM32 pour la gestion de puissance hybride
 * électrique/thermique sur turbopropulseur Safran PW100
 * 
 * COMMANDES SÉRIE:
 * - 'd' : Mode DÉCOLLAGE
 * - 'n' : Mode NORMAL
 * - 'u' : Mode URGENCE
 * - '+' : Augmenter puissance (+10 Cv)
 * - '-' : Diminuer puissance (-10 Cv)
 * - 's' : Afficher status complet
 * - 'h' : Afficher aide
 * - 'r' : Reset système
 * 
 * Format: <nombre> pour définir puissance exacte (ex: "1500" = 1500 Cv)
 */

#include "config.h"
#include "PowerDistribution.h"
#include "FlightMode.h"
#include "ARINCSimulator.h"

// ============================================================================
// INSTANCES GLOBALES
// ============================================================================

PowerDistribution powerCalc;      ///< Calculateur de distribution
FlightMode flightMode;             ///< Gestionnaire de mode de vol
ARINCSimulator arinc;              ///< Simulateur ARINC 429

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

unsigned long lastUpdateTime = 0;      ///< Dernier update affichage (ms)
unsigned long lastARINCTime = 0;       ///< Dernière transmission ARINC (ms)
bool systemReady = false;              ///< Flag système initialisé

String serialBuffer = "";              ///< Buffer de réception série

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    // Initialisation LED status
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, LOW);
    
    // Initialisation Serial/ARINC
    arinc.begin(SERIAL_BAUDRATE);
    
    // Banner système
    arinc.sendSystemBanner();
    
    // Initialisation mode par défaut (DÉCOLLAGE)
    flightMode.setMode(PowerDistribution::FlightMode::DECOLLAGE);
    flightMode.setTotalPower(DecollageConfig::INITIAL_POWER);
    
    // Status initial
    sendCurrentStatus();
    
    // Affichage aide
    printHelp();
    
    // Système prêt
    systemReady = true;
    digitalWrite(LED_STATUS_PIN, HIGH);
    
    Serial.println(F("[SYSTEM] Système opérationnel"));
    Serial.println(F(""));
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================

void loop() {
    unsigned long currentTime = millis();
    
    // Lecture commandes série
    handleSerialInput();
    
    // Update périodique affichage (100ms)
    if (currentTime - lastUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        updateDisplay();
    }
    
    // Transmission ARINC périodique (50ms)
    if (currentTime - lastARINCTime >= ARINC_TX_INTERVAL) {
        lastARINCTime = currentTime;
        sendARINCData();
    }
    
    // Heartbeat LED
    if ((currentTime / 500) % 2 == 0) {
        digitalWrite(LED_STATUS_PIN, HIGH);
    } else {
        digitalWrite(LED_STATUS_PIN, LOW);
    }
}

// ============================================================================
// GESTION ENTRÉES SÉRIE
// ============================================================================

void handleSerialInput() {
    while (Serial.available() > 0) {
        char inChar = (char)Serial.read();
        
        // Commandes directes (caractères uniques)
        if (inChar == '\n' || inChar == '\r') {
            // Traitement buffer si nombre entré
            if (serialBuffer.length() > 0) {
                processNumberInput(serialBuffer);
                serialBuffer = "";
            }
        } else if (inChar >= '0' && inChar <= '9') {
            // Accumulation chiffres
            serialBuffer += inChar;
        } else {
            // Commande caractère
            processCommand(inChar);
            serialBuffer = "";
        }
    }
}

void processCommand(char cmd) {
    switch (cmd) {
        // Changement de mode
        case 'd':
        case 'D':
            flightMode.setMode(PowerDistribution::FlightMode::DECOLLAGE);
            Serial.println(F("\n[CMD] Changement mode → DÉCOLLAGE"));
            arinc.sendFlightMode(PowerDistribution::FlightMode::DECOLLAGE);
            sendCurrentStatus();
            break;
        
        case 'n':
        case 'N':
            flightMode.setMode(PowerDistribution::FlightMode::NORMAL);
            Serial.println(F("\n[CMD] Changement mode → NORMAL"));
            arinc.sendFlightMode(PowerDistribution::FlightMode::NORMAL);
            sendCurrentStatus();
            break;
        
        case 'u':
        case 'U':
            flightMode.setMode(PowerDistribution::FlightMode::URGENCE);
            Serial.println(F("\n[CMD] Changement mode → URGENCE"));
            arinc.sendFlightMode(PowerDistribution::FlightMode::URGENCE);
            sendCurrentStatus();
            break;
        
        // Ajustement puissance
        case '+':
            flightMode.increasePower(ENCODER_STEP);
            Serial.print(F("\n[CMD] Puissance +"));
            Serial.print(ENCODER_STEP);
            Serial.println(F(" Cv"));
            sendCurrentStatus();
            break;
        
        case '-':
            flightMode.decreasePower(ENCODER_STEP);
            Serial.print(F("\n[CMD] Puissance -"));
            Serial.print(ENCODER_STEP);
            Serial.println(F(" Cv"));
            sendCurrentStatus();
            break;
        
        // Status système
        case 's':
        case 'S':
            Serial.println(F("\n[CMD] Status système complet"));
            sendFullDashboard();
            break;
        
        // Aide
        case 'h':
        case 'H':
        case '?':
            printHelp();
            break;
        
        // Reset
        case 'r':
        case 'R':
            Serial.println(F("\n[CMD] Reset système..."));
            resetSystem();
            break;
        
        default:
            // Commande inconnue
            if (cmd != ' ' && cmd != '\t') {
                Serial.print(F("\n[WARN] Commande inconnue: "));
                Serial.println(cmd);
            }
            break;
    }
}

void processNumberInput(String numberStr) {
    long value = numberStr.toInt();
    
    if (value < 0) {
        Serial.println(F("\n[ERROR] Valeur négative invalide"));
        return;
    }
    
    if (value > 65535) {
        Serial.println(F("\n[ERROR] Valeur trop grande (max 65535)"));
        return;
    }
    
    flightMode.setTotalPower((uint16_t)value);
    Serial.print(F("\n[CMD] Puissance définie: "));
    Serial.print(value);
    Serial.println(F(" Cv"));
    
    sendCurrentStatus();
}

// ============================================================================
// AFFICHAGE
// ============================================================================

void updateDisplay() {
    // Update silencieux (pas d'affichage continu)
    // Utiliser commande 's' pour voir le status
}

void sendCurrentStatus() {
    // Calcul distribution actuelle
    PowerDistribution::PowerOutput output = powerCalc.calculate(
        flightMode.getMode(),
        flightMode.getTotalPower()
    );
    
    // Envoi status formaté
    arinc.sendFullStatus(
        flightMode.getMode(),
        output.total,
        output.electric,
        output.thermal
    );
}

void sendFullDashboard() {
    // Calcul distribution actuelle
    PowerDistribution::PowerOutput output = powerCalc.calculate(
        flightMode.getMode(),
        flightMode.getTotalPower()
    );
    
    // Envoi dashboard complet avec barres
    arinc.sendDashboard(
        flightMode.getMode(),
        output.total,
        output.electric,
        output.thermal
    );
}

void sendARINCData() {
    // Transmission ARINC périodique (silencieuse)
    // Peut être activée pour monitoring temps réel
}

// ============================================================================
// UTILITAIRES
// ============================================================================

void printHelp() {
    Serial.println(F(""));
    Serial.println(F("╔════════════════════════════════════════════════════════════════╗"));
    Serial.println(F("║                      COMMANDES DISPONIBLES                     ║"));
    Serial.println(F("╠════════════════════════════════════════════════════════════════╣"));
    Serial.println(F("║  MODES DE VOL:                                                 ║"));
    Serial.println(F("║    d - Mode DÉCOLLAGE (Electric 0-1000, Thermal 0-2250)       ║"));
    Serial.println(F("║    n - Mode NORMAL (Thermal only 0-2750)                       ║"));
    Serial.println(F("║    u - Mode URGENCE (Electric 0-1000, Thermal 0-2750)         ║"));
    Serial.println(F("║                                                                ║"));
    Serial.println(F("║  CONTRÔLE PUISSANCE:                                           ║"));
    Serial.println(F("║    + - Augmenter puissance (+10 Cv)                            ║"));
    Serial.println(F("║    - - Diminuer puissance (-10 Cv)                             ║"));
    Serial.println(F("║    <nombre> - Définir puissance exacte (ex: 1500)              ║"));
    Serial.println(F("║                                                                ║"));
    Serial.println(F("║  SYSTÈME:                                                      ║"));
    Serial.println(F("║    s - Afficher status complet + dashboard                     ║"));
    Serial.println(F("║    h - Afficher cette aide                                     ║"));
    Serial.println(F("║    r - Reset système                                           ║"));
    Serial.println(F("╚════════════════════════════════════════════════════════════════╝"));
    Serial.println(F(""));
}

void resetSystem() {
    // Reset au mode DÉCOLLAGE
    flightMode.setMode(PowerDistribution::FlightMode::DECOLLAGE);
    flightMode.setTotalPower(DecollageConfig::INITIAL_POWER);
    
    Serial.println(F("[SYSTEM] Reset complet - Mode DÉCOLLAGE - 50 Cv"));
    
    sendCurrentStatus();
}
