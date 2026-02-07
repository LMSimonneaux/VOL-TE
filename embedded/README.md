# Système Embarqué de Gestion Puissance Hybride
## Safran PW100 - Hackathon FlyImpulse 2026

### 📋 Description

Système embarqué STM32 répliquant exactement la logique de l'interface web `interface.html`. Gestion de la distribution de puissance hybride électrique/thermique pour turbopropulseur.

---

## 🛠️ Matériel Requis

### Microcontrôleur
- **STM32** (recommandé: STM32F103C8T6 "Blue Pill" ou équivalent)
- Alternative: toute carte Arduino compatible (Uno, Mega, Due, etc.)

### Périphériques (optionnels pour version complète)
- Encodeur rotatif pour ajustement puissance
- 2 boutons pour changement de mode
- LED de status
- Écran LCD (pour affichage local)

### Câblage Serial
- Connexion USB-Serial pour moniteur terminal

---

## 💻 Installation Arduino IDE

### 1. Support STM32 pour Arduino IDE

```bash
# Ajout du board manager URL dans Arduino IDE:
# File → Preferences → Additional Board Manager URLs
https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
```

### 2. Installation des boards STM32

1. Ouvrir **Tools → Board → Boards Manager**
2. Chercher "STM32"
3. Installer **"STM32 MCU based boards by STMicroelectronics"**

### 3. Configuration du projet

1. Copier tout le dossier `embedded/` sur votre machine
2. Ouvrir `PowerManagement/PowerManagement.ino` dans Arduino IDE
3. Sélectionner votre board: **Tools → Board → STM32 boards → Your Board**
4. Sélectionner le port série: **Tools → Port → COMx** (Windows) ou **/dev/ttyUSBx** (Linux/Mac)

---

## 🏗️ Structure du Projet

```
embedded/
├── PowerManagement/
│   └── PowerManagement.ino      # Programme principal Arduino
├── config.h                      # Configuration globale
├── PowerDistribution.h/.cpp      # Calcul distribution puissance
├── FlightMode.h/.cpp             # Gestion modes de vol
├── ARINCSimulator.h/.cpp         # Simulation protocole ARINC 429
└── README.md                     # Ce fichier
```

### Architecture Modulaire

- **config.h**: Constantes, pins, paramètres par mode
- **PowerDistribution**: Logique électrique/thermique (réplique `interface.html`)
- **FlightMode**: State machine des modes (Décollage/Normal/Urgence)
- **ARINCSimulator**: Formatage sorties série style avionique
- **PowerManagement.ino**: Boucle principale, commandes série

---

## 📡 Compilation et Upload

### Via Arduino IDE

1. **Vérifier**: Sketch → Verify/Compile
2. **Upload**: Sketch → Upload
3. **Moniteur série**: Tools → Serial Monitor (115200 baud)

### Via cli (optionnel)

```bash
arduino-cli compile --fqbn STMicroelectronics:stm32:GenF1:pnum=BLUEPILL_F103C8 PowerManagement/
arduino-cli upload -p /dev/ttyUSB0 --fqbn STMicroelectronics:stm32:GenF1:pnum=BLUEPILL_F103C8 PowerManagement/
```

---

## 🎮 Utilisation

### Connexion

1. Brancher le STM32 en USB
2. Ouvrir le moniteur série (115200 baud)
3. Le système affiche le banner de démarrage

### Commandes Série

| Commande | Description | Exemple |
|----------|-------------|---------|
| `d` | Mode **DÉCOLLAGE** | `d` |
| `n` | Mode **NORMAL** | `n` |
| `u` | Mode **URGENCE** | `u` |
| `+` | Augmenter puissance (+10 Cv) | `+` |
| `-` | Diminuer puissance (-10 Cv) | `-` |
| `1500` | Définir puissance exacte | `1500` → 1500 Cv |
| `s` | Afficher status complet | `s` |
| `h` | Aide | `h` |
| `r` | Reset système | `r` |

### Exemple de Session

