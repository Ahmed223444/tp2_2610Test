#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>    // pour raise et signal

#include "server.h"
#include "camera.h"
#include "screen.h"
#include "backup_server.h"

ServerMonitor mainServer;
BackupServer backupServer;

pthread_t cameraThreadId;
pthread_t screenThreadId;

int systemCrashed = 0;   // 0 = normal, 1 = en panne

void simulateCrash() {
    if (systemCrashed) {
        printf("[SYSTEME] Le système est déjà en panne.\n");
        return;
    }

    printf("\n[!! PANNE] Panne simulée ! Sauvegarde en cours...\n");

    backupVideos(&backupServer, &mainServer);

    printf("[!! PANNE] Sauvegarde terminée.\n");

    pthread_cancel(cameraThreadId);
    pthread_cancel(screenThreadId);

    systemCrashed = 1;

    printf("[!! PANNE] Tous les processus ont été interrompus.\n");
    printf("[SYSTEME] Tapez 'r' pour restaurer le serveur, ou 'q' pour quitter.\n");
}

void handleCrashSignal(int sig) {
    printf("\n[SIGNAL] Signal reçu (%d), simulation de panne...\n", sig);
    simulateCrash();
}

void restoreSystem() {
    if (!systemCrashed) {
        printf("[SYSTEME] Aucun besoin de restauration, le système fonctionne normalement.\n");
        return;
    }

    printf("\n[RESTORE] Restauration du serveur principal à partir de la sauvegarde...\n");

    restoreVideos(&backupServer, &mainServer);

    pthread_create(&cameraThreadId, NULL, cameraThread, &mainServer);
    pthread_create(&screenThreadId, NULL, screenThread, &mainServer);

    systemCrashed = 0;

    printf("[RESTORE] Restauration complète. Le système reprend son fonctionnement normal.\n");
}

void shutdownSystem() {
    printf("\n[SYSTEME] Arrêt du système...\n");

    pthread_cancel(cameraThreadId);
    pthread_cancel(screenThreadId);
    pthread_join(cameraThreadId, NULL);
    pthread_join(screenThreadId, NULL);

    destroyServer(&mainServer);
    destroyBackupServer(&backupServer);

    printf("[SYSTEME] Tous les threads terminés et ressources libérées.\n");
    printf("[SYSTEME] Fin du programme.\n");
}

int main() {
    srand(time(NULL));

    signal(SIGUSR1, handleCrashSignal);

    printf("[SYSTEME] Initialisation du serveur principal...\n");
    initServer(&mainServer);

    printf("[SYSTEME] Initialisation du serveur de sauvegarde...\n");
    initBackupServer(&backupServer);

    printf("[SYSTEME] Lancement des threads caméra et écran...\n");
    pthread_create(&cameraThreadId, NULL, cameraThread, &mainServer);
    pthread_create(&screenThreadId, NULL, screenThread, &mainServer);

    printf("[SYSTEME] Système opérationnel.\n");
    printf("=== COMMANDES DISPONIBLES ====\n");
    printf("  p : simuler une panne\n");
    printf("  r : restaurer le serveur après panne\n");
    printf("  q : quitter proprement\n");
    printf("==============================\n");

    char command;
    while (1) {
        printf("\n> ");
        command = getchar();

        if (command == '\n')
            continue;

        switch (command) {
            case 'p':
                raise(SIGUSR1);
                break;

            case 'r':
                restoreSystem();
                break;

            case 'q':
                shutdownSystem();
                return 0;

            default:
                printf("[SYSTEME] Commande inconnue.\n");
                break;
        }

        while (getchar() != '\n');
    }

    return 0;
}
