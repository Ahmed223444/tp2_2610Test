#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "backup_server.h"
#include "server.h"

// Fonction simple pour calculer le hash d'une vidéo (somme des codes ASCII)
static unsigned long hashVideo(const Video* video) {
    unsigned long hash = 0;
    for (int i = 0; i < CONTENT_SIZE && video->content[i] != '\0'; i++) {
        hash += video->content[i];
    }
    hash += video->id;
    hash += video->duration;
    return hash;
}

void initBackupServer(BackupServer* backup) {
    backup->count = 0;
    pthread_mutex_init(&backup->mutex, NULL);
}

void destroyBackupServer(BackupServer* backup) {
    pthread_mutex_destroy(&backup->mutex);
}

void backupVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);

    pthread_mutex_lock(&backup->mutex);

    backup->count = mainServer->count;

    for (int i = 0; i < mainServer->count; i++) {

        backup->buffer[i] = mainServer->buffer[i];

        // Calcul du hash pour vérification
        unsigned long hashOriginal = hashVideo(&mainServer->buffer[i]);
        unsigned long hashCopy     = hashVideo(&backup->buffer[i]);

        if (hashOriginal != hashCopy) {
            fprintf(stderr, "[BACKUP] Erreur de copie sur la vidéo %d\n", mainServer->buffer[i].id);
        }
    }

    pthread_mutex_unlock(&backup->mutex);
    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Sauvegarde terminée, confirmation envoyée au serveur principal.\n");
}

void restoreVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);
    pthread_mutex_lock(&backup->mutex);

    mainServer->count = backup->count;
    mainServer->in = backup->count % MAX_VIDEOS;
    mainServer->out = 0;

    for (int i = 0; i < backup->count; i++) {
        mainServer->buffer[i] = backup->buffer[i];
    }

    pthread_mutex_unlock(&backup->mutex);
    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Restauration terminée, le serveur principal peut reprendre.\n");
}