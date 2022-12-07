#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "methods.c"

#define MAX_PROCESS 10000

struct process_info
{
    int burst_t,
        arrive_t,
        end_t;
    int wait_t;
    int turnaround_t;
    int remaining_t;
    int proc_num;
};

int readMethod;
int num_process;
int tot_wait_t = 0;
int tot_turnaround_t = 0;

int timeQ;
int timeQ1;
int timeQ2;

int currentTime = 0;
int idleTime = 0;

FILE *fp;

struct process_info procArray[MAX_PROCESS];

struct process_info readyQ[MAX_PROCESS];
int pInRQ = 0;

struct process_info roundRobinQ[MAX_PROCESS];
int pInRRQ = 0;

struct process_info FCFSQ[MAX_PROCESS];
int pInFCFSQ = 0;

struct process_info RRQ2[MAX_PROCESS];
int pInRR2 = 0;

struct process_info finalQ[MAX_PROCESS];
int nOfFinal = 0;

////---------------Copy the array---------------////
void copyArray(struct process_info procArray[])
{
    for (int i = 0; i < num_process; i++)
    {
        roundRobinQ[i] = procArray[i];
    }

} // end of copyArray()

//----Sort the Processes According to arrival time----//
void sortProcess(struct process_info procArray[])
{
    int arr_temp;
    int b_temp;
    int pnum_temp;
    int rt_temp;
    for (int i = 0; i < num_process; i++)
    {
        for (int j = i + 1; j < num_process; j++)
        {
            if (procArray[i].arrive_t > procArray[j].arrive_t)
            {

                arr_temp = procArray[i].arrive_t;
                procArray[i].arrive_t = procArray[j].arrive_t;
                procArray[j].arrive_t = arr_temp;

                b_temp = procArray[i].burst_t;
                procArray[i].burst_t = procArray[j].burst_t;
                procArray[j].burst_t = b_temp;

                pnum_temp = procArray[i].proc_num;
                procArray[i].proc_num = procArray[j].proc_num;
                procArray[j].proc_num = pnum_temp;

                rt_temp = procArray[i].remaining_t;
                procArray[i].remaining_t = procArray[j].remaining_t;
                procArray[j].remaining_t = rt_temp;
            }
        } // End of [for(int j=i+1;j<num_process; j++)]
    }     // End of [for(int i=0;i<num_process; i++)]
} // end of sortProcess()

//-------------Calculate end time-------------//
void calcEndTime(int num_process, struct process_info procArray[])
{

    for (int i = 0; i < num_process; i++)
    {
        if (i == 0)
        {
            // In case the first process doesn't start at 0 [There's idle time]
            if (procArray[i].arrive_t > 0)
            {
                idleTime = procArray[i].arrive_t;
                procArray[i].end_t = procArray[i].burst_t + idleTime;
            }
            else
            {
                procArray[i].end_t = procArray[i].burst_t;
            }
        }
        else
        {
            // In case there's idle time between the processes
            if (procArray[i].arrive_t > procArray[i - 1].end_t)
            {
                idleTime = procArray[i].arrive_t - procArray[i - 1].end_t;
                procArray[i].end_t = procArray[i - 1].end_t + procArray[i].burst_t + idleTime;
            }
            else
            {
                procArray[i].end_t = procArray[i - 1].end_t + procArray[i].burst_t;
            }
        }

    } // End of [for (int i=0; i<num_process; i++)]
} // End of [calcEndTime]

//-----------------Calculate WT & TAT-----------------//
void waitingT_turnaroundT(int num_process, struct process_info procArray[])
{
    for (int i = 0; i < num_process; i++)
    {
        procArray[i].turnaround_t = procArray[i].end_t - procArray[i].arrive_t;
        procArray[i].wait_t = procArray[i].turnaround_t - procArray[i].burst_t;
    }
}

