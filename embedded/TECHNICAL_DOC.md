# Documentation Technique - Système Embarqué
## Architecture et Spécifications

### 🏛️ Architecture Logicielle

```
┌─────────────────────────────────────────────────────────────┐
│                    PowerManagement.ino                       │
│                   (Boucle Principale)                        │
│  • Setup Hardware                                            │
│  • Loop: handleSerialInput() → processCommand()             │
│  • Scheduling updates (100ms display, 50ms ARINC)           │
└──────────────┬──────────────┬───────────────┬───────────────┘
               │              │               │
       ┌───────▼──────┐  ┌────▼────────┐ ┌───▼──────────────┐
       │  FlightMode  │  │PowerDistrib │ │ ARINCSimulator   │
       │              │  │             │ │                  │
       │ • DECOLLAGE  │  │ • calculate │ │ • sendStatus()   │
       │ • NORMAL     │  │   Decollage │ │ • sendDashboard()│
       │ • URGENCE    │  │ • calculate │ │ • formatLabel()  │
       │              │  │   Normal    │ │                  │
       │ • setMode()  │  │ • calculate │ │                  │
       │ • setTotal   │  │   Urgence   │ │                  │
       │   Power()    │  │             │ │                  │
       └──────────────┘  └─────────────┘ └──────────────────┘
               │              │               │
               └──────────────┴───────────────┘
                              │
                    ┌─────────▼─────────┐
                    │     config.h      │
                    │                   │
                    │ • DecollageConfig │
                    │ • NormalConfig    │
                    │ • UrgenceConfig   │
                    │ • ARINC Labels    │
                    │ • Pin Mapping     │
                    └───────────────────┘
```

---

### 🔄 Diagramme de Flux de Données

```
┌─────────────┐
│   Commande  │ (Serial: 'd', 'n', 'u', '+', '-', '1500')
│   Série     │
└──────┬──────┘
       │
       ▼
┌─────────────────────────────────┐
│  handleSerialInput()            │
│  • Accumule buffer pour nombres │
│  • Détecte commandes char       │
└──────┬──────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  processCommand(cmd)            │
│  • 'd','n','u' → FlightMode     │
│  • '+','-' → adjust power       │
│  • number → setTotalPower()     │
└──────┬──────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  FlightMode::setMode()          │
│  ou setTotalPower()             │
│  • Update internal state        │
│  • Apply constraints            │
└──────┬──────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  PowerDistribution::calculate() │
│  • Switch sur mode actif        │
│  • Appelle calculateXXX()       │
│  • Return {electric, thermal}   │
└──────┬──────────────────────────┘
       │
       ▼
┌─────────────────────────────────┐
│  ARINCSimulator::sendStatus()  │
│  • Formate output ARINC-like    │
│  • Serial.println() dashboard   │
└──────┬──────────────────────────┘
       │
       ▼
┌─────────────┐
│  Terminal   │ (Affichage formaté avec box drawing)
│  Utilisateur│
└─────────────┘
```

---

### 🧮 Algorithme de Distribution par Mode

#### Mode DÉCOLLAGE

```cpp
Input: totalPower (Cv)

electricPower = min(totalPower, ELECTRIC_MAX)   // Cap à 1000 Cv
remaining = totalPower - electricPower
thermalPower = min(remaining, THERMAL_MAX)      // Cap à 2250 Cv

Output: {electric: electricPower, thermal: thermalPower}

Exemples:
  500 Cv  → Electric: 500,  Thermal: 0
  1500 Cv → Electric: 1000, Thermal: 500
  3000 Cv → Electric: 1000, Thermal: 2000
  4000 Cv → Electric: 1000, Thermal: 2250 (plafonné)
```

#### Mode NORMAL

```cpp
Input: totalPower (Cv)

electricPower = 0                               // Pas d'électrique
thermalPower = min(totalPower, THERMAL_MAX)     // Cap à 2750 Cv

Output: {electric: 0, thermal: thermalPower}

Exemples:
  500 Cv  → Electric: 0,    Thermal: 500
  1500 Cv → Electric: 0,    Thermal: 1500
  3000 Cv → Electric: 0,    Thermal: 2750 (plafonné)
```

