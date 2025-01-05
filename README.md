# Concurrent Processes in Unix: TA Exam Marking System

This repository contains a solution to a concurrency problem using Unix/Linux shared memory, semaphores, and processes. The problem involves coordinating five Teaching Assistants (TAs) marking exams concurrently while accessing a shared database of students. The solution is implemented incrementally and adheres to the specified requirements for process synchronization and inter-process communication.

## Repository Name

`concurrent-processes-ta-marking`

## Problem Description

Five Teaching Assistants (TAs) are tasked with marking a pile of exams. They:
1. Sit around a circular table with a pile of exams.
2. Access a database of student numbers to determine which student to mark next.
3. Mark one exercise for the student and save the mark in an individual file.

The system ensures that:
- Only two TAs can mark exams at the same time.
- Each TA must lock their own semaphore and the next semaphore (cyclically).
- Once a TA finishes accessing the database, they release their semaphores and begin marking.
- The system terminates when all TAs have marked each student three times.

## Project Structure

```
concurrent-processes-ta-marking/
├── src/
│   ├── part2a.c         # Initial solution with semaphores and files
│   ├── part2b.c         # Solution using semaphores and shared memory
│   ├── part2c.c         # Improved protocol for semaphore handling
│   ├── part2e.c         # Report on deadlock/livelock scenarios
├── data/
│   ├── students.txt     # Input file with student numbers
│   ├── TA1.txt          # Output file for TA 1
│   ├── TA2.txt          # Output file for TA 2
│   ├── TA3.txt          # Output file for TA 3
│   ├── TA4.txt          # Output file for TA 4
│   ├── TA5.txt          # Output file for TA 5
├── README.md            # Project documentation
```

## Solutions

### Part 2.a: Basic Semaphore-Based Solution
- Implements the protocol for accessing the database using semaphores.
- Each TA creates an individual output file to save the marks.
- No shared memory is used in this part.

### Part 2.b: Shared Memory Solution
- Converts the database access to use shared memory.
- Each TA uses a shared array to coordinate marking.
- Implements process coordination using semaphores.

### Part 2.c: Improved Semaphore Protocol
- Adds logic to handle unavailable semaphores by releasing already acquired locks.
- Ensures fair access to the database for all TAs.

### Part 2.d: Deadlock/Livelock Report
- Discusses conditions under which deadlock or livelock may occur.
- Explains the execution order observed during runs.
- Results are saved in `part2d_report.txt`.

### Part 2.e: Deadlock-Free and Livelock-Free Solution
- Implements a protocol that guarantees freedom from deadlock and livelock.
- Details of the solution and execution results are documented in `part2e_report.txt`.

## How to Build and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/concurrent-processes-ta-marking.git
   cd concurrent-processes-ta-marking
   ```

2. Build the program:
   ```bash
   make
   ```

3. Run each part:
   ```bash
   ./part2a
   ./part2b
   ./part2c
   ./part2e
   ```

## Dependencies

- GCC compiler
- Unix/Linux operating system
- POSIX libraries for shared memory and semaphores

## Future Enhancements

- Extend the system to handle more TAs or a larger student database.
- Improve efficiency of semaphore acquisition and release.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

