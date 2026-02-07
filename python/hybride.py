#!/usr/bin/env python3
"""
Système de propulsion hybride push-pull - Version minimaliste
5 phases de vol avec distribution de puissance
"""

from enum import Enum
from dataclasses import dataclass

# Constante: 100% = 2200W
PUISSANCE_MAX_W = 2200.0


class Phase(Enum):
    """Phases de vol."""
    TAKEOFF = 1
    CLIMB = 2
    CRUISE = 3
    DESCENT = 4
    LANDING = 5


@dataclass
class Distribution:
    """Distribution de puissance pour une phase."""
    pull: float          # Hélice pull (%)
    push: float          # Hélice push (%)
    generator: float     # Générateur (%)
    pull_w: float = 0.0      # Hélice pull (W)
    push_w: float = 0.0      # Hélice push (W)
    generator_w: float = 0.0 # Générateur (W)


# Configuration des phases (valeurs par aile)
PHASES = {
    Phase.TAKEOFF: Distribution(pull=80.0, push=20.0, generator=0.0),
    Phase.CLIMB:   Distribution(pull=60.0, push=20.0, generator=20.0),
    Phase.CRUISE:  Distribution(pull=55.0, push=20.0, generator=25.0),
    Phase.DESCENT: Distribution(pull=30.0, push=20.0, generator=50.0),
    Phase.LANDING: Distribution(pull=80.0, push=20.0, generator=0.0),
}


def pourcent_vers_watt(pourcent: float) -> float:
    """Convertit un pourcentage en Watts."""
    return (pourcent / 100.0) * PUISSANCE_MAX_W


def calculer_puissance(phase: Phase, commande_pilote: float) -> Distribution:
    """
    Calcule la distribution de puissance.
    
    Args:
        phase: Phase de vol
        commande_pilote: Pourcentage demandé (0-100%)
    
    Returns:
        Distribution de puissance
    """
    config = PHASES[phase]
    total = config.pull + config.push
    ratio = commande_pilote / 100.0
    
    pull_pct = config.pull * ratio
    push_pct = config.push * ratio
    gen_pct = 80.0 - pull_pct  # Turbine constante à 80%
    
    return Distribution(
        pull=pull_pct,
        push=push_pct,
        generator=gen_pct,
        pull_w=pourcent_vers_watt(pull_pct),
        push_w=pourcent_vers_watt(push_pct),
        generator_w=pourcent_vers_watt(gen_pct)
    )


def afficher_etat(phase: Phase, commande: float = 100.0):
    """Affiche l'état du système pour une phase donnée."""
    dist = calculer_puissance(phase, commande)
    print("\n" + "=" * 60)
    print(f"PHASE: {phase.name} | Commande: {commande:.0f}%")
    print("=" * 60)
    print(f"  Pull:      {dist.pull:5.1f}% = {dist.pull_w:6.0f}W")
    print(f"  Push:      {dist.push:5.1f}% = {dist.push_w:6.0f}W")
    print(f"  Generator: {dist.generator:5.1f}% = {dist.generator_w:6.0f}W")
    print(f"  Turbine:   80.0% = {pourcent_vers_watt(80.0):6.0f}W (constant)")
    print("=" * 60)


def simulateur():
    """Simulateur interactif de vol."""
    phase_actuelle = Phase.CRUISE
    commande = 100.0
    
    print("\n" + "█" * 60)
    print("█" + " SIMULATEUR HYBRIDE PUSH-PULL ".center(58) + "█")
    print("█" * 60)
    print("\nCommandes:")
    print("  1 = TAKEOFF  |  2 = CLIMB  |  3 = CRUISE")
    print("  4 = DESCENT  |  5 = LANDING")
    print("  S = Afficher état  |  Q = Quitter")
    
    afficher_etat(phase_actuelle, commande)
    
    while True:
        try:
            cmd = input("\n> ").strip().upper()
            
            if cmd == 'Q':
                print("\nSimulation terminée.")
                break
            elif cmd == 'S':
                afficher_etat(phase_actuelle, commande)
            elif cmd == '1':
                phase_actuelle = Phase.TAKEOFF
                afficher_etat(phase_actuelle, commande)
            elif cmd == '2':
                phase_actuelle = Phase.CLIMB
                afficher_etat(phase_actuelle, commande)
            elif cmd == '3':
                phase_actuelle = Phase.CRUISE
                afficher_etat(phase_actuelle, commande)
            elif cmd == '4':
                phase_actuelle = Phase.DESCENT
                afficher_etat(phase_actuelle, commande)
            elif cmd == '5':
                phase_actuelle = Phase.LANDING
                afficher_etat(phase_actuelle, commande)
            else:
                print("Commande inconnue. Tapez 1-5, S ou Q")
                
        except KeyboardInterrupt:
            print("\n\nSimulation interrompue.")
            break
        except EOFError:
            break


if __name__ == "__main__":
    simulateur()