//---------------Calculate Avg WT & TAT---------------//
void AvgTime(int num_process, struct process_info procArray[])
{
    for (int i = 0; i < num_process; i++)
    {
        tot_wait_t += procArray[i].wait_t;
        tot_turnaround_t += procArray[i].turnaround_t;
    }
}

//----------------Display the results----------------//
void results(int num_process, struct process_info procArray[])
{

    printf(" \n");
    printf("Process    |    Arrival    |    Burst    |    Exit     |   Turnaround    |    Wait    |\n");

    for (int i = 0; i < num_process; i++)
    {
        printf("P%d         |%6d         |%6d       |%6d       |%6d           |%6d      |\n", procArray[i].proc_num, procArray[i].arrive_t, procArray[i].burst_t, procArray[i].end_t, procArray[i].turnaround_t, procArray[i].wait_t);
    }

    printf("\n\nAverage waiting time: %0.1f ", (float)tot_wait_t / num_process);
    printf("\nAverage turnaround time: %0.1f ", (float)tot_turnaround_t / num_process);
}

//----------------Write the results in file----------------//
void writeInFile(int num_process, struct process_info procArray[])
{
    fprintf(fp, "Process    |    Arrival    |    Burst    |    Exit     |   Turnaround    |    Wait    |\n");
    for (int i = 0; i < num_process; i++)
    {
        fprintf(fp, "P%d         |%6d         |%6d       |%6d       |%6d           |%6d      |\n", procArray[i].proc_num, procArray[i].arrive_t, procArray[i].burst_t, procArray[i].end_t, procArray[i].turnaround_t, procArray[i].wait_t);
    }

    fprintf(fp, "\n\nAverage waiting time: %0.1f ", (float)tot_wait_t / num_process);
    fprintf(fp, "\nAverage turnaround time: %0.1f ", (float)tot_turnaround_t / num_process);
}

//---------------get info of processes---------------//
void readFromConsole()
{
    printf("\nPlease enter total number of processes: ");
    scanf("%d", &num_process);

    printf("\nPlease enter the information of each process: ");
    for (int i = 0; i < num_process; i++)
    {
        printf("\n P%d Burst time: ", i);
        scanf("%d", &procArray[i].burst_t);
        procArray[i].remaining_t = procArray[i].burst_t;

        printf(" P%d Arrival time: ", i);
        scanf("%d", &procArray[i].arrive_t);

        procArray[i].proc_num = i;
    }

    printf("\nPlease enter quantum time for RR: ");
    scanf("%d", &timeQ);

    printf("Please enter quantum time for RR1 of MLQ: ");
    scanf("%d", &timeQ1);

    printf("Please enter quantum time for RR2 of MLQ: ");
    scanf("%d", &timeQ2);
}

//---------------read info of processes from a file---------------//
void readFromFile()
{

    //*create a refrence of a file
    FILE *fPointer;
    //*an array that holds all the lines
    char singleLine[100];

    char *item;
    // record count -->
    int recordCount = 0;

    //*open the file - for reading
    fPointer = fopen("data.txt", "r");

    //*keep looping till the end of the file
    //*fgets(array name,#lines,file)
    while (fgets(singleLine, 99, fPointer))
    {
        if (recordCount == 0)
        {
            num_process = atoi(singleLine);
            printf("Total number of processes=%d \n", num_process);
            recordCount++;
        }
        else if (recordCount == num_process + 1)
        {
            timeQ = atoi(singleLine);
            printf("timeQ=%d \n", timeQ);
            recordCount++;
        }
        else if (recordCount == num_process + 2)
        {
            timeQ1 = atoi(singleLine);
            printf("timeQ1=%d \n", timeQ1);
            recordCount++;
        }
        else if (recordCount == num_process + 3)
        {
            timeQ2 = atoi(singleLine);
            printf("timeQ2=%d \n", timeQ2);
            recordCount++;
        }
        else
        {
            item = strtok(singleLine, " ");
            procArray[recordCount - 1].arrive_t = atoi(item);

            item = strtok(NULL, " ");
            procArray[recordCount - 1].burst_t = atoi(item);
            procArray[recordCount - 1].remaining_t = procArray[recordCount - 1].burst_t;
            procArray[recordCount - 1].proc_num = recordCount - 1;
            recordCount++;
        }
    }
    fclose(fPointer);
}

