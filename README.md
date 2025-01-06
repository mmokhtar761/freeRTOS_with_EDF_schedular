# EDF Scheduler Implementation Using FreeRTOS

## Project Overview
This project involves the implementation of an Earliest Deadline First (EDF) scheduler on FreeRTOS. The goal was to modify the FreeRTOS kernel to support dynamic priority scheduling, replacing its default static priority-based scheduler. The EDF scheduler assigns task priorities based on their deadlines, ensuring that tasks with the closest deadlines are given the highest priority.

## Requirements
- Thesis Reference: "Implementation and Test of EDF and LLREF Schedulers in FreeRTOS" by Enrico Carraro.
- Hardware:
  * ST STM32F429 Discovery Board
  * ARM Cortex M4 (180 MHz)
  * 2 MB Flash, 256 KB RAM
  * 2.4” QVGA TFT LCD
- Software:
  * FreeRTOS v8.2.2
  * GCC Compiler
  * Keil Simulator
  * Simso Offline Simulator
  * CooCox CoIDE

## Implementation Details
The EDF scheduler was implemented by modifying the FreeRTOS kernel, particularly the `tasks.c` file. Key changes include:

1. **New Ready List:**  
   A custom ready list (`xReadyTasksListEDF`) was created to store tasks in ascending order of their deadlines.

2. **Task Creation:**  
   A new task creation function `xTaskPeriodicCreate` was added to initialize tasks with periodic behavior and deadlines.

3. **Idle Task Management:**  
   The idle task was assigned the farthest deadline to ensure it runs only when no other tasks are ready.

4. **Context Switching:**  
   The `vTaskSwitchContext` function was updated to always select the task with the nearest deadline for execution.

5. **Trace Macros:**  
   Custom trace macros were added to log task switches and delays during runtime.

## Tasks Description
Six tasks were implemented to verify the EDF scheduler:

1. **Button_1_Monitor**  
   - Periodicity: 50 ms, Deadline: 50 ms  
   - Monitors rising and falling edges of button 1.  
   - Execution Time: 15 µs

2. **Button_2_Monitor**  
   - Periodicity: 50 ms, Deadline: 50 ms  
   - Monitors rising and falling edges of button 2.  
   - Execution Time: 15 µs

3. **Periodic_Transmitter**  
   - Periodicity: 100 ms, Deadline: 100 ms  
   - Sends periodic data every 100 ms.  
   - Execution Time: 20 µs

4. **Uart_Receiver**  
   - Periodicity: 20 ms, Deadline: 20 ms  
   - Receives and displays data on UART.  
   - Execution Time: 22 µs

5. **Load_1_Simulation**  
   - Periodicity: 10 ms, Deadline: 10 ms  
   - Simulates a heavy load with an execution time of 5 ms.

6. **Load_2_Simulation**  
   - Periodicity: 100 ms, Deadline: 100 ms  
   - Simulates a heavy load with an execution time of 12 ms.

## Verification Methods
The system implementation was verified using three methods:

1. **Analytical Methods:**  
   - Calculated the system hyperperiod as 100 ms.  
   - CPU load was calculated as 0.622 (62.2%).  
   - Verified schedulability using utilization rate and time demand analysis.

2. **Simso Simulation:**  
   - Simulated the tasks using the Simso offline scheduler.  
   - Generated Gantt charts and verified correct EDF scheduling behavior.

3. **Keil Runtime Simulation:**  
   - Used Keil simulator to run the application.  
   - Verified task execution using GPIOs and a logic analyzer.  
   - Measured CPU usage and plotted task execution on the logic analyzer.

## Results
1. CPU Load: 62.2% during a 100 ms hyperperiod.
2. Keil Simulation:
   - The logic analyzer confirmed correct task switching according to EDF scheduling.
   - Trace logs showed correct preemptive and cooperative behavior.
   ![Keil Logic Analyzer Output - Full](EDF_Demo%20KeilARM7_LPC21xx/Tests/02-%20All%20tasks%20implemented%20and%20executing%20well.png)

3. Simso Simulation:
   - The Gantt charts from Simso matched the expected task execution order.

The results confirmed that the implemented EDF scheduler works as intended, ensuring timely task execution and system schedulability.

## How to Run the Project
1. **Setup the Development Environment:**  
   - Install GCC compiler and CooCox CoIDE.  
   - Connect the STM32F429 Discovery board.

2. **Build and Flash:**  
   - Open the project in CooCox CoIDE.  
   - Compile the code and flash it onto the board.

3. **Simulate in Keil:**  
   - Load the project in Keil simulator.  
   - Run the simulation and observe the task execution using trace macros and the logic analyzer.

4. **Simulate in Simso:**  
   - Define the tasks in Simso.  
   - Run the simulation and verify the task scheduling.
