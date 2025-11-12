#include "server.h"
#include <stdio.h>
#include <stdlib.h>

void initServer(ServerMonitor* server) {
    server->count = 0;
    server->in = 0;
    server->out = 0;

    for (int i = 0; i < MAX_VIDEOS; i++)
        server->beingProcessed[i] = 0;

    pthread_mutex_init(&server->mutex, NULL);    // Mutex pour assurer l'accès exclusif à la mémoire partagée
    pthread_cond_init(&server->notFull, NULL);   // Variable conditionnelle pour vérifier si le serveur est plein
    pthread_cond_init(&server->notEmpty, NULL);  // Variable conditionnelle pour vérifier si le serveur est vide
}

void destroyServer(ServerMonitor* server) {
    pthread_mutex_destroy(&server->mutex);
    pthread_cond_destroy(&server->notFull);
    pthread_cond_destroy(&server->notEmpty);
}

void sendVideo(ServerMonitor* server, Video video) {
    pthread_mutex_lock(&server->mutex);

    while (server->count == MAX_VIDEOS) {
        printf("[SERVER] Mémoire pleine, caméra en attente...\n");
        pthread_cond_wait(&server->notFull, &server->mutex);
    }

    server->buffer[server->in] = video;
    server->in = (server->in + 1) % MAX_VIDEOS;
    server->count++;

    printf("[SERVER] Vidéo %d reçue (durée: %d sec). Total: %d vidéos.\n",
           video.id, video.duration, server->count);

    pthread_cond_signal(&server->notEmpty);

    pthread_mutex_unlock(&server->mutex);
}

Video getVideo(ServerMonitor* server) {
    pthread_mutex_lock(&server->mutex);

    while (server->count == 0) {
        printf("[SERVER] Aucune vidéo disponible, écran en attente...\n");
        pthread_cond_wait(&server->notEmpty, &server->mutex);
    }

    int index = server->out;
    Video video = server->buffer[index];
    server->beingProcessed[index] = 1;

    printf("[SERVER] Vidéo %d envoyée à un écran (en cours de diffusion).\n",
           video.id);

    pthread_mutex_unlock(&server->mutex);
    return video;
}

void completeVideo(ServerMonitor* server, int index) {
    pthread_mutex_lock(&server->mutex);

    if (server->beingProcessed[index]) {
        server->beingProcessed[index] = 0;
        server->out = (server->out + 1) % MAX_VIDEOS;
        server->count--;

        printf("[SERVER] Vidéo à l’index %d complétée et retirée du buffer. "
               "Vidéos restantes : %d.\n", index, server->count);

        pthread_cond_signal(&server->notFull);
    }

    pthread_mutex_unlock(&server->mutex);
}