//---------------Reset the variables---------------//
void reset()
{
    for (int y = 0; y < num_process; y++)
    {
        procArray[y].wait_t = 0;
        procArray[y].turnaround_t = 0;
        procArray[y].remaining_t = procArray[y].burst_t;
        procArray[y].end_t = 0;
    }

    for (int y = 0; y < pInFCFSQ; y++)
    {
        FCFSQ[y].wait_t = 0;
        FCFSQ[y].turnaround_t = 0;
        FCFSQ[y].remaining_t = FCFSQ[y].burst_t;
        FCFSQ[y].end_t = 0;
    }

    for (int y = 0; y < pInRR2; y++)
    {
        RRQ2[y].wait_t = 0;
        RRQ2[y].turnaround_t = 0;
        RRQ2[y].remaining_t = RRQ2[y].burst_t;
        RRQ2[y].end_t = 0;
    }

    for (int y = 0; y < pInRRQ; y++)
    {
        roundRobinQ[y].wait_t = 0;
        roundRobinQ[y].turnaround_t = 0;
        roundRobinQ[y].remaining_t = roundRobinQ[y].burst_t;
        roundRobinQ[y].end_t = 0;
    }

    for (int y = 0; y < nOfFinal; y++)
    {
        finalQ[y].wait_t = 0;
        finalQ[y].turnaround_t = 0;
        finalQ[y].remaining_t = finalQ[y].burst_t;
        finalQ[y].end_t = 0;
    }

    tot_turnaround_t = 0;
    tot_wait_t = 0;
    currentTime = 0;
    idleTime = 0;
    nOfFinal = 0;
    pInFCFSQ = 0;
    pInRR2 = 0;
    pInRRQ = 0;
}

void FCFS()
{
    printf("\n---------------------------------------FCFS Scheduling---------------------------------------\n");
    fprintf(fp, "\n---------------------------------------FCFS Scheduling---------------------------------------\n");

    // Calculate the end time of the processes
    calcEndTime(num_process, procArray);
    // Calculate WT & TAT
    waitingT_turnaroundT(num_process, procArray);
    // Calculate average of WT & TAT
    AvgTime(num_process, procArray);
    // Display the results
    results(num_process, procArray);
    // Write the results in the file
    writeInFile(num_process, procArray);
    printf(" \n");
    fprintf(fp, " \n");
    // reset the variables
    reset();
}

