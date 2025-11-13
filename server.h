#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

typedef struct {
    int id;
    char titre[128];
    int duree;
} Video;

typedef struct {
    Video videos[100];
    int nb_videos;
    int panne;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ServerMonitor;

void init_server(ServerMonitor* serveur);
void* server_thread(void* arg);
void ajouter_video(ServerMonitor* serveur, const Video* v);
void simuler_panne(ServerMonitor* serveur);
void restaurer_serveur_principal(ServerMonitor* serveur);

#endif
