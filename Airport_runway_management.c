#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PLANES 10
#define BACKUP_FILE "runway_queue.dat"   // Computer backup (Save/Load state)
#define REPORT_FILE "runway_report.txt"  // Human logbook (Formatted by Date)

typedef struct {
    int id;
    int fuelLevel;
    int arrivalTime;
    int emergency; // 1 for emergency, 0 otherwise
    int canceled;  // 1 if the plane's landing is canceled
} Plane;

typedef struct {
    Plane* planes;
    int count;
    int capacity;
} PriorityQueue;

// Function prototypes
void initQueue(PriorityQueue* queue);
void enqueue(PriorityQueue* queue, Plane plane);
Plane dequeue(PriorityQueue* queue);
Plane peek(const PriorityQueue* queue);
int isRunwayFree(const PriorityQueue* queue);
void heapifyUp(PriorityQueue* queue, int index);
void heapifyDown(PriorityQueue* queue, int index);
void swap(Plane* a, Plane* b);
void displayQueue(const PriorityQueue* queue);
void resizeQueue(PriorityQueue* queue);
void saveQueueToFile(const PriorityQueue* queue);
void loadQueueFromFile(PriorityQueue* queue);
void appendToDailyLog(const PriorityQueue* queue);
void logLandingToFile(Plane plane);
void cancelLanding(PriorityQueue* queue, int planeId);

// Initialize the priority queue
void initQueue(PriorityQueue* queue) {
    queue->count = 0;
    queue->capacity = MAX_PLANES;
    queue->planes = (Plane*)malloc(sizeof(Plane) * queue->capacity);
}

// Resize the queue when it's full
void resizeQueue(PriorityQueue* queue) {
    queue->capacity *= 2;
    queue->planes = (Plane*)realloc(queue->planes, sizeof(Plane) * queue->capacity);
}

// Insert a plane into the queue and re-heapify
void enqueue(PriorityQueue* queue, Plane plane) {
    if (queue->count >= queue->capacity) {
        printf("\nQueue is full. Resizing the queue.\n");
        resizeQueue(queue);
    }
    queue->planes[queue->count] = plane;
    heapifyUp(queue, queue->count);
    queue->count++;
}

// Remove the highest-priority plane from the queue and re-heapify
Plane dequeue(PriorityQueue* queue) {
    if (queue->count <= 0) {
        printf("\nRunway is empty.\n");
        Plane emptyPlane = {0, 0, 0, 0, 0};
        return emptyPlane;
    }
    Plane top = queue->planes[0];
    queue->planes[0] = queue->planes[--queue->count];
    heapifyDown(queue, 0);
    return top;
}

// Peek at the highest-priority plane without removing it
Plane peek(const PriorityQueue* queue) {
    if (queue->count <= 0) {
        Plane emptyPlane = {0, 0, 0, 0, 0};
        return emptyPlane;
    }
    return queue->planes[0];
}

// Check if the runway is free (queue is empty)
int isRunwayFree(const PriorityQueue* queue) {
    return queue->count == 0;
}

