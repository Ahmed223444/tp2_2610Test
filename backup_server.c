#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "backup_server.h"

#define JOURNAL_FILE "videos_journal.txt"

pthread_mutex_t backup_mutex = PTHREAD_MUTEX_INITIALIZER;

void* backup_server_thread(void* arg) {
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

int sauvegarder_serveur(ServerMonitor* serveur) {
    pthread_mutex_lock(&backup_mutex);
    FILE* f = fopen(JOURNAL_FILE, "a");
    if (!f) {
        perror("[BACKUP] Erreur ouverture journal");
        pthread_mutex_unlock(&backup_mutex);
        return -1;
    }

    for (int i = 0; i < serveur->nb_videos; i++) {
        Video* v = &serveur->videos[i];
        fprintf(f, "ADD %d %s %d\n", v->id, v->titre, v->duree);
    }

    fclose(f);
    pthread_mutex_unlock(&backup_mutex);
    printf("[BACKUP] Sauvegarde terminée.\n");
    return 0;
}

int restaurer_serveur(ServerMonitor* serveur) {
    pthread_mutex_lock(&backup_mutex);
    FILE* f = fopen(JOURNAL_FILE, "r");
    if (!f) {
        perror("[BACKUP] Aucun fichier de sauvegarde trouvé");
        pthread_mutex_unlock(&backup_mutex);
        return -1;
    }

    serveur->nb_videos = 0;
    char action[8], titre[128];
    int id, duree;

    while (fscanf(f, "%s %d %s %d", action, &id, titre, &duree) == 4) {
        if (strcmp(action, "ADD") == 0) {
            Video v = { .id = id, .duree = duree };
            strncpy(v.titre, titre, sizeof(v.titre));
            serveur->videos[serveur->nb_videos++] = v;
        }
    }

    fclose(f);
    pthread_mutex_unlock(&backup_mutex);
    printf("[BACKUP] Restauration terminée (%d vidéos restaurées)\n", serveur->nb_videos);
    return 0;
}

// Journalisation bas-niveau
int persist_video_to_journal(const Video *v) {
    pthread_mutex_lock(&backup_mutex);
    FILE* f = fopen(JOURNAL_FILE, "a");
    if (!f) {
        perror("persist_video_to_journal");
        pthread_mutex_unlock(&backup_mutex);
        return -1;
    }
    fprintf(f, "ADD %d %s %d\n", v->id, v->titre, v->duree);
    fclose(f);
    pthread_mutex_unlock(&backup_mutex);
    return 0;
}

int mark_video_processed_in_journal(int video_id) {
    pthread_mutex_lock(&backup_mutex);
    FILE* f = fopen(JOURNAL_FILE, "a");
    if (!f) {
        perror("mark_video_processed_in_journal");
        pthread_mutex_unlock(&backup_mutex);
        return -1;
    }
    fprintf(f, "DEL %d\n", video_id);
    fclose(f);
    pthread_mutex_unlock(&backup_mutex);
    return 0;
}

int load_videos_from_journal(ServerMonitor* serveur) {
    return restaurer_serveur(serveur);
}

int compact_journal(void) {
    // Optionnel : pas nécessaire pour la version simple
    return 0;
}
