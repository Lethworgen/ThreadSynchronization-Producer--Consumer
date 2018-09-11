Name  :Anirudh Mohan
Date  :11/17/2017
Class :CS 415

Compile Instructions: gcc -I -Wall pandc.c -o pandc -lpthread


Run Instructions: ./pandc 7 5 3 15 2 2 

Project Description:
A Producer-Consumer program that take in elements from command line and should create/initialize the Bounded Buffer Queue, print a timestamp, spawn off C consumer threads & P producer threads, wait for all of the threads to finish and then print off another timestamp & the duration of execution. 