# 🗂️ Norme d’organisation

## 🕘 Réunions d’équipe

### 🔹 **Daily Stand-up**

* **Fréquence** : Chaque jour de travail **sauf le lundi** (mardi et mercredi).
* **Heure** : 10h00 **dernier délai**.
* **Durée maximale** : 15 minutes.
* **Format** : stand-up (chacun répond à 3 questions) : 
  1. Ce que j’ai fait hier
  2. Ce que je vais faire aujourd’hui
  3. Problèmes ou blocages éventuels
* **Retranscription obligatoire** sur **Notion** après chaque daily.
* **Présence** : non obligatoire mais **fortement recommandée**.
* **Absents** : doivent **rattraper la retranscription** de manière autonome sur Notion.

---

### 🔹 **Weekly Meeting**

* **Fréquence** : une fois par semaine, le **lundi** (remplace le daily).
* **Heure** : 14h00 **dernier délai**.
* **Durée** : variable (30 à 60 minutes).
* **Objectifs** :
  * Définir les objectifs et tâches du sprint hebdomadaire.
  * Répartir les rôles et tickets.
  * Effectuer le **merge `dev → main`** si les conditions sont remplies.
* **Présence obligatoire** pour tous les membres actifs.
* **Retranscription obligatoire** sur **Notion** après chaque weekly.

---

## 🧩 Gestion des tâches et de l’avancement

### 🔹 **Tickets & suivi**

* La gestion des tâches se fait via **GitHub Projects**.
* Chaque ticket doit comporter :

  * Une **description claire** du besoin.
  * Une ou plusieurs **assignations**.
  * Une **deadline réaliste**.
  * Si applicable : une **milestone** (sprint).
* Les tickets doivent être **liés aux Pull Requests correspondantes** via l’onglet *Development*.

---

### 🔹 **Rushs / Sprints**

* Chaque **weekly** marque le début d’un nouveau **sprint**.
* Les tickets sont regroupés par **rush** dans GitHub Projects.
* Les tâches prioritaires doivent être placées dans la colonne **“In Progress”**.
* Les tickets terminés doivent être déplacés dans **“Done”** une fois la PR mergée.

---

## 💬 Communication interne

* Toute communication technique se fait sur le **Discord** du projet.
* Un salon spécifique `#absent-retard` est prévu pour :

  * Signaler toute **absence prévue**.
  * Prévenir tout **retard**.
* Les discussions importantes ou décisions techniques doivent être **résumées et archivées sur Notion**.

---

## 🧾 Bonnes pratiques d’équipe

* Respecter les horaires de réunion et **éviter les digressions** pendant les stand-ups.
* Ne jamais bloquer un autre membre sans communication (prévenir sur Discord en cas d’imprévu).
* Mettre à jour les tickets GitHub Projects dès qu’une tâche change d’état.
* En cas de désaccord technique :

  * Prévenir le **chef de projet** (`@mrGonzalezGomez`),
  * Si aucun consensus n’est trouvé, un **vote interne** peut être organisé sur le salon Discord `#sondage`.
