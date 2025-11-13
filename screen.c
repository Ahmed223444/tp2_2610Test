#include <stdio.h>
#include <unistd.h>
#include "screen.h"
#include "server.h"

void broadcastVideo(Video video) {
    printf("[SCREEN] Diffusion de la vidéo %d : %s (durée %d s)\n",
           video.id, video.content, video.duration);

    // Simulation de la diffusion
    sleep(video.duration);
}

void* screenThread(void* arg) {
    ServerMonitor* server = (ServerMonitor*)arg;

    while (1) {
        Video video = getVideo(server);
        
        // Vérifier si on a reçu une vidéo valide (le serveur pourrait être inactif)
        if (video.id == 0) {
            usleep(100000); // Attendre 100ms avant de réessayer
            continue;
        }
        
        int index = server->out;

        broadcastVideo(video);

        completeVideo(server, index);
    }

    return NULL;
}
