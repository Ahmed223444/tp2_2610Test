#include "backup_server.h"
#include <stdio.h>
#include <string.h>

void initBackupServer(BackupServer* b) {
    b->count = 0;
}

void destroyBackupServer(BackupServer* b) {
    b->count = 0;
}

void backupVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);

    backup->count = mainServer->count;
    for (int i = 0; i < mainServer->count; i++)
        backup->videos[i] = mainServer->videos[(mainServer->out + i) % MAX_VIDEOS];

    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Sauvegarde terminée, confirmation envoyée au serveur principal.\n");
}

void restoreVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);

    mainServer->in = 0;
    mainServer->out = 0;
    mainServer->count = backup->count;

    for (int i = 0; i < backup->count; i++)
        mainServer->videos[i] = backup->videos[i];

    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Restauration complète (%d vidéos restaurées).\n", backup->count);
}
