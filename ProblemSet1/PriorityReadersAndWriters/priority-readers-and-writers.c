/*	
 *
 *	CS 2600 / CS 8803: Introduction to Operating Systems
 *  Georgia Tech OMSCS via Udacity
 *  Problem Set 1: Priority Readers and Writers
 *  Author: Tomasz Foster
 * 
 */	

#include "priority-readers-and-writers.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
/*	
 *	Define Constants
 */	

#define NUM_READERS 5
#define NUM_WRITERS 5
#define NUM_READS 5
#define NUM_WRITES 5

/*	
 *	Define shared mutex as well as read and write conditions.
 */	

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t c_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER;

/*	
 *	Instantiate some shared resource to be read and written to
 *  as well as globally scoped resource counter and current number of readers
 */	

int sharedResource = 0;
int resourceCounter = 0;
int readersWaiting = 0;
int numReaders = 0;

/*	
 *	
 */	

int main( int argc, char *argv[]) {

    int i;

    int numReader[NUM_READERS];
    int numWriter[NUM_WRITERS];

    pthread_t readerThreadIds[NUM_READERS];
    pthread_t writerThreadIds[NUM_WRITERS];

    // seed random number generator: taken from solution to Problem Set 1
    // the following `usleep` functions are also derived from the solution set
    srandom((unsigned int)time(NULL));

    // create readers and writers
    for( i = 0; i < NUM_READERS; i++ ) {
        numReader[i] = i;
        pthread_create( &readerThreadIds[i], NULL, reader, &numReader[i] );   
    }

    for( i = 0; i < NUM_WRITERS; i++ ) {
        numWriter[i] = i;
        pthread_create( &writerThreadIds[i], NULL, writer, &numWriter[i] );   
    }

    // join readers and writers. 4 seperate for loops as we want to join them once they're all created
    for( i = 0; i < NUM_READERS; i++ ) {
        pthread_join( readerThreadIds[i], NULL );
    }

    for( i = 0; i < NUM_WRITERS; i++ ) {
        pthread_join( writerThreadIds[i], NULL );
    }

    return 0;
}

void *reader ( void *param ) {

    int i = 0, numberOfReaders = 0;
    int readerId = *((int *)param);

    for( i=0; i< NUM_READS; i++ ){

        // wait a random time interval so reads/writes don't happen at same time
        usleep( 1000 * ( random() % NUM_READERS + NUM_WRITERS ));

        pthread_mutex_lock( &m );                                   // lock
            readersWaiting++;
            while( resourceCounter == -1)
                pthread_cond_wait(&c_read, &m);
            readersWaiting--;
            numberOfReaders = ++resourceCounter;
        pthread_mutex_unlock( &m );                                 // unlock

        /* Critical Section */
        printf("Reader: %d  |  Value: %2d  Readers: %2d\n", readerId, sharedResource, numberOfReaders );
        /* End Critical Section */

        pthread_mutex_lock( &m );                                   // lock
            resourceCounter--;
            if ( resourceCounter == 0 ) {
                pthread_cond_signal( &c_write );
            }
        pthread_mutex_unlock( &m );                                 // unlock

    }

    pthread_exit(0);

}

void *writer ( void *param ) {

    int i = 0, numberOfReaders = 0;
    int writerId = *((int *)param);

    for( i=0; i< NUM_WRITES; i++ ){

        // wait a random time interval so reads/writes don't happen at same time
        usleep( 1000 * ( random() % NUM_READERS + NUM_WRITERS ));

        pthread_mutex_lock( &m );                                    // lock
            while( resourceCounter != 0){
                pthread_cond_wait(&c_write, &m);
            }
            resourceCounter = -1;
            numberOfReaders = resourceCounter + 1;
        pthread_mutex_unlock( &m );                                 // unlock

        /* Critical Section */

        printf("Writer: %d  |  Value: %2d  Readers: %2d\n", writerId, ++sharedResource, numberOfReaders );

        /* End Critical Section */

        pthread_mutex_lock( &m );                                   // lock
            resourceCounter = 0;
            // this is where we prioritize the readers
            if( readersWaiting > 0 ){
                pthread_cond_broadcast( &c_read );
            } else {
                pthread_cond_signal( &c_write );
            }
        pthread_mutex_unlock( &m );                                 // unlock

    }

    pthread_exit(0);
}