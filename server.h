#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_VIDEOS 10        // nombre maximum de vidéos dans le buffer
#define CONTENT_SIZE 256     // taille du contenu d'une vidéo

typedef struct {
    int id;
    int duration;
    char content[CONTENT_SIZE];
} Video;

typedef struct {
    Video buffer[MAX_VIDEOS];
    int in;                  // index d'insertion
    int out;                 // index de retrait
    int count;               // nombre d'éléments présents
    bool active;             // indique si le serveur est actif ou en panne
    int beingProcessed[MAX_VIDEOS]; // 0 = libre, 1 = en cours de diffusion

    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
} ServerMonitor;

void initServer(ServerMonitor* server);
void destroyServer(ServerMonitor* server);

void sendVideo(ServerMonitor* server, Video video);
Video getVideo(ServerMonitor* server);

void completeVideo(ServerMonitor* server, int index);

#endif