#### Mode URGENCE

```cpp
Input: totalPower (Cv)

electricPower = min(totalPower, ELECTRIC_MAX)   // Cap à 1000 Cv
remaining = totalPower - electricPower
thermalPower = min(remaining, THERMAL_MAX)      // Cap à 2750 Cv

Output: {electric: electricPower, thermal: thermalPower}

Exemples:
  500 Cv  → Electric: 500,  Thermal: 0
  1500 Cv → Electric: 1000, Thermal: 500
  3000 Cv → Electric: 1000, Thermal: 2000
  4000 Cv → Electric: 1000, Thermal: 2750 (plafonné)
```

---

### 🎯 Machine à États - FlightMode

```
                    ┌──────────────┐
                    │              │
           ┌────────▶  DÉCOLLAGE   ◀────────┐
           │        │              │        │
           │        └──────┬───────┘        │
           │               │                │
           │          nextMode()            │
           │               │                │
           │               ▼                │
           │        ┌──────────────┐        │
           │        │              │        │
      nextMode()    │    NORMAL    │   nextMode()
           │        │              │        │
           │        └──────┬───────┘        │
           │               │                │
           │          nextMode()            │
           │               │                │
           │               ▼                │
           │        ┌──────────────┐        │
           │        │              │        │
           └────────│   URGENCE    │────────┘
                    │              │
                    └──────────────┘

Chaque transition:
  1. appelle setMode(newMode)
  2. applyPowerConstraints() ajuste totalPower_ si hors limites
  3. sendFlightMode() notifie via ARINC
```

---

### 📊 Diagramme de Séquence - Changement de Mode

```
User    Serial   PowerManagement   FlightMode   PowerDist   ARINC
  │        │            │               │            │         │
  ├───'d'──▶            │               │            │         │
  │        │            │               │            │         │
  │        ├─processCmd─▶               │            │         │
  │        │            │               │            │         │
  │        │            ├─setMode(DEC)──▶            │         │
  │        │            │               │            │         │
  │        │            │               ├─apply──────┤         │
  │        │            │               │ Constraints│         │
  │        │            │               │            │         │
  │        │            ├─calculate()───┼────────────▶         │
  │        │            │               │            │         │
  │        │            │               │        {electric,    │
  │        │            │               │         thermal}     │
  │        │            │               │            │         │
  │        │            ├─sendStatus()──┼────────────┼─────────▶
  │        │            │               │            │         │
  │        │            │               │            │    Format
  │        │            │               │            │    ARINC
  │        │            │               │            │         │
  │◀───────┴────────────┴───────────────┴────────────┴─────────┤
  │   ┌─────────────────────────────────────────────────────┐  │
  │   │ MODE: DECOLLAGE                                      │  │
  │   │ Puissance Totale:      50 Cv                         │  │
  │   │ Puissance Électrique:  50 Cv                         │  │
  │   │ Puissance Thermique:   0 Cv                          │  │
  │   └─────────────────────────────────────────────────────┘  │
```

---

### 🔌 Pin Mapping (config.h)

```
STM32 Blue Pill     Fonction              Type
─────────────────────────────────────────────────────────
PC13                LED_STATUS_PIN        Output
PA2                 ENCODER_CLK_PIN       Input (interrupt)
PA3                 ENCODER_DT_PIN        Input (interrupt)
PA4                 BUTTON_NORMAL_PIN     Input (pull-up)
PA5                 BUTTON_URGENCE_PIN    Input (pull-up)
PA9  (TX)           Serial TX             UART
PA10 (RX)           Serial RX             UART

Notes:
- Encodeur rotatif: CLK/DT avec interruptions pour debouncing
- Boutons: Active LOW avec pull-up interne
- Serial: 115200 baud, 8N1
```

---

