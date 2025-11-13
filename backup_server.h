#ifndef BACKUP_SERVER_H
#define BACKUP_SERVER_H

#include "server.h"

typedef struct {
    Video videos[MAX_VIDEOS];
    int count;
} BackupServer;

void initBackupServer(BackupServer* b);
void destroyBackupServer(BackupServer* b);

void backupVideos(BackupServer* backup, ServerMonitor* mainServer);
void restoreVideos(BackupServer* backup, ServerMonitor* mainServer);

#endif
