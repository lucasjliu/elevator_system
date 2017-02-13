# Elevator Control System

This is a simulation of basic elevator control system as described [here](https://mesosphere.com/jobs/challenges/intern/index.html). It shows how multiple elevators are scheduled by a greedy algorithm to serve passenger pickup requests

## Design

#### Problem Statement

The control system mainly include a vector of **Elevator** objects handling **Request** in a queue. In each time period, triggered by `step()` function call, Elevators can move at most one floor, requests generated in this period are queued to be handled sequentially, and passengers can enter or exit an elevator that currently stops. A pickup request contains a start floor and destincation floor. Capacity, door open time are ignored, so passengers can enter or exit an elevator in only one step. So the goal of the system is to select the best elevator to pick up and serve the passenger, and make sure it stops at correct pick up or destination floors.

#### Scheduling Algorithm

We first discuss the stopping problem. State of an elevator can be `READY` when it is unused, `BUSY` when it is in service, and `PICKUP` when it is on the way to pick up a passenger. So for a `BUSY` elevator, it just keep a vector of stop floors and print out a message when it is on one of the stop floors. Here I implemented with a boolean array as flags for each floor because the floor number is usually constant value, thus space complexity is still constant, while it achieve constant time querying and updating. Update would be O(logn) for vector implementation. And for a `PICKUP` elevator, it also keep the stop floors, but never print out until it reaches the pick up point, starts serving and becomes a `BUSY` one.

For the selection problem, solution is find all the potential elevators for a request, compute scores according to certain optimization criteria, and find the optimal one. The optimizatin criteria I used here is: serve as many passengers as possible and minimize their waiting time. So an `BUSY` elevator should also be considered for a pickup request if it is about to pass by the passenger and going on the same direction. In this way the waiting time can be reduced compared to FCFS method because passengers have more chance to be served sooner, not just waiting for an unused one. For a `PICKUP` elevator, intuitively if it is picking up on 3rd floor and heading up, it should also consider a pick up request on 2nd floor. Therefore a `PICKUP` elevator is considered 'potential' for a request if the location and destination of request can both be passed by. However, to simplify the implementation I just chose to exclude 'PICKUP' ones from other requests.

Given the algorithm to find potential elevators for a request, the optimal one can be chosen by measurement of absolute distance. Requests with no potential elevators are put into another pending queue, and handled as soon as there is an unused elevators.


## Documentation

#### Modules

* **ELevators** defined in `ElevatorSystem.h`, encapsulates the details of the elevators container and handle batch operation on the it.

* **ElevatorSystem** handles user interface functions and optimization algorithm, modifes status of elevator through a `Elevators` member. 

* **TestCase** defined in `test.cpp`, serves as a user simulation. It receives a vector of pickup requests in each step and calls a `ElevatorSystem` object to handle them.

#### Interface

* `void step()`: as a clock simulation.
* `void pickup(const Request& req)` accept and queue a passenger pickup request, which is sequentially handled at the end of current step.
* `void update(int eid, const Status& status)` update status of an elevator by id
* `Status status(int eid)` get status of an elevator by id
* `void printStatus()` display current status of all elevators (todo)


## Usage Instructions

#### Build
```
mkdir bin
mkdir obj
make
```

#### Running
```
./bin/test
```
This program only has one test case right now. More tests can be added to test.cpp.

#### Example
Consider a test case with two elevators and requests as follow:
```
{{
	{1, 5, UP}, //req #0
	{2, 6, UP}, //req #1
	{1, 4, UP},
},
{
	{3, 4, UP},
	{1, 7, UP},
	{1, 7, UP},
}}
```
It means five requests are generated at the first two steps, where the first argument is start floor, second is destination floor, and third is direction. In a first-come-first-serve manner, two unused elevators are assigned with the first two requests. Req #2 is put into pending queue cause. Elevator picks up and finises req #3 which proves the greedy algorithm. After elevator #0 finishes req #0, it goes down to 1st floor to pick up req#2. And so does elevator #1 for req #4. Finally elevator #1 picks up and finishes req #5.

## Author

This project is created by Jiahuan Liu ([email](jiahuan.liu@outlook.com)), Feb 12th, 2017.