### ⏱️ Timing et Scheduling

```
Fonction              Intervalle    Priorité    Où
─────────────────────────────────────────────────────────
handleSerialInput()   Chaque loop   1 (High)    loop()
updateDisplay()       100 ms        2 (Medium)  loop()
sendARINCData()       50 ms         3 (Low)     loop()
LED Heartbeat         500 ms        4 (Low)     loop()

Timing Critique:
- Serial: Traité immédiatement (chaque loop)
- Display: 100ms suffisant pour lecture humaine
- ARINC: 50ms = 20Hz refresh rate (avionics standard)
```

---

### 🛡️ Gestion d'Erreurs

#### Overflow Protection

```cpp
// PowerDistribution.cpp - Fonction min()
uint16_t PowerDistribution::min(uint16_t a, uint16_t b) {
    return (a < b) ? a : b;  // Pas de calcul, pas d'overflow
}

// FlightMode.cpp - Increment
void FlightMode::increasePower(uint16_t increment) {
    uint16_t newPower = totalPower_ + increment;
    
    // Détection overflow
    if (newPower < totalPower_) {
        newPower = getMaxPower();
    }
    
    setTotalPower(newPower);
}
```

#### Underflow Protection

```cpp
// FlightMode.cpp - Decrement
void FlightMode::decreasePower(uint16_t decrement) {
    uint16_t newPower = 0U;
    
    // Prévention underflow
    if (totalPower_ > decrement) {
        newPower = totalPower_ - decrement;
    }
    
    setTotalPower(newPower);  // Au minimum: getMinPower()
}
```

#### Constraints Enforcement

```cpp
// FlightMode.cpp - Contraintes automatiques
void FlightMode::setTotalPower(uint16_t power) {
    uint16_t minPower = getMinPower();
    uint16_t maxPower = getMaxPower();
    
    // Clamping dans les limites du mode actif
    if (power < minPower) {
        totalPower_ = minPower;
    } else if (power > maxPower) {
        totalPower_ = maxPower;
    } else {
        totalPower_ = power;
    }
}
```

---

### 💾 Utilisation Mémoire (STM32F103C8)

```
Ressource         Utilisé    Disponible   %
─────────────────────────────────────────────
Flash (Code)      ~8 KB      64 KB        12%
RAM (Global)      ~500 B     20 KB        2.5%
Stack             ~1 KB      -            -

Optimisations MISRA-like:
- Pas de malloc/new (pas de fragmentation heap)
- Variables dans stack ou static
- String minimal (F() macro pour Flash)
- Pas de récursion
```

---

### 🔐 Conformité Standards (Partielle)

#### DO-178C Inspirations

| Exigence | Implémentation | Status |
|----------|----------------|--------|
| Modularité | Classes séparées (.h/.cpp) | ✅ |
| Typage strict | uint16_t, const correctness | ✅ |
| Pas de comportement indéfini | Overflow checks, clamps | ✅ |
| Traçabilité | Doxygen comments | ✅ |
| Testing | Manuel (pas de coverage tools) | ⚠️ |

#### MISRA C++ Guidelines

| Règle | Description | Respect |
|-------|-------------|---------|
| No dynamic allocation | Pas de new/delete | ✅ |
| Explicit types | uint16_t vs int | ✅ |
| Const correctness | const methods, params | ✅ |
| No undefined behavior | Checks overflow/underflow | ✅ |
| Limited side effects | Pure functions quand possible | ✅ |

#### ARINC 429 Simulation

| Feature | Real ARINC | Notre Implémentation |
|---------|------------|----------------------|
| Bitrate | 12.5/100 kbps | 115200 baud (Serial) |
| Label | 8 bits octal | Simulé (0x270, 0x271...) |
| Data | 32 bits | Formatage texte |
| Parity | Paire/Impaire | Checksum (non utilisé) |
| SDI/SSM | Status bits | Mode texte |

---

### 📈 Évolution du Code (Git-like)

