# Corentin
  - ajouté :
    - attribut signal dans struct my_thread
    - initialisation à 0 dans thread_create et constr()
    - ajout fonction handleSignal (utilisée dans scheduler)
    - ajout fonction t_signal permettant de modifier le champ signal du thread courant + déclarée dans thread.h
    - ajout signal.c + test dans Makefile (mais non ajouté aux tests globaux)
  - questions :
    - utiliser un enum global pour les signaux pour "remplacer" les int ?
    - reste à envoyer les signaux aux autres threads
    - quels signaux à gérer SIGKILL, SIGUSR1

# Réunion
  - Mettre une notice du makefile dans le Readme
  - Enlever le répertoire Install du git
  - Plusieurs mesures par nombre de threads (pour lisser les courbes)
