#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "server.h"
#include "backup_server.h"

void init_server(ServerMonitor* serveur) {
    serveur->nb_videos = 0;
    serveur->panne = 0;
    pthread_mutex_init(&serveur->mutex, NULL);
    pthread_cond_init(&serveur->cond, NULL);
}

void* server_thread(void* arg) {
    ServerMonitor* serveur = (ServerMonitor*)arg;
    while (1) {
        pthread_mutex_lock(&serveur->mutex);
        if (serveur->panne) {
            pthread_mutex_unlock(&serveur->mutex);
            sleep(1);
            continue;
        }

        pthread_mutex_unlock(&serveur->mutex);
        sleep(1);
    }
    return NULL;
}

void ajouter_video(ServerMonitor* serveur, const Video* v) {
    pthread_mutex_lock(&serveur->mutex);
    if (serveur->nb_videos < 100) {
        serveur->videos[serveur->nb_videos++] = *v;
        persist_video_to_journal(v);
        printf("[SERVER] Vidéo %d ajoutée et sauvegardée.\n", v->id);
    }
    pthread_mutex_unlock(&serveur->mutex);
}

void simuler_panne(ServerMonitor* serveur) {
    pthread_mutex_lock(&serveur->mutex);
    serveur->panne = 1;
    pthread_mutex_unlock(&serveur->mutex);
    printf("[!! PANNE] Panne simulée ! Sauvegarde en cours...\n");
    sauvegarder_serveur(serveur);
    printf("[!! PANNE] Sauvegarde terminée.\n");
}

void restaurer_serveur_principal(ServerMonitor* serveur) {
    pthread_mutex_lock(&serveur->mutex);
    serveur->panne = 0;
    pthread_mutex_unlock(&serveur->mutex);
    restaurer_serveur(serveur);
    printf("[SYSTEME] Restauration complète.\n");
}
