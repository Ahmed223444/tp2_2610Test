#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "server.h"
#include "backup_server.h"

int main() {
    ServerMonitor serveur;
    pthread_t tid_server, tid_backup;

    init_server(&serveur);

    pthread_create(&tid_server, NULL, server_thread, &serveur);
    pthread_create(&tid_backup, NULL, backup_server_thread, &serveur);

    printf("  p : simuler une panne\n");
    printf("  r : restaurer le serveur après panne\n");
    printf("  q : quitter proprement\n");
    printf("==============================\n");

    char cmd;
    while (1) {
        printf("> ");
        scanf(" %c", &cmd);
        if (cmd == 'p') simuler_panne(&serveur);
        else if (cmd == 'r') restaurer_serveur_principal(&serveur);
        else if (cmd == 'q') break;
        else printf("[SYSTEME] Commande inconnue.\n");
    }

    printf("[SYSTEME] Arrêt propre.\n");
    return 0;
}