```
> d                    // Mode DÉCOLLAGE
> 1500                 // 1500 Cv total
> s                    // Status complet

┌────────────────────────────────────────────────────────────┐
│ MODE: DECOLLAGE                                             │
├────────────────────────────────────────────────────────────┤
│ Puissance Totale:      1500 Cv                             │
│ Puissance Électrique:  1000 Cv                             │
│ Puissance Thermique:   500 Cv                              │
└────────────────────────────────────────────────────────────┘

> n                    // Mode NORMAL
> s

┌────────────────────────────────────────────────────────────┐
│ MODE: NORMAL                                                │
├────────────────────────────────────────────────────────────┤
│ Puissance Totale:      1500 Cv                             │
│ Puissance Électrique:  0 Cv                                │
│ Puissance Thermique:   1500 Cv                             │
└────────────────────────────────────────────────────────────┘
```

---

## 🔧 Configuration Avancée

### Modification des Paramètres

Éditer `config.h` pour changer:

- **Pins**: Encodeur, boutons, LED
- **Limites de puissance** par mode
- **Baudrate série**
- **Intervalles de mise à jour**

### Exemple: Changer le plafond électrique en mode URGENCE

```cpp
// Dans config.h, ligne ~30
namespace UrgenceConfig {
    constexpr uint16_t ELECTRIC_MAX = 1200U;  // Ancien: 1000U
}
```

---

## 📊 Logique de Distribution

### Mode DÉCOLLAGE
```
Electric: min(TOTAL, 1000 Cv)
Thermal:  min(TOTAL - Electric, 2250 Cv)
Max Total: 3250 Cv
```

### Mode NORMAL
```
Electric: 0 Cv (thermique seul)
Thermal:  min(TOTAL, 2750 Cv)
Max Total: 2750 Cv
```

### Mode URGENCE
```
Electric: min(TOTAL, 1000 Cv)
Thermal:  min(TOTAL - Electric, 2750 Cv)
Max Total: 3750 Cv
```

**Cette logique est identique à `interface.html`**

---

## 🚀 Prochaines Étapes (Extensions)

### Hardware
- [ ] Intégrer encodeur rotatif physique (pins 2-3)
- [ ] Ajouter boutons mode (pins 4-5)
- [ ] Connecter écran LCD I2C pour affichage local
- [ ] LED RGB pour indication mode visuelle

### Software
- [ ] Sauvegarde EEPROM (mode/puissance au redémarrage)
- [ ] Watchdog timer pour sécurité
- [ ] Datalogger SD card (historique puissance)
- [ ] Interface Bluetooth pour monitoring mobile

### Avionique
- [ ] ARINC 429 matériel (HI-8585 transceiver)
- [ ] CAN bus pour intégration avionics suite
- [ ] Gestion DO-254 I/O discretes
- [ ] Redondance capteurs

---

## 🔍 Débogage

### Problème: Compilation échoue

**Solution**: Vérifier que tous les fichiers (.h, .cpp) sont dans le même dossier que le .ino

### Problème: Upload échoue

**Solutions**:
1. Vérifier le bon port série sélectionné
2. Presser le bouton RESET sur STM32 pendant l'upload
3. Installer driver USB-Serial (CH340 pour Blue Pill)

### Problème: Caractères bizarres dans le moniteur série

**Solution**: Vérifier baudrate = **115200 baud** dans Tools → Serial Monitor

### Problème: Commandes ne fonctionnent pas

**Solution**: 
1. Vérifier "No line ending" ou "Newline" dans Serial Monitor
2. Envoyer les commandes une par une
3. Taper `h` pour afficher l'aide

---

## 📝 Conformité Standards

### Inspiré de:
- **DO-178C**: Structure modulaire, typage strict, pas de malloc
- **MISRA C++**: Const correctness, explicit types, defensive programming
- **ARINC 429**: Format messages (simulé sur Serial)

### Limitations Hackathon:
- Pas de certification formelle
- Serial au lieu d'ARINC matériel
- Pas de redondance
- Testing manuel (pas de DO-178C coverage)

---

## 👥 Auteurs

**Hackathon FlyImpulse 2026** - Équipe Safran PW100

---

## 📄 Licence

Projet Hackathon - Usage éducatif et démonstration uniquement
