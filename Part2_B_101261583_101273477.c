#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h> 

sem_t *semaphores[5];

void ta_process(int ta_number, int studentList[]){
    char ta_file[20]; //Create a name variable for the TA's output file
    sprintf(ta_file, "TA%d.txt", ta_number + 1); //Set the name variable for the file names to TA<TA number>.txt

    int counter = 0; //Create a counter variable to track the amount of time a TA has marked all students
    int student = -1; //Create a variable to keep track of the current student. Initilaized to -1 since it'll be incremented before first student is marked
    while (counter < 3){
        // Lock semaphore for current and next TA
        sem_wait(semaphores[ta_number]); //Lock the TA's own semaphore
        sem_wait(semaphores[(ta_number + 1) % 5]); //Lock the next TA's semaphore

        FILE *TA_File  = fopen(ta_file, "a"); //Create a file for the TAs and put it in append mode

        if (!TA_File) { //Check if the TA_File was opened successfully
            printf("Failed to open TA File");
            exit(EXIT_FAILURE); //If not opened successfully, exit
        }

        student++; //Move to next student

        printf("TA%d is marking student %d.\n", ta_number + 1, *(studentList + (student % 20)));
        fprintf(TA_File, "TA%d is marking student %d.\n", ta_number + 1, *(studentList + (student % 20)));
        sleep(rand() % 10 + 1); //Marking time

        if (*(studentList + (student % 20)) == 9999){ //If the last student is reached
            counter++; //Increment counter
        }

        int random = rand() % 11; //Create a random integer between 0 and 10, since %11 can only return integers 0-10
        printf("TA%d gave student %d the following grade: %d\n", ta_number + 1, *(studentList + (student % 20)), random);
        fprintf(TA_File, "TA%d gave student %d the following grade: %d\n", ta_number + 1, *(studentList + (student % 20)), random);

        //Release semaphores
        sem_post(semaphores[ta_number]);
        sem_post(semaphores[(ta_number + 1) % 5]);

        fclose(TA_File); //Close the TA's file
    }
    printf("TA%d finished marking!\n", ta_number + 1);
    exit(EXIT_SUCCESS); //exit this TA process
}

int main(){
    pid_t pid;

    srand(time(NULL)); //Seed the random number generator

    int key = ftok("Shared memory key", 65); //Create a unique key for the shared momory. 65 is used to give it the identifier 'A'.
    int sharedMemoryID = shmget(key, sizeof(int) * 20, IPC_CREAT | 0666); //Create an area of shared memory that we can read from and write into, and save the ID
    if (sharedMemoryID == -1){ //shmget returns -1 if it failed, so we make sure it didn't fail
        printf("shmget failed"); 
        exit(1); //exit if failed
    }
    int *studentList = (int *)shmat(sharedMemoryID, NULL, 0); //Attach the shared variable to the shared memory
    if (studentList == (int *)-1){ //shmatt returns -1 if it failed, so we make sure it didn't fail
        printf("shmat faild");
        exit(1); //exit if failed
    } //The shared list will only be read from and never written into, so no semaphores are needed to prevent race conditions

    //Create semaphores for each TA
    for (int i = 0; i < 5; i++) {
        char sem_name[10]; //Create a name variable for the semaphores
        snprintf(sem_name, 10, "/sem%d", i + 1); //Set the name variable to /sem<TA number> so that each TA has a unique semaphore name

        //Create the semaphore using O_CREAT. Using O_CREAT in the sem_open function tells the program to create the semaphore if it doesn't already exist
        semaphores[i] = sem_open(sem_name, O_CREAT, 0666, 1);
        if (semaphores[i] == SEM_FAILED) { //If sem_open failed
            printf("sem_open failed");
            exit(1);  //Exit if sem_open failed
        }
    }

    int classList[] = {1, 678, 1247, 1876, 2490, 2623, 3105, 3752, 4123, 4801, 5109, 5728, 6465, 6879, 7345, 7722, 8023, 8690, 9166, 9999};

    memcpy(studentList, classList, sizeof(int) * 20); //Copy the class list into the shared memory

    for (int i = 0; i < 5; i++){ //Create 5 processes for the TAs
        pid = fork(); //Fork to create new process

        if (pid < 0){ //If fork is less than 0, fork failed
            printf("Fork failed");
        } else if (pid == 0){
            ta_process(i, studentList); //run the TA's process
            exit(0);
        }
    }

    for (int i = 0; i < 5; i++) {
        wait(NULL); //Wait for any child process to finish
    }

    int checkDetach = shmdt(studentList); //Detach the student list from shared memory
    if (checkDetach == -1) { //shmdt returns -1 if it failed, so we make sure it didn't fail
        printf("shmdt failed");
        exit(1);
    }

    for (int i = 0; i < 5; i++) { //Unlink the semaphores
        char sem_name[10]; //Create a variable name for the semaphor to be unlinked
        snprintf(sem_name, 10, "/sem%d", i + 1); //Set the semaphore name to be match the names of the semaphores created earlier (/sem<TA number>)
        sem_unlink(sem_name); //Unlink the semaphore
    }

    return 0;
}