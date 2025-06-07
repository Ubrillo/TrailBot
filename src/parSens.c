//======================================================================
// of the 4tronix initio robot car. One can run this program within an
// ssh session.
//
//
// Compilation: 
// gcc -o parSens -Wall -Werror -lcurses -lwiringPi -lpthread -linitio parSens.c
//
//======================================================================

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <initio.h>
#include <curses.h>

//======================================================================
// parSens():
// Simple program to get starting with parallel programming of the 
// initio robot car.
// Use the ultrasonic sensors to drive forward but keep distance of 1m once an
// object sensed. At the same time use the left and right infrared (IR) sensor
// to stop if they sense an object.
// The given program is a sequential version of the program.
// The task is to refacture the program such that the ultrasonic measurement
// is done in a separate thread and new measurements are communicated to
// the main thread.
//======================================================================

#define SPEED_MAX 100

// desitance [in cm] to keep, given as two threshold values:
#define DIST_MIN 90
#define DIST_MAX 110

struct thread_dat {
  int distance;
  int bExit; // flag to indicate termination of thread
};

pthread_mutex_t count_mutex; // mutex to protect thread communication
void parSens(int argc, char *argv[], struct thread_dat *pdat) 
{
  int ch = 0;
  int distance;
  while (ch != 'q') {
    mvprintw(1, 1,"%s: Press 'q' to end program", argv[0]);


    pthread_mutex_lock(&count_mutex);
    distance = pdat->distance;
    pthread_mutex_unlock(&count_mutex);

    if (initio_IrLeft()==1 || initio_IrRight()==1) {
      initio_DriveForward (0); // Stop
    } else if (distance > DIST_MAX) {
      initio_DriveForward (SPEED_MAX); // Drive Forward
    } else if (distance < DIST_MIN) {
      initio_DriveReverse (SPEED_MAX); // Drive Backward
    } else {
      initio_DriveForward (0); // Stop - keep distance
    } // if
    mvprintw(3, 1,"Distance: %d cm  ", distance);

    ch = getch();
    if (ch != ERR)
      mvprintw(2, 1,"Key code: '%c' (%d)", ch, ch);
    refresh();
    //delay (100); // pause 100ms
  } // while

  return;
}


// Thread function to measure distance with ultrasonic sensor
void *worker(void *p_thread_dat)
{
  struct thread_dat *ptdat = (struct thread_dat *) p_thread_dat;
  while (!ptdat -> bExit){
     int dist = initio_UsGetDistance();
     pthread_mutex_lock(&count_mutex);
     ptdat->distance = dist;
     pthread_mutex_unlock(&count_mutex);
     delay(100); // wait 100ms before next measurement
 }
 return NULL;
}

//======================================================================
// main(): initialisation of libraries, etc
//======================================================================
int main (int argc, char *argv[])
{
  WINDOW *mainwin = initscr();  // curses: init screen
  noecho ();                    // curses: prevent the key being echoed
  cbreak();                     // curses: disable line buffering 
  nodelay(mainwin, TRUE);       // curses: set getch() as non-blocking 
  keypad (mainwin, TRUE);       // curses: enable detection of cursor and other keys

  initio_Init (); // initio: init the library

  pthread_t us_thread;          // pthread: thread handle
  pthread_attr_t pt_attr;       // pthread: thread attributes
  struct thread_dat tdat;       // data structure to communicate with thread
  tdat.distance = 0;
  tdat.bExit = 0;
  pthread_attr_init(&pt_attr);  // pthread: create and init thread attribute
   
  pthread_create(&us_thread, &pt_attr, worker, &tdat);
  
  parSens(argc, argv, &tdat);
  tdat.bExit = 1;               // signal thread to terminate
  
  pthread_join(us_thread, NULL); //wait for thread to finish
  pthread_attr_destroy(&pt_attr); //destroy thread attributes
  pthread_mutex_destroy(&count_mutex); //destory mutex
  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}

