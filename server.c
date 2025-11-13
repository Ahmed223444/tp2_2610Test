#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void initServer(ServerMonitor* server) {
    server->in = 0;
    server->out = 0;
    server->count = 0;
    pthread_mutex_init(&server->mutex, NULL);
    pthread_cond_init(&server->notEmpty, NULL);
    pthread_cond_init(&server->notFull, NULL);
}

void destroyServer(ServerMonitor* server) {
    pthread_mutex_destroy(&server->mutex);
    pthread_cond_destroy(&server->notEmpty);
    pthread_cond_destroy(&server->notFull);
}

void sendVideo(ServerMonitor* server, Video video) {
    pthread_mutex_lock(&server->mutex);

    while (server->count == MAX_VIDEOS)
        pthread_cond_wait(&server->notFull, &server->mutex);

    server->videos[server->in] = video;
    server->in = (server->in + 1) % MAX_VIDEOS;
    server->count++;

    printf("[SERVER] Vidéo %d reçue (durée: %d sec). Total: %d vidéos.\n",
           video.id, video.duration, server->count);

    pthread_cond_signal(&server->notEmpty);
    pthread_mutex_unlock(&server->mutex);
}

Video getVideo(ServerMonitor* server) {
    pthread_mutex_lock(&server->mutex);

    while (server->count == 0)
        pthread_cond_wait(&server->notEmpty, &server->mutex);

    Video video = server->videos[server->out];
    pthread_mutex_unlock(&server->mutex);
    return video;
}

void completeVideo(ServerMonitor* server, int index) {
    pthread_mutex_lock(&server->mutex);

    server->out = (server->out + 1) % MAX_VIDEOS;
    server->count--;

    printf("[SERVER] Vidéo à l’index %d complétée et retirée du buffer. Vidéos restantes : %d.\n",
           index, server->count);

    pthread_cond_signal(&server->notFull);
    pthread_mutex_unlock(&server->mutex);
}
