#ifndef BACKUP_SERVER_H
#define BACKUP_SERVER_H

#include <stdbool.h>
#include "server.h"

#define BACKUP_MAX_VIDEOS 50

typedef struct {
    Video buffer[BACKUP_MAX_VIDEOS];
    Video processingBuffer[BACKUP_MAX_VIDEOS]; // Nouveau: sauvegarde des vidéos en cours
    int count;
    int processingCount; // Nouveau: compte des vidéos en cours de traitement
    pthread_mutex_t mutex;
} BackupServer;

void initBackupServer(BackupServer* backup);
void destroyBackupServer(BackupServer* backup);

void backupVideos(BackupServer* backup, ServerMonitor* mainServer);
void restoreVideos(BackupServer* backup, ServerMonitor* mainServer);

// Nouvelle fonction pour sauvegarder les vidéos en cours de traitement
void backupProcessingVideos(BackupServer* backup, Video* processingVideos, int count);

#endif