// Maintain the max-heap property by moving a plane up
void heapifyUp(PriorityQueue* queue, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if ((queue->planes[index].emergency > queue->planes[parent].emergency) ||
            (queue->planes[index].emergency == queue->planes[parent].emergency &&
             queue->planes[index].fuelLevel < queue->planes[parent].fuelLevel) ||
            (queue->planes[index].emergency == queue->planes[parent].emergency &&
             queue->planes[index].fuelLevel == queue->planes[parent].fuelLevel &&
             queue->planes[index].arrivalTime < queue->planes[parent].arrivalTime)) {

            swap(&queue->planes[index], &queue->planes[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

// Maintain the max-heap property by moving a plane down
void heapifyDown(PriorityQueue* queue, int index) {
    while (index * 2 + 1 < queue->count) {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int largest = index;

        if ((queue->planes[left].emergency > queue->planes[largest].emergency) ||
            (queue->planes[left].emergency == queue->planes[largest].emergency &&
             queue->planes[left].fuelLevel < queue->planes[largest].fuelLevel) ||
            (queue->planes[left].emergency == queue->planes[largest].emergency &&
             queue->planes[left].fuelLevel == queue->planes[largest].fuelLevel &&
             queue->planes[left].arrivalTime < queue->planes[largest].arrivalTime)) {
            largest = left;
        }

        if (right < queue->count &&
            ((queue->planes[right].emergency > queue->planes[largest].emergency) ||
             (queue->planes[right].emergency == queue->planes[largest].emergency &&
              queue->planes[right].fuelLevel < queue->planes[largest].fuelLevel) ||
             (queue->planes[right].emergency == queue->planes[largest].emergency &&
              queue->planes[right].fuelLevel == queue->planes[largest].fuelLevel &&
              queue->planes[right].arrivalTime < queue->planes[largest].arrivalTime))) {
            largest = right;
        }

        if (largest != index) {
            swap(&queue->planes[index], &queue->planes[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

// Swap two planes
void swap(Plane* a, Plane* b) {
    Plane temp = *a;
    *a = *b;
    *b = temp;
}

// Display the queue cleanly with human-readable text descriptors
void displayQueue(const PriorityQueue* queue) {
    if (queue->count == 0) {
        printf("\nNo planes in the queue.\n");
        return;
    }
    printf("\n--- Active Runway Queue (Heap Structure Status) ---\n");
    printf("ID\tFuel\tArrival\tEmergency Status\tFlight Status\n");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < queue->count; i++) {
        printf("%d\t%d\t%d\t%s\t\t%s\n",
               queue->planes[i].id,
               queue->planes[i].fuelLevel,
               queue->planes[i].arrivalTime,
               queue->planes[i].emergency ? "CRITICAL (YES)" : "Normal (No)",
               queue->planes[i].canceled ? "CANCELED" : "Active Waiting");
    }
}

// Save the system data state cleanly (For the computer to load)
void saveQueueToFile(const PriorityQueue* queue) {
    FILE* file = fopen(BACKUP_FILE, "wb");
    if (file == NULL) {
        printf("Error saving system backup state.\n");
        return;
    }
    fwrite(&queue->count, sizeof(int), 1, file);
    fwrite(queue->planes, sizeof(Plane), queue->count, file);
    fclose(file);

    // Simultaneously print the structured daily diary log for humans
    appendToDailyLog(queue);
}

// Load the system state back up instantly (For the computer)
void loadQueueFromFile(PriorityQueue* queue) {
    FILE* file = fopen(BACKUP_FILE, "rb");
    if (file == NULL) {
        printf("No previous backup state file found to load.\n");
        return;
    }
    fread(&queue->count, sizeof(int), 1, file);
    queue->capacity = (queue->count > 0) ? queue->count : MAX_PLANES;
    queue->planes = (Plane*)realloc(queue->planes, sizeof(Plane) * queue->capacity);

    if (queue->count > 0) {
        fread(queue->planes, sizeof(Plane), queue->count, file);
    }
    fclose(file);
    printf("System backup state restored successfully.\n");
}

// Generates the continuous, structured daily logbook for humans to open in Notepad
void appendToDailyLog(const PriorityQueue* queue) {
    FILE* file = fopen(REPORT_FILE, "a"); // Append mode
    if (file == NULL) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(file, "\n=========================================================\n");
    fprintf(file, "AIRPORT RUNWAY DATA LOG - DATE: %02d-%02d-%04d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    fprintf(file, "=========================================================\n");
    fprintf(file, "Planes Currently Waiting in Queue:\n");
    fprintf(file, "Total in Queue: %d\n", queue->count);
    fprintf(file, "---------------------------------------------------------\n");
    fprintf(file, "ID\tFuel\tArrival\tEmergency\tCanceled\n");
    fprintf(file, "---------------------------------------------------------\n");

    if (queue->count == 0) {
        fprintf(file, "(No planes waiting in queue)\n");
    } else {
        for (int i = 0; i < queue->count; i++) {
            fprintf(file, "%d\t%d\t%d\t%d\t\t%d\n",
                    queue->planes[i].id, queue->planes[i].fuelLevel,
                    queue->planes[i].arrivalTime, queue->planes[i].emergency,
                    queue->planes[i].canceled);
        }
    }
    fprintf(file, "---------------------------------------------------------\n\n");
    fclose(file);
    printf("Queue snapshot successfully saved/appended to human logbook (%s).\n", REPORT_FILE);
}

// Logs live landings directly underneath the daily diary records
void logLandingToFile(Plane plane) {
    FILE* file = fopen(REPORT_FILE, "a");
    if (file == NULL) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(file, "[LANDED] Date: %02d-%02d-%04d Time: %02d:%02d:%02d -> Plane ID: %d | Fuel: %d | Emergency: %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            plane.id, plane.fuelLevel,
            plane.emergency ? "YES" : "NO");
    fclose(file);
}

// Cancel landing clearance for a plane
void cancelLanding(PriorityQueue* queue, int planeId) {
    for (int i = 0; i < queue->count; i++) {
        if (queue->planes[i].id == planeId) {
            queue->planes[i].canceled = 1; // Mark plane as canceled
            printf("Plane %d landing canceled.\n", planeId);
            return;
        }
    }
    printf("Plane %d not found in the queue.\n", planeId);
}

// Main function
int main() {
    PriorityQueue runwayQueue;
    initQueue(&runwayQueue);

    int choice, planeId = 1, time = 0;
    while (1) {
        printf("\n\n--- Airport Runway Management ---\n");
        printf("1. Enqueue Plane\n");
        printf("2. Dequeue Plane for Landing\n");
        printf("3. Peek at Next Plane\n");
        printf("4. Check if Runway is Free\n");
        printf("5. Display Queue\n");
        printf("6. Save Queue/Generate Daily Log\n");
        printf("7. Load/Restore Queue State\n");
        printf("8. Cancel Landing Clearance\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                Plane newPlane;
                newPlane.id = planeId++;
                newPlane.canceled = 0;

                // Robust Input Validation for Fuel Level Bound Checking
                do {
                    printf("Enter fuel level (1-100): ");
                    scanf("%d", &newPlane.fuelLevel);
                    if (newPlane.fuelLevel < 1 || newPlane.fuelLevel > 100) {
                        printf("Invalid Input! Fuel capacity constraints range from 1 to 100.\n");
                    }
                } while (newPlane.fuelLevel < 1 || newPlane.fuelLevel > 100);

                newPlane.arrivalTime = ++time;

                // Automatically flag emergency condition if fuel reserves are critical
                if (newPlane.fuelLevel < 10) {
                    newPlane.emergency = 1;
                    printf("Fuel level is below 10. Automatically considered as an emergency.\n");
                } else {
                    // Robust Input Validation for Emergency Flag
                    do {
                        printf("Is it an emergency landing? (1 for Yes, 0 for No): ");
                        scanf("%d", &newPlane.emergency);
                        if (newPlane.emergency != 0 && newPlane.emergency != 1) {
                            printf("Invalid choice! Enter 1 for Emergency or 0 for Normal execution.\n");
                        }
                    } while (newPlane.emergency != 0 && newPlane.emergency != 1);
                }

                enqueue(&runwayQueue, newPlane);
                printf("Plane %d enqueued successfully.\n", newPlane.id);
                break;
            }
            case 2: {
                int planeLanded = 0;
                // Dequeue loops continuously past canceled tracks until a valid track is landed
                while (!isRunwayFree(&runwayQueue)) {
                    Plane landingPlane = dequeue(&runwayQueue);
                    if (landingPlane.canceled == 1) {
                        printf("Plane %d landing was canceled. Skipping track line...\n", landingPlane.id);
                        continue;
                    }
                    printf("Plane %d is landing successfully.\n", landingPlane.id);
                    logLandingToFile(landingPlane);
                    planeLanded = 1;
                    break;
                }
                if (!planeLanded) {
                    printf("Runway is free. No planes waiting to land.\n");
                }
                break;
            }
            case 3: {
                if (isRunwayFree(&runwayQueue)) {
                    printf("\nRunway is empty. No planes waiting to land.\n");
                } else {
                    int found = 0;
                    // Safely search down the array sequence for the true un-canceled top operational target
                    for (int i = 0; i < runwayQueue.count; i++) {
                        if (runwayQueue.planes[i].canceled == 0) {
                            Plane nextPlane = runwayQueue.planes[i];
                            printf("\nNext plane scheduled to land:\n");
                            printf("-> ID: %d | Fuel: %d | Emergency Status: %s\n",
                                   nextPlane.id, nextPlane.fuelLevel,
                                   nextPlane.emergency ? "CRITICAL" : "NORMAL");
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("\nAll waiting flights within the queue structure have been canceled.\n");
                    }
                }
                break;
            }
            case 4: {
                if (isRunwayFree(&runwayQueue)) {
                    printf("Runway is free.\n");
                } else {
                    printf("Runway is occupied.\n");
                }
                break;
            }
            case 5:
                displayQueue(&runwayQueue);
                break;
            case 6:
                saveQueueToFile(&runwayQueue);
                break;
            case 7:
                loadQueueFromFile(&runwayQueue);
                break;
            case 8: {
                int planeIdToCancel;
                printf("Enter the ID of the plane to cancel: ");
                scanf("%d", &planeIdToCancel);
                cancelLanding(&runwayQueue, planeIdToCancel);
                break;
            }
            case 9:
                printf("Exiting Airport Runway Management.\n");
                free(runwayQueue.planes); // Free dynamic allocation footprint
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
