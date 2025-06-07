# TrailBot
A robot car that detects an object using ultrasonic sensors and follow the object keeping a specified distance. The development process involved a real time system developed using Linux, raspberry pi and C programming language to include multi-threaded algorithm.

## Design Pattern
* Embedded program to get starting with parallel programming of the  Use the ultrasonic sensors to drive forward but keep distance of 1m once an object sensed. At the same time use the left and right infrared (IR) sensor to stop if they sense an object.
* The program works such that the ultrasonic measurement is done in a separate thread and new measurements are communicated to the main thread.
