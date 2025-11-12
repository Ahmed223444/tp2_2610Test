#ifndef BACKUP_SERVER_H
#define BACKUP_SERVER_H

#include <stdbool.h>
#include "server.h"

#define BACKUP_MAX_VIDEOS 50

typedef struct {
    Video buffer[BACKUP_MAX_VIDEOS];
    int count;
    pthread_mutex_t mutex;
} BackupServer;

void initBackupServer(BackupServer* backup);
void destroyBackupServer(BackupServer* backup);

void backupVideos(BackupServer* backup, ServerMonitor* mainServer);

void restoreVideos(BackupServer* backup, ServerMonitor* mainServer);

#endif