```
v1.0.0 - Hackathon Demo
├── config.h                    # Configuration centralisée
├── PowerDistribution.h/.cpp    # Logique pure (testable)
├── FlightMode.h/.cpp           # State machine
├── ARINCSimulator.h/.cpp       # I/O formatting
└── PowerManagement.ino         # Intégration Arduino

Prochaines versions:
v1.1.0 - Hardware Integration
├── EncoderHandler.h/.cpp       # Rotary encoder ISR
├── ButtonManager.h/.cpp        # Debounce + state
└── LCDDisplay.h/.cpp           # I2C LCD driver

v2.0.0 - ARINC Matériel
├── ARINC429Bus.h/.cpp          # HI-8585 driver
├── CANBusInterface.h/.cpp      # Avionics suite integration
└── RedundancyManager.h/.cpp    # Dual channel
```

---

### 🧪 Procédure de Test (Manuel)

#### Test 1: Distribution Mode DÉCOLLAGE
```
1. Reset système (commande 'r')
2. Vérifier mode = DÉCOLLAGE
3. Entrer '500' → Electric: 500, Thermal: 0 ✓
4. Entrer '1500' → Electric: 1000, Thermal: 500 ✓
5. Entrer '4000' → Electric: 1000, Thermal: 2250 ✓
```

#### Test 2: Distribution Mode NORMAL
```
1. Commande 'n'
2. Vérifier mode = NORMAL
3. Entrer '500' → Electric: 0, Thermal: 500 ✓
4. Entrer '1500' → Electric: 0, Thermal: 1500 ✓
5. Entrer '3000' → Electric: 0, Thermal: 2750 ✓
```

#### Test 3: Distribution Mode URGENCE
```
1. Commande 'u'
2. Vérifier mode = URGENCE
3. Entrer '500' → Electric: 500, Thermal: 0 ✓
4. Entrer '1500' → Electric: 1000, Thermal: 500 ✓
5. Entrer '4000' → Electric: 1000, Thermal: 2750 ✓
```

#### Test 4: Transitions de Mode
```
1. Mode DÉCOLLAGE, 1500 Cv
2. Passage NORMAL → Thermal: 1500, Electric: 0 ✓
3. Passage URGENCE → Electric: 1000, Thermal: 500 ✓
4. Retour DÉCOLLAGE → Electric: 1000, Thermal: 500 ✓
   (Puissance préservée si dans limites)
```

#### Test 5: Contraintes
```
1. Mode NORMAL, entrer '5000'
   → Clamped à 2750 Cv ✓
2. Commande '-' 300 fois
   → Clamped à 0 Cv ✓
3. Overflow test: '+' jusqu'à max
   → Reste dans limites uint16_t ✓
```

---

### 📡 Format de Sortie ARINC-Like

#### Status Complet
```
┌────────────────────────────────────────────────────────────┐
│ MODE: DECOLLAGE                                             │
├────────────────────────────────────────────────────────────┤
│ Puissance Totale:      1500 Cv                             │
│ Puissance Électrique:  1000 Cv                             │
│ Puissance Thermique:   500 Cv                              │
└────────────────────────────────────────────────────────────┘
```

#### Dashboard avec Barres
```
╔════════════════════════════════════════════════════════════════╗
║  MODE: URGENCE                                                 ║
╠════════════════════════════════════════════════════════════════╣
║  TOTAL  [████████████████░░░░░░░░░░░░░░░░░░░░░░░░] 1500 Cv ║
║  ELEC   [████████████████████████████████████████] 1000 Cv  ║
║  THRM   [████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░] 500 Cv  ║
╚════════════════════════════════════════════════════════════════╝
```

---

### 🎓 Guides de Référence

- **DO-178C**: Software Considerations in Airborne Systems
- **MISRA C++:2008**: Guidelines for Critical Systems
- **ARINC 429**: Digital Information Transfer System
- **STM32 Reference**: RM0008 (STM32F10xxx)
- **Arduino STM32**: https://github.com/stm32duino/Arduino_Core_STM32
