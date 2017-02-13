//
//  ElevatorSystem.h
//
//  by Lucas (Jiahuan.Liu)
//	jiahaun.liu@outlook.com
//
//  02/12/2017
//
#ifndef _ELEVATOR_SYSTEM_
#define _ELEVATOR_SYSTEM_

#include <vector>
#include <queue>
#include <list>

#include <functional>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using std::vector;
using std::queue;
using std::list;
using std::function;
using std::pair;
using std::cout;
using std::endl;

enum State {READY = 1, BUSY = 2, PICKUP = 4};
enum Direction {UP = 1, DOWN = -1};

const int FLOOR_NUM = 80;

#define _DEBUG 1;//print result or not

//a user request
struct Request
{
	Request()
		:currFloor(1), destFloor(1), direction(Direction::UP) {}
	Request(int curr, int dest, Direction dir)
		:currFloor(curr), destFloor(dest), direction(dir) {}
	int			currFloor;
	int			destFloor;
	Direction	direction;
};

struct Status
{
	Status() :floor(1), direction(Direction::UP), state(State::READY) {}
	Status(int floor, Direction dir, State state)
		:floor(floor), direction(dir), state(state) {}
	
	int			floor;
	Direction	direction;
	State		state;
};

struct Elevator
{
	Elevator() :dest(1), pickup(1) {memset(stops, 0, sizeof(stops)/sizeof(bool));}
	Elevator(int eid) :eid(eid), dest(1), pickup(1) {memset(stops, 0, sizeof(stops)/sizeof(bool));}
	//determine dest and make a movement
	void moveOnce();
	void updateDirection(int floor);
	
	int			eid;
	Status		status;
	int			dest;
	int			pickup;
	bool		stops[FLOOR_NUM + 1];
	//ReqQueue	pickupQueue;
};

using ReqQueue = queue<Request>;
using ElevPtr = Elevator*;
using ElevList = list<ElevPtr>;
using DistFunc = function<int(const Elevator&, const Request&)>;
using DistVec = vector<pair<int, ElevPtr>>;

//elevators container and controller logic
class Elevators
{
public:
	Elevators(int elevNum);
public:
	void			step();
	void			update(int eid, const Status& status) {_elevVec[eid].status = status;}
	void			assign(int eid, const Request& req);
	Status			status(int eid) const {return _elevVec[eid].status;}
	ElevList		getElev(State state);
	ElevList		getAllElev();
	std::string		getStatusStr() const {return "";}
private:
	vector<Elevator> _elevVec;
};

//main algorithm of control system
class ElevatorSystem
{
public:
	ElevatorSystem(int elevNum) :_elevators(elevNum) {}
	
	//time step. handle requests and move elevator forward.
	void step()
	{
		static std::string splitLine(40, '-');
		cout << splitLine << endl;
		
		ElevList readyElev = _elevators.getElev(State::READY);
		
		_handleRequest(readyElev, _pendReqs);
		
		ElevList allElev = _elevators.getAllElev();
		
		_handleRequest(allElev, _currReqs);
		
		while (!_currReqs.empty())
		{
			_pendReqs.push(_currReqs.front());
			_currReqs.pop();
		}
		
		_elevators.step();
	}
	
	//accept a passenger pickup request
	void pickup(const Request& req)
	{
		//put into current request queue
		_currReqs.push(req);
	}
	
	//update status of an elevator by id
	void update(int eid, const Status& status)
	{
		_elevators.update(eid, status);
	}
	
	//get status of an elevator by id
	Status status(int eid)
	{
		return _elevators.status(eid);
	}
	
	//display current status of all elevators
	void printStatus() {}
	
private:
	void _handleRequest(ElevList& elevs, ReqQueue& requests)
	{
		Request req;
		ElevPtr elev;
		
		while (!requests.empty() && !elevs.empty())
		{
			req = requests.front();
			requests.pop();
			
			elev = _selectElev(elevs, req);
			
			if (elev)
			{
				_elevators.assign(elev->eid, req);
			}
			else
			{// no potential elevator, put this request into pending queue.
			//	request from _pendReqs should never come here
				_pendReqs.push(req);
			}
		}
	}
	
	static int _dist(const Elevator& elevator, const Request& request)
	{
		int dist = FLOOR_NUM;
		
		if (elevator.status.state == State::READY || _canPassBy(elevator, request))
		{// potential elevator if it is ready or gonna pass by the passenger
			dist = abs(elevator.status.floor - request.currFloor);
		}
		
		return dist;
	}
	
	static bool _canPassBy(const Elevator& elev, const Request& req)
	{
		return (elev.status.state == State::BUSY &&
				elev.status.direction * req.direction > 0 &&
				elev.status.direction * (req.currFloor - elev.status.floor) > 0);
	}
	
	/*static bool _canPickUp(const Elevator& elevator, const Request& request)
	{
		if (elevator.status.state == READY)
		{
			return true;
		}
		else if (elevator.status.state == BUSY)
		{
			return elevator.status.direction * request.direction > 0 && //same way
				(request.currFloor - elevator.status.floor) * elevator.status.direction > 0; //has not passed by
		}
		else
		{
			//has not passed by and destination on the same way
		}
	}*/
	
	//select best elevator and remove from the queue
	static ElevPtr _selectElev(ElevList& elevs, const Request& request, const DistFunc& distFunc = _dist)
	{
		if (elevs.empty()) return nullptr;
	
		vector<pair<int, ElevPtr>> scoreVec;
		
		for (ElevList::iterator itr = elevs.begin(); itr != elevs.end(); itr++)
		{
			scoreVec.push_back({distFunc(**itr, request), *itr});
		}
		
		auto minPair = _selectMin(scoreVec);
		
		ElevPtr elev = nullptr;
		
		if (minPair.first < FLOOR_NUM)
		{// for a valid potential elevator
			elev = minPair.second;
			
			//remove from the list to avoid repeating
			for (ElevList::iterator itr = elevs.begin(); itr != elevs.end();)
			{
				if (elev->eid == (*itr)->eid)
				{
					elevs.erase(itr++);
				}
				else
				{
					itr++;
				}
			}
		}
		
		return elev;
	}
	
	static pair<int, ElevPtr> _selectMin(vector<pair<int, ElevPtr>>& scoreVec)
	{
		pair<int, ElevPtr> minPair = scoreVec.front();
		
		for (vector<pair<int, ElevPtr>>::iterator itr = scoreVec.begin() + 1;
			 itr != scoreVec.end(); itr++)
		{
			if (itr->first < minPair.first)
			{
				minPair = *itr;
			}
		}
		return minPair;
	}
	
private:
	Elevators		_elevators;
	ReqQueue		_currReqs;
	ReqQueue		_pendReqs;
};

#endif