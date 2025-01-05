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

void ta_process(int ta_number){
    char ta_file[20]; //Create a name variable for the TA's output file
    sprintf(ta_file, "TA%d.txt", ta_number + 1); //Set the name variable for the file names to TA<TA number>.txt

    int student = -1; //Create a variable to keep track of the current student. Initilaized to -1 since it'll be incremented before first student is marked
    int currentStudentID; //Create a variable to store the current student's ID
    for (int i = 0; i < 3; i++){ //Loop through all students 3 times
        FILE *dataBase = fopen("database.txt", "r"); //Open the database file in read mode, if the file is already open, this will reset the file pointer to the start of the start of the file.
        while (1){
            // Lock semaphore for current and next TA
            sem_wait(semaphores[ta_number]); //Lock the TA's own semaphore
            sem_wait(semaphores[(ta_number + 1) % 5]); //Lock the next TA's semaphore

            FILE *TA_File  = fopen(ta_file, "a"); //Open the TAs ouput file in append mode

            if (!TA_File) { //Check if the TA_File was opened successfully
                printf("Failed to open TA File");
                exit(EXIT_FAILURE); //If not opened successfully, exit
            }

            student++; //Move to next student

            if (fscanf(dataBase, "%d", &currentStudentID) == EOF){ //Move the file pointer
                break; //Break if the file pointer has reached the end of the file
            }

            printf("TA%d is marking student %d.\n", ta_number + 1, currentStudentID);
            fprintf(TA_File, "TA%d is marking student %d.\n", ta_number + 1, currentStudentID);

            sleep(rand() % 10 + 1); //Marking time

            int random = rand() % 11; //Create a random integer between 0 and 10, since %11 can only return integers 0-10
            printf("TA%d gave student %d the following grade: %d\n", ta_number + 1, currentStudentID, random);
            fprintf(TA_File, "TA%d gave student %d the following grade: %d\n", ta_number + 1, currentStudentID, random);

            //Release semaphores
            sem_post(semaphores[ta_number]);
            sem_post(semaphores[(ta_number + 1) % 5]);

            fclose(TA_File); //Close the TA's file
        }
    }
    printf("TA%d finished marking!\n", ta_number + 1);
    exit(EXIT_SUCCESS); //exit this TA process
}

int main(){
    pid_t pid;

    srand(time(NULL)); //Seed the random number generator

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

    for (int i = 0; i < 5; i++){ //Create 5 processes for the TAs
        pid = fork(); //Fork to create new process

        if (pid < 0){ //If fork is less than 0, fork failed
            printf("Fork failed");
        } else if (pid == 0){
            ta_process(i); //run the TA's process
            exit(0);
        }
    }

    for (int i = 0; i < 5; i++) {
        wait(NULL); //Wait for any child process to finish
    }

    for (int i = 0; i < 5; i++) { //Unlink the semaphores
        char sem_name[10]; //Create a variable name for the semaphor to be unlinked
        snprintf(sem_name, 10, "/sem%d", i + 1); //Set the semaphore name to be match the names of the semaphores created earlier (/sem<TA number>)
        sem_unlink(sem_name); //Unlink the semaphore
    }

    return 0;
}