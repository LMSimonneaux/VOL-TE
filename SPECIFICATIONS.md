# Système de Propulsion Hybride Push-Pull - Spécifications

## Situation

Vous développez un système de propulsion hybride innovant pour un avion utilisant une architecture push-pull. **L'avion possède deux ailes, chacune équipée d'une paire de moteurs push-pull.**

Chaque aile dispose de deux hélices : une hélice pull alimentée par une turbine thermique et une hélice push alimentée par un moteur électrique sur batterie. La turbine fournit une puissance constante de 80% de la puissance maximale disponible. Cette puissance se répartit dynamiquement entre l'hélice pull et un générateur électrique qui charge la batterie alimentant le moteur électrique de l'hélice push. Le système doit s'adapter automatiquement selon cinq phases de vol distinctes (décollage, montée, palier, descente, atterrissage), chacune ayant des exigences spécifiques de distribution de puissance.

### Architecture Systèmepilote (pourcentage de puissance demandée via curseur) en une distribution optimale et automatique de la puissance entre l'hélice pull (turbine), l'hélice push (moteur électrique), et le générateur électrique (charge/décharge batterie), tout en respectant les contraintes de chaque phase de vol.

L'algorithme doit gérer **les deux ailes de l'avion**, soit en mode synchronisé (même commande pour gauche et droite), soit en mode asymétrique (commandes indépendantes pour chaque aile)

**IMPORTANT** : L'avion comporte **deux paires de moteurs push-pvia un ou deux curseurs (mode synchronisé ou asymétrique), pendant que le système gère automatiquement la répartition complexe de puissance selon la phase de vol pour chaque ailete doit avoir accès au contrôle des deux ailes :
- **Aile gauche** : 1 turbine + 1 générateur + 1 hélice pull + 1 hélice push + 1 batterie
- **Aile droite** : 1 turbine + 1 générateur + 1 hélice pull + 1 hélice push + 1 batterie

Le système de gestion doit permettre au pilote de contrôler les deux paires de moteurs, soit de manière synchronisée (commande unique pour les deux ailes), soit de manière indépendante (commandes différenciées gauche/droite) pour permettre des manœuvres asymétriques si nécessaire.

**Configuration bi-moteur** :
- L'avion possède **2 paires de moteurs push-pull** (une par aile : gauche et droite)
- Chaque aile est un système indépendant avec sa propre turbine, générateur et batterie
- Toutes les valeurs ci-dessous sont **par aile**

**Contraintes par aile** :
- La turbine fournit toujours 80% de la puissance maximale (constante)
- Puissance maximale disponible = 100% par aile
**Note** : Toutes les valeurs ci-dessous sont **par aile**. Pour obtenir la puissance totale de l'avion, multiplier par 2.

#### Phase 1 (Décollage)
- Puissance totale hélices (par aile) convertit l'excédent de puissance turbine en charge batterie
- La batterie alimente le moteur électrique de l'hélice push
- Chaque aile peut être contrôlée indépendamment ou en mode synchronisé

Permettre au pilote de contrôler simplement la puissance totale de chaque aile via un curseur unique, pendant que le système gère automatiquement la répartition complexe de puissance selon la phase de vol, garantissant ainsi la sécurité, l'efficacité énergétique et le respect des contraintes techniques du système hybride.

## Knowledge

### Contraintes du système

- La turbine fournit toujours 80% de la puissance maximale (constante)
- Puissance maximale disponible = 100%
- Deux hélices par aile : pull (turbine) et push (moteur électrique)
- Le générateur électrique convertit l'excédent de puissance turbine en charge batterie
- La batterie alimente le moteur électrique de l'hélice push

### Répartition de puissance par phase de vol

#### Phase 1 (Décollage)
- Puissance totale hélices : 100%
- Hélice pull (turbine) : 80%
- Hélice push (moteur électrique) : 20%
- Générateur électrique : 0%
- État batterie : décharge maximale

#### Phase 2 (Montée)
- Puissance totale hélices : 80%
- Hélice pull (turbine) : 60%
- Hélice push (moteur électrique) : 20%
- Générateur électrique : 20%
- État batterie : stable (ni charge ni décharge)

#### Phase 3 (Palier)
- Puissance totale hélices : 75%
- Hélice pull (turbine) : 55%
- Hélice push (moteur électrique) : 20%
- Générateur électrique : 25%
- État batterie : recharge

#### Phase 4 (Descente)
- Puissance totale hélices : 50%
- Hélice pull (turbine) : 30%
- Hélice push (moteur électrique) : 20%
- Générateur électrique : 50%
- État batterie : recharge maximale

#### Phase 5 (Atterrissage)
- Identique à Phase 1 (Décollage)

### Contraintes de sécurité critiques

- Niveau minimal de batterie : 20% (seuil de sécurité absolu)
- Durée des transitions entre phases : 5 secondes (transitions progressives obligatoires)
- Aucune variation brusque de puissance autorisée

### Environnement de développement

- Phase de simulation : Pythonles paramètres suivants :

