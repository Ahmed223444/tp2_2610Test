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

// ---- Partie 2 : journal de persistance ----
int persist_video_to_journal(const Video *v);    // append video
int mark_video_processed_in_journal(int video_id); // append tombstone
int load_videos_from_journal(ServerMonitor* mainServer); // rebuild in-memory from journal
int compact_journal(void); // optionnel

#endif
