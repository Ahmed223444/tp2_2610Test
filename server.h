#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

#define MAX_VIDEOS 100
#define CONTENT_SIZE 256

typedef struct {
    int id;
    char content[CONTENT_SIZE];
    int duration;
} Video;

typedef struct {
    Video videos[MAX_VIDEOS];
    int in;
    int out;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
} ServerMonitor;

// Initialisation
void initServer(ServerMonitor* server);
void destroyServer(ServerMonitor* server);

// Gestion du flux
void sendVideo(ServerMonitor* server, Video video);
Video getVideo(ServerMonitor* server);
void completeVideo(ServerMonitor* server, int index);

#endif
