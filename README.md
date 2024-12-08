# **ft_irc - Plan de travail**

| **Étape**                   | **Tâches clés**                                                                                                         | **Responsable** | **Durée estimée**       | **Remarques**                                                                                 |
|------------------------------|-------------------------------------------------------------------------------------------------------------------------|------------------|--------------------------|------------------------------------------------------------------------------------------------|
| **1. Initialisation**        | - Configuration de l'architecture.<br>- Création du Makefile.<br>- Test initial de compilation.                          | Ensemble         | 4-5 heures (1 jour)     | Travail facile à terminer rapidement si vous suivez une structure claire.                     |
| **2. Backend serveur**       | - Configuration des sockets.<br>- Gestion des connexions (accept, listen).<br>- Mise en place de `poll`.                | Personne 1       | 16-18 heures (2 jours)  | Inclut la gestion des clients et des entrées/sorties non bloquantes.                          |
| **3. Commandes IRC**         | - Implémentation des commandes obligatoires (NICK, USER, JOIN, PRIVMSG, etc.).                                          | Personne 2       | 18-20 heures (2 jours)  | Certaines commandes peuvent être complexes (JOIN, gestion des canaux, rôles).                |
| **4. Gestion des canaux**    | - Gestion des messages dans les canaux.<br>- Commandes avancées (KICK, TOPIC, MODE).                                    | Personne 2       | 16-18 heures (2 jours)  | Peut être fait en parallèle avec les commandes de base.                                       |
| **5. Intégration**           | - Connecter backend et commandes.<br>- Tester le fonctionnement global.                                                | Ensemble         | 8-10 heures (1 jour)    | Nécessite beaucoup de tests pour vérifier que tout fonctionne ensemble.                       |
| **6. Tests unitaires**       | - Création de scripts de test.<br>- Validation des fonctionnalités et des cas limites.                                  | Ensemble         | 9-12 heures (1 jour)    | Inclut des tests de performance et de robustesse (connexions multiples, latence).             |
| **7. Partie bonus**          | - Implémentation du bot IRC.<br>- Envoi de fichiers.                                                                    | Ensemble         | 9-12 heures (1 jour)    | Optionnel : Réalisé uniquement si la partie obligatoire est terminée.                         |
| **8. Finalisation et rendu** | - Nettoyage du code.<br>- Préparation du dépôt Git.<br>- Répétition pour la soutenance.                                  | Ensemble         | 9 heures (1 jour)       | Vérifiez que tout est conforme à la norme C++98 et prêt pour l’évaluation.                    |

---

## **Structure suggérée des fichiers**