void RR()
{
    printf("\n--------------------------------Round Robin Scheduling--------------------------------\n");
    fprintf(fp, "\n--------------------------------Round Robin Scheduling--------------------------------\n");

    // Enter the first process to the ready queue
    readyQ[0] = procArray[0];

    // pInRQ indicates the numer of processes in ready queue
    pInRQ = 1;

    int p = 1;
    int m = 0;
    int exe = 0;

    // In case the first process doesn't start at 0 (idle time)
    if (readyQ[0].arrive_t > 0)
    {
        idleTime = readyQ[0].arrive_t;
        currentTime += idleTime;
    }

    // Start of RR code
    for (int i = 0; i < pInRQ; i++)
    {

        // First case: the process's remaining time is less than quantum time
        if (readyQ[i].remaining_t <= timeQ && readyQ[i].remaining_t > 0)
        {
            // adjust current time
            currentTime += readyQ[i].remaining_t;
            // adjust remaining time
            readyQ[i].remaining_t = 0;
            // Increment #of executions
            exe++;

            // Check if a new process arrived
            for (int j = p; j < num_process; j++)
            {
                if (procArray[j].arrive_t <= currentTime)
                {
                    // add it to ready queue
                    readyQ[pInRQ] = procArray[j];
                    // increment number of processes in ready queue
                    pInRQ++;
                    p++;
                    m++;
                }
            }
        } // End of first case

        // Second case: the process's remaining time is greater than quantum time
        else if (readyQ[i].remaining_t > timeQ)
        {
            // adjust remaining time
            readyQ[i].remaining_t = readyQ[i].remaining_t - timeQ;
            // adjust current time
            currentTime += timeQ;
            // Increment #of executions
            exe++;

            // Check if a process arrived while the previous process was executed
            // The loop starts at p (because we want to check only the new processes)
            for (int j = p; j < num_process; j++)
            {
                if (procArray[j].arrive_t <= currentTime)
                {
                    // add it to the ready queue
                    readyQ[pInRQ] = procArray[j];
                    // increment the number of processes in ready queue
                    pInRQ++;
                    p++;
                    m++;
                }
            }

            // Add the process again to ready queue
            readyQ[pInRQ] = readyQ[i];
            // increment the number of processes in ready queue
            pInRQ++;

        } // end of second case

        // In case the process's remaining time is 0 (fully executed)
        if (readyQ[i].remaining_t == 0)
        {
            // adjust end time
            readyQ[i].end_t = currentTime;
            // save the number of the process then add it to the final queue
            int r = readyQ[i].proc_num;
            finalQ[r] = readyQ[i];
            nOfFinal++;
        }

        // if the number of execution = processes in ready queue (meaning all processes in RQ executed)
        if (exe == pInRQ)
        {
            // check if th next process arrival time is greater than current time --> there's idle time
            if (procArray[m + 1].arrive_t >= currentTime)
            {
                // calculate idel time
                idleTime = procArray[m + 1].arrive_t - currentTime;
                // increment the current time
                currentTime += idleTime;
                // add the process to the ready queue
                readyQ[pInRQ] = procArray[m + 1];
                // increment the number of processes in ready queue
                pInRQ++;
                p++;
                m++;
            }
        }

    } // End of [for(int i=0;i<pInRQ;i++)]

    // calculate WT & TAT
    waitingT_turnaroundT(nOfFinal, finalQ);
    // calculate average WT & TAT
    AvgTime(nOfFinal, finalQ);
    // Display the results
    results(nOfFinal, finalQ);
    // Write the results in the file
    writeInFile(nOfFinal, finalQ);
    printf(" \n");
    fprintf(fp, " \n");
    // reset the variables
    reset();
}

