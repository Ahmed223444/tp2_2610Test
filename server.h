#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_VIDEOS 10
#define CONTENT_SIZE 256

typedef struct {
    int id;
    int duration;
    char content[CONTENT_SIZE];
} Video;

typedef struct {
    Video buffer[MAX_VIDEOS];
    int in;
    int out;
    int count;
    bool active;
    
    // Nouveau: suivi des vidéos en cours de traitement
    int beingProcessed[MAX_VIDEOS]; // 0 = libre, 1 = en cours de diffusion
    int processingStarted[MAX_VIDEOS]; // timestamp du début de traitement
    
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
} ServerMonitor;

void initServer(ServerMonitor* server);
void destroyServer(ServerMonitor* server);

void sendVideo(ServerMonitor* server, Video video);
Video getVideo(ServerMonitor* server);
void completeVideo(ServerMonitor* server, int index);

// Nouvelle fonction pour récupérer les vidéos en cours de traitement
int getProcessingVideos(ServerMonitor* server, Video* output);

#endif
