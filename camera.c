#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "camera.h"
#include "server.h"

Video createVideo() {
    static int nextId = 1;
    Video video;
    video.id = nextId++;
    snprintf(video.content, CONTENT_SIZE, "Video simulée #%d", video.id);

    video.duration = rand() % 5 + 1;

    return video;
}

void* cameraThread(void* arg) {
    ServerMonitor* server = (ServerMonitor*)arg;

    while (1) {
        
        Video video = createVideo();

        sleep(video.duration);
        
        sendVideo(server, video);

        printf("[CAMERA] Vidéo %d envoyée au serveur\n", video.id);
        
    }

    return NULL;
}