void MultiLevel2()
{
    printf("\n-----------------------------Multilevel Scheduling: 2 Levels-----------------------------\n");
    fprintf(fp, "\n-----------------------------Multilevel Scheduling: 2 Levels-----------------------------\n");

    // copy the main array and put the results in round robin queue
    copyArray(procArray);
    // adjust the number of processes in RR queue
    pInRRQ = num_process;


    int j = 0;

    // check if there's idle time in the beginning
    if (roundRobinQ[0].arrive_t > 0)
    {
        idleTime = roundRobinQ[0].arrive_t;
        currentTime = idleTime;
    }

    // start of multi level scheduling code
    for (int i = 0; i < pInRRQ; i++)
    {

        // In case the processes (in RR queue) has remaiing time < q
        if (roundRobinQ[i].remaining_t <= timeQ && roundRobinQ[i].remaining_t > 0)
        {
            // increment the current time
            currentTime += roundRobinQ[i].remaining_t;
            // adjust remaining time
            roundRobinQ[i].remaining_t = 0;
            // adjust the end time
            roundRobinQ[i].end_t = currentTime;
            // add the process to the final queue
            finalQ[nOfFinal] = roundRobinQ[i];
            // increment number of processes in Final queue
            nOfFinal++;
        }

        // In case the processes (in RR queue) has remaiing time > q
        else if (roundRobinQ[i].remaining_t > timeQ)
        {
            // adjust the remaining time
            roundRobinQ[i].remaining_t = roundRobinQ[i].remaining_t - timeQ;
            // increment the current time
            currentTime = currentTime + timeQ;
            // add the process to the FCFS queue
            FCFSQ[pInFCFSQ] = roundRobinQ[i];
            // increment number of process in FCFS queue
            pInFCFSQ++;
        }
        
        // If we didn't reach to the end of RR queue
        if (i != pInRRQ - 1)
        {
            // Case1: the next process arrival's time greater than current time & the FCFS queue isn't empty
            if (roundRobinQ[i + 1].arrive_t > currentTime && pInFCFSQ != 0)
            {
                int flag = 1;
                while (flag == 1)
                {
                    // increment the current time
                    currentTime += FCFSQ[j].remaining_t;
                    // adjust the end time
                    FCFSQ[j].end_t = currentTime;
                    // add the process to final queue and increment the number of processes in final queue
                    finalQ[nOfFinal] = FCFSQ[j];
                    nOfFinal++;
                    j++;

                    // check again if the next process arrived
                    if (roundRobinQ[i + 1].arrive_t < currentTime)
                    {
                        // set the flag to zero and stop the while loop
                        flag = 0;
                    }
                }
            }
            // Case2: the next process arrival's time greater than current time & the FCFS queue is empty
            else if (roundRobinQ[i + 1].arrive_t > currentTime && pInFCFSQ == 0)
            {
                // calculate the idle time
                idleTime = roundRobinQ[i + 1].arrive_t - currentTime;
                // increment the current time
                currentTime += idleTime;
            }
        }

        // If we reach to the end of RR queue
        else if (i == pInRRQ - 1)
        {
            for (int k = j; k < pInFCFSQ; k++)
            {
                // increment the current time
                currentTime += FCFSQ[k].remaining_t;
                // adjust the end time
                FCFSQ[k].end_t = currentTime;
                // add the process to the final queue and increment the number of processes in final queue
                finalQ[nOfFinal] = FCFSQ[j];
                nOfFinal++;
                j++;
            }
        }
    }

    // calculate WT & TAT
    waitingT_turnaroundT(nOfFinal, finalQ);
    // calculate average WT & TAT
    AvgTime(nOfFinal, finalQ);
    // Display the results
    results(nOfFinal, finalQ);
    // Write the results in the file
    writeInFile(nOfFinal, finalQ);
    printf(" \n");
    fprintf(fp, " \n");
    // reset the variables
    reset();
    printf("\n");
}

