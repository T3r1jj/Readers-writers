/* 
 * Copyright 2014 Damian Terlecki.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int readersInQueue = 0;
int writersInQueue = 0;
int readersInLibrary = 0;
int writersInLibrary = 0;
sem_t readSem, writeSem, varSem;
unsigned long uSecSleep = 2000000;

perror_exit(char* errorStr) {
    perror(errorStr);
    exit(EXIT_FAILURE);
}

void *readerF(void* arg) {
    while (1) {
        sem_wait(&varSem);
            readersInQueue++;
            printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                    readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
        sem_post(&varSem);

        sem_wait(&readSem);
            sem_wait(&varSem);
                if (readersInLibrary == 0) {
                    sem_post(&varSem);
                    sem_wait(&writeSem);
                } else {
                    sem_post(&varSem);
                }
            sem_wait(&varSem);
                readersInLibrary++;
                readersInQueue--;
                printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                        readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
            sem_post(&varSem);
        sem_post(&readSem);

        /* Reading... */
        usleep(rand() % uSecSleep);

        sem_wait(&varSem);
            readersInLibrary--;
            printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                    readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
            if (readersInLibrary == 0) {
                sem_post(&writeSem);
            }
        sem_post(&varSem);

        /* Sleep and enter the queue again */
        usleep(rand() % uSecSleep);
    }
}

void *writerF(void* arg){
    while (1) {
        sem_wait(&varSem);
            writersInQueue++;
            printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                    readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
        sem_post(&varSem);
        sem_wait(&readSem);
            sem_wait(&writeSem);
            sem_post(&readSem);
            sem_wait(&varSem);
                writersInLibrary++;
                writersInQueue--;
                printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                        readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
            sem_post(&varSem);

            /* Writing... */
            usleep(rand() % uSecSleep);

            sem_wait(&varSem);
                writersInLibrary--;
                printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
                        readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
            sem_post(&varSem);
        sem_post(&writeSem);

        /* Sleep and enter the queue again */
        usleep(rand() % uSecSleep);
    }
}

int main(int argc, char** argv) {
    printf("\nReaders - writers 3rd problem (no starvation)\n");
    if (sem_init(&readSem, 0, 1)) perror_exit("While initializing readSem (sem_init)");
    if (sem_init(&writeSem, 0, 1)) perror_exit("While initializing writeSem (sem_init)");
    if (sem_init(&varSem, 0, 1)) perror_exit("While initializing varSem (sem_init)");
    int readersCount, writersCount;
    if ((argv[1] == NULL) || (argv[2]) == NULL ) {
        printf("Number of readers > ");
        if (scanf("%d", &readersCount) == EOF) perror_exit("scanf");
        printf("Number of writers > ");
        if (scanf("%d", &writersCount) == EOF) perror_exit("scanf");
        printf("Starting in 1 sec...\n\n");
        sleep(1);
    } else {
        readersCount = atoi(argv[1]);
        writersCount = atoi(argv[2]);
        printf("Number of Readers = %d\n", readersCount);
        printf("Number of Writers = %d\n", writersCount);
        printf("Starting in 2 sec...\n\n");
        sleep(2);
    }
    srand(time(NULL));
    pthread_t *readerThread = calloc(readersCount, sizeof(pthread_t));
    pthread_t *writerThread = calloc(writersCount, sizeof(pthread_t));
    long i = 0;
    printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n",
            readersInQueue, writersInQueue, readersInLibrary, writersInLibrary);
    for (i = 0; i < readersCount; ++i) {
        if (pthread_create(&readerThread[i], NULL, readerF, (void*)i)) {
            perror_exit("Error while creating reader thread (pthread_create)");
        }
    }
    for (i = 0; i < writersCount; ++i) {
        if (pthread_create(&writerThread[i], NULL, writerF, (void*)i)) {
            perror_exit("Error while creating writer thread (pthread_create)");
        }
    }
    for (i = 0; i < readersCount; ++i) {
        if (pthread_join(readerThread[i], NULL)) {  // wait for thread finish which will not happen
            perror_exit("Error while waiting for reader thread termination (pthread_join)");
        }
    }
    free(readerThread);
    for (i = 0; i < writersCount; ++i) {
        if (pthread_join(writerThread[i], NULL)) {
            perror_exit("Error while waiting for writer thread termination (pthread_join)");
        }
    }
    free(writerThread);
    sem_destroy(&varSem);
    sem_destroy(&readSem);
    sem_destroy(&writeSem);
}
