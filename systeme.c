// Ajouter cette fonction pour gérer proprement l'arrêt
void safeThreadCancel(pthread_t thread) {
    pthread_cancel(thread);
    void* result;
    pthread_join(thread, &result);
}

void simulateCrash() {
    if (systemCrashed) {
        printf("[SYSTEME] Le système est déjà en panne.\n");
        return;
    }

    printf("\n[!! PANNE] Panne simulée ! Sauvegarde en cours...\n");

    // Désactiver le serveur principal pour éviter de nouvelles opérations
    pthread_mutex_lock(&mainServer.mutex);
    mainServer.active = false;
    pthread_mutex_unlock(&mainServer.mutex);

    // Réveiller tous les threads en attente
    pthread_cond_broadcast(&mainServer.notEmpty);
    pthread_cond_broadcast(&mainServer.notFull);

    // Sauvegarder les données
    backupVideos(&backupServer, &mainServer);

    printf("[!! PANNE] Sauvegarde terminée. %d vidéos sauvegardées.\n", backupServer.count);

    // Arrêter les threads proprement
    safeThreadCancel(cameraThreadId);
    safeThreadCancel(screenThreadId);

    systemCrashed = 1;

    printf("[!! PANNE] Tous les processus ont été interrompus.\n");
    printf("[SYSTEME] Tapez 'r' pour restaurer le serveur, ou 'q' pour quitter.\n");
}

void restoreSystem() {
    if (!systemCrashed) {
        printf("[SYSTEME] Aucun besoin de restauration, le système fonctionne normalement.\n");
        return;
    }

    printf("\n[RESTORE] Restauration du serveur principal à partir de la sauvegarde...\n");

    // Restaurer les vidéos
    restoreVideos(&backupServer, &mainServer);

    // Réactiver le serveur
    pthread_mutex_lock(&mainServer.mutex);
    mainServer.active = true;
    pthread_mutex_unlock(&mainServer.mutex);

    // Relancer les threads
    pthread_create(&cameraThreadId, NULL, cameraThread, &mainServer);
    pthread_create(&screenThreadId, NULL, screenThread, &mainServer);

    systemCrashed = 0;

    printf("[RESTORE] Restauration complète. Le système reprend son fonctionnement normal.\n");
}