**Mode synchronisé** (commande unique pour les deux ailes) :
- Le pourcentage de puissance demandée par le pilote (0-100%)
- La phase de vol actuelle (1-5)
- Le niveau de charge actuel des batteries (% gauche et % droite)

**Mode asymétrique** (commandes indépendantes) :
- Le pourcentage de puissance aile gauche (0-100%)
- Le pourcentage de puissance aile droite (0-100%)
- La phase de vol actuelle (1-5)
- Le niveau de charge batterie gauche (%)
- Le niveau de charge batterie droit

Schéma draw.io disponible illustrant le système complet : `HackathonFly.drawio.html`

## Instructions pour l'algorithme

### 1. Paramètres d'entrée

L'algorithme doit prendre en entrée trois paramètres :
- Le pourcentage de puissance demandée par le pilote (0-100%)
- La phase de vol actuelle (1-5)
- Le niveau de charge actuel de la batterie (%)

### 2. Calcul de répartition de puissance

L'algorithme doit calculer automatiquement la répartition selon la logique suivante :
- Identifier la phase de vol active
- Appliquer les ratios de distribution correspondants à cette phase
- Calculer la puissance pour l'hélice pull = (pourcentage pilote × ratio turbine de la phase) / puissance totale de la phase
- Calculer la puissance pour l'hélice push = (pourcentage pilote × 20%) / puissance totale de la phase
- Calculer la puissance du générateur = 80% - puissance hélice pull
- Vérifier que la somme (hélice pull + hélice push) = pourcentage demandé par le pilote

### 3. Transitions progressives entre phases

Implémenter des transitions progressives sur exactement 5 secondes :
- Lors d'un changement de phase, interpoler linéairement les valeurs de puissance entre l'état actuel et l'état cible
- Diviser la transition en pas de temps réguliers (par exemple 50ms) pour assurer une progression fluide
- Maintenir la cohérence de la distribution pendant toute la durée de la transition
- Empêcher tout nouveau changement de phase pendant qu'une transition est en cours
**pour chaque aile** :
- Puissance totale demandée (%)
- Phase de vol active
- Puissance hélice pull (% et kW si puissance max connue)
- Puissance hélice push (% et kW si puissance max connue)
- Puissance générateur électrique (% et kW si puissance max connue)
- État de la batterie (décharge/stable/recharge) avec taux (%) et niveau de charge actuel
- Statut de transition (en cours/terminée) avec temps restant si applicable
- Alertes de sécurité actives (batterie faible, limite de phase, etc.)

**Affichage global** :
- Puissance totale avion (somme des deux ailes)
- Symétrie/Asymétrie de la distribution gauche-droite
- Visualisation graphique comparative des deux ailesvec recommandation de changer de phase de vol
- En Phase 1 (décollage) ou Phase 5 (atterrissage), si la batterie est à 20%, interdire le décollage ou forcer un atterrissage d'urgence avec puissance réduite

### 5. Gestion des cas limites

- Si le pourcentage demandé dépasse la puissance maximale disponible pour la phase, plafonner à la limite de la phase et alerter le pilote
- Si la batterie est pleine (100%) en Phase 3 ou 4, ajuster la distribution pour rediriger l'excédent du générateur vers l'hélice pull afin d'éviter la surcharge
- Si une incohérence est détectée dans les calculs (somme ≠ 100%), déclencher une alerte système et basculer en mode de sécurité avec distribution par défaut

### 6. Génération de sortie structurée

L'algorithme doit afficher :
- Puissance totale demandée (%)
- Phase de vol active
- Puissance hélice pull (% et kW si puissance max connue)
- Puissance hélice push (% et kW si puissance max c (par aile)
- **Module de gestion des transitions** (par aile)
- **Module de surveillance batterie et sécurité** (par aile)
- **Module de gestion bi-moteur** (coordination gauche-droite, détection asymétrie)
- **Module d'interface pilote et affichage** (mode synchronisé/asymétrique)

Cette modularité permettra des ajustements futurs des ratios de distribution par phase, l'ajout de nouvelles fonctionnalités de sécurité, et la gestion avancée des manœuvres asymétriques

### 7. Mode de diagnostic

Inclure un mode de diagnostic permettant de :
- Simuler différentes commandes pilote et phases pour valider le comportement du système
- Tester les scénarios de batterie faible et les transitions entre phases
- Générer des logs détaillés pour analyse post-vol
- Vérifier la conformité avec les contraintes de sécurité

## Livrables

L'algorithme doit être développé en deux versions :
- **Version Python** pour simulation et validation du comportement avec interface de test
- **Version C++** optimisée pour l'embarqué avec architecture temps réel et gestion mémoire stricte

## Architecture modulaire

L'architecture doit être modulaire avec séparation claire entre :
- **Module de calcul de distribution de puissance**
- **Module de gestion des transitions**
- **Module de surveillance batterie et sécurité**
- **Module d'interface pilote et affichage**

Cette modularité permettra des ajustements futurs des ratios de distribution par phase et l'ajout de nouvelles fonctionnalités de sécurité.
