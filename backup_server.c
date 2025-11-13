#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "backup_server.h"
#include "server.h"

#define JOURNAL_FILE "backup_journal.txt"

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
        persist_video_to_journal(&mainServer->buffer[i]);
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

// ---- Partie 2 : journal ----
int persist_video_to_journal(const Video *v) {
    FILE *f = fopen(JOURNAL_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%d;%s;%d\n", v->id, v->content, v->duration);
    fclose(f);
    return 0;
}

int mark_video_processed_in_journal(int video_id) {
    FILE *f = fopen(JOURNAL_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%d;PROCESSED\n", video_id);
    fclose(f);
    return 0;
}

int load_videos_from_journal(ServerMonitor* mainServer) {
    FILE *f = fopen(JOURNAL_FILE, "r");
    if (!f) return -1;

    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        Video v;
        if (strstr(line, "PROCESSED")) continue; // ignorer tombstones
        sscanf(line, "%d;%255[^;];%d", &v.id, v.content, &v.duration);
        mainServer->buffer[count++] = v;
    }
    mainServer->count = count;
    mainServer->in = count % MAX_VIDEOS;
    mainServer->out = 0;
    fclose(f);
    return count;
}

// Optionnel : compacter le journal en supprimant les vidéos marquées processed
int compact_journal(void) {
    FILE *f = fopen(JOURNAL_FILE, "r");
    if (!f) return -1;
    FILE *temp = fopen("backup_journal.tmp", "w");
    if (!temp) { fclose(f); return -1; }

    char line[512];
    while(fgets(line, sizeof(line), f)) {
        if (!strstr(line, "PROCESSED")) {
            fputs(line, temp);
        }
    }
    fclose(f);
    fclose(temp);
    remove(JOURNAL_FILE);
    rename("backup_journal.tmp", JOURNAL_FILE);
    return 0;
}