void MultiLevel3()
{

    printf("\n-----------------------------Multilevel Scheduling: 3 Levels-----------------------------\n");
    fprintf(fp, "\n-----------------------------Multilevel Scheduling: 3 Levels------------------------------\n");

    // Copy the main queue to the RR queue
    copyArray(procArray);
    // Set the number of processes in RR queue to the number of processes
    pInRRQ = num_process;

    int s = 0;
    int j = 0;

    // Check if there's idle time in the beginning
    if (roundRobinQ[0].arrive_t > 0)
    {
        idleTime = procArray[0].arrive_t;
        currentTime = idleTime;
    }

    // The beginning of Multi-level [3queues]
    for (int i = 0; i < pInRRQ; i++)
    {
        // In case the processes (in RR queue) has remaiing time < q
        if (roundRobinQ[i].remaining_t <= timeQ1 && roundRobinQ[i].remaining_t > 0)
        {
            // Increment current time
            currentTime += roundRobinQ[i].remaining_t;
            // Adjust remaining time
            roundRobinQ[i].remaining_t = 0;
            // set the end time
            roundRobinQ[i].end_t = currentTime;
            // Add the process to the final queue
            finalQ[nOfFinal] = roundRobinQ[i];
            // Increment number of processes in final queue
            nOfFinal++;
        }

        // In case the processes (in RR queue) has remaiing time > q
        else if (roundRobinQ[i].remaining_t > timeQ1)
        {
            // Ajust remaining time
            roundRobinQ[i].remaining_t -= timeQ1;
            // Increment current time
            currentTime += timeQ1;
            // Add the process to the second queue
            RRQ2[pInRR2] = roundRobinQ[i];
            // Increment number of processes in the second queue
            pInRR2++;
        }

        // In case we reach to the end of first RR queue
        if (i == pInRRQ - 1)
        {
            // Start to execute processes in the second RR queue depending on their remaining time
            for (int k = j; k < pInRR2; k++)
            {
                // In case the remaining time <= q2
                if (RRQ2[k].remaining_t <= timeQ2 && RRQ2[k].remaining_t > 0)
                {
                    // increment current time
                    currentTime += RRQ2[k].remaining_t;
                    // adjust remaining time
                    RRQ2[k].remaining_t = 0;
                    // set the end time
                    RRQ2[k].end_t = currentTime;
                    // add the process to the final queue
                    finalQ[nOfFinal] = RRQ2[k];
                    // increment number of processes in final queue
                    nOfFinal++;
                }

                // In case the remaining time > q2
                else if (RRQ2[k].remaining_t > 0)
                {
                    // ajsust the remaining time
                    RRQ2[k].remaining_t -= timeQ2;
                    // increment the current time
                    currentTime += timeQ2;
                    // add the process to FCFS queue
                    FCFSQ[pInFCFSQ] = RRQ2[k];
                    // increment number of processes in FCFS queue
                    pInFCFSQ++;
                }

                // In case we reach to the end of the second RR queue
                if (k == pInRR2 - 1)
                {
                    // start executing the processes in FCFS queue
                    for (int n = s; n < pInFCFSQ; n++)
                    {
                        // increment the current time
                        currentTime += FCFSQ[n].remaining_t;
                        // adjust the end time of the process
                        FCFSQ[n].end_t = currentTime;
                        // add the process to the final queue
                        finalQ[nOfFinal] = FCFSQ[n];
                        // increment number of processes in final queue
                        nOfFinal++;
                        s++;
                    }
                }
            }
        }
    }

    // calculate WT & TAT
    waitingT_turnaroundT(nOfFinal, finalQ);
    // calculate average WT & TAT
    AvgTime(nOfFinal, finalQ);
    // print the results
    results(nOfFinal, finalQ);
    // write the results in the file
    writeInFile(nOfFinal, finalQ);
    printf(" \n");
    fprintf(fp, " \n");
}

//-------------------------------------End of functions-------------------------------------//

int main()
{
    int execution = 1;
    while (execution == 1)
    {
        fp = fopen("results.txt", "a");
        printf("\n\n\nWelcome to CPU Scheduling Program \n");
        printf("Would you like to: \n 1. Read from a file.\n 2. Read from the console. \n 3. Exit.\n\n");
        printf("Enter Your choice:");
        scanf("%d", &readMethod);

        if (readMethod == 1)
        {
            readFromFile();
            sortProcess(procArray);
            FCFS();
            RR();
            MultiLevel2();
            MultiLevel3();
            readMethod = 0;
        }
        else if (readMethod == 2)
        {
            readFromConsole();
            sortProcess(procArray);
            FCFS();
            RR();
            MultiLevel2();
            MultiLevel3();
            readMethod = 0;
        }
        else if (readMethod == 3)
        {
            printf("\nWe hope you like our program ;)\n\n");
            execution = 0;
        }
        else
        {
            printf("\nTry again with a valid number :)");
            return -1;
        }

        // Sort the processes according to arrival time
        //sortProcess(procArray);

        // close the file
        fclose(fp);
        reset();
    }
    return 0;
} // end of main
