#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "backup_server.h"
#include "server.h"

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
    backup->processingCount = 0;
    pthread_mutex_init(&backup->mutex, NULL);
}

void destroyBackupServer(BackupServer* backup) {
    pthread_mutex_destroy(&backup->mutex);
}

void backupVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);
    pthread_mutex_lock(&backup->mutex);

    // Sauvegarde des vidéos normales
    backup->count = mainServer->count;
    for (int i = 0; i < mainServer->count; i++) {
        backup->buffer[i] = mainServer->buffer[i];
        
        unsigned long hashOriginal = hashVideo(&mainServer->buffer[i]);
        unsigned long hashCopy = hashVideo(&backup->buffer[i]);
        
        if (hashOriginal != hashCopy) {
            fprintf(stderr, "[BACKUP] Erreur de copie sur la vidéo %d\n", mainServer->buffer[i].id);
        }
    }

    // Nouveau: sauvegarde des vidéos en cours de traitement
    backup->processingCount = 0;
    for (int i = 0; i < MAX_VIDEOS; i++) {
        if (mainServer->beingProcessed[i]) {
            backup->processingBuffer[backup->processingCount] = mainServer->buffer[i];
            backup->processingCount++;
        }
    }

    pthread_mutex_unlock(&backup->mutex);
    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Sauvegarde terminée. %d vidéos normales, %d vidéos en cours de traitement.\n", 
           backup->count, backup->processingCount);
}

void restoreVideos(BackupServer* backup, ServerMonitor* mainServer) {
    pthread_mutex_lock(&mainServer->mutex);
    pthread_mutex_lock(&backup->mutex);

    // Restauration des vidéos normales
    mainServer->count = backup->count;
    mainServer->in = backup->count % MAX_VIDEOS;
    mainServer->out = 0;

    for (int i = 0; i < backup->count; i++) {
        mainServer->buffer[i] = backup->buffer[i];
        mainServer->beingProcessed[i] = 0; // Réinitialiser l'état de traitement
    }

    // Nouveau: restauration des vidéos qui étaient en cours de traitement
    // Elles sont remises au début du buffer pour être retraitées
    int processingStart = mainServer->in;
    for (int i = 0; i < backup->processingCount; i++) {
        if (mainServer->count < MAX_VIDEOS) {
            mainServer->buffer[mainServer->in] = backup->processingBuffer[i];
            mainServer->in = (mainServer->in + 1) % MAX_VIDEOS;
            mainServer->count++;
        }
    }

    pthread_mutex_unlock(&backup->mutex);
    pthread_mutex_unlock(&mainServer->mutex);

    printf("[BACKUP] Restauration terminée. %d vidéos normales + %d vidéos à retraiter.\n", 
           backup->count, backup->processingCount);
}
