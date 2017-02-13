//
//  Elevator.cpp
//
//  by Lucas (Jiahuan.Liu)
//	jiahaun.liu@outlook.com
//
//  02/12/2017
//

#include "ElevatorSystem.h"
#include <assert.h>

void Elevator::moveOnce()
{
	if (status.state != State::READY)
	{
		if (status.state == PICKUP)
		{//if PICKUP, do nothing unless reaching the pickup point
			if (status.floor == pickup)
			{//change state to BUSY
				updateDirection(dest);
				status.state = BUSY;
			}
		}
		
		if (status.state == BUSY)
		{
			if (stops[status.floor])
			{//print log if an elevator currently stops
				printf("Elevator #%d stops at floor %d\n", eid, status.floor);
			}
			if (status.floor == dest)
			{//reach dest, so change state to ready, clear, and stay still
				status.state = State::READY;
				memset(stops, 0, sizeof(stops)/sizeof(bool));
			}
		}
		
		status.floor += status.direction;//move up or down
	}
}

void Elevator::updateDirection(int floor)
{
	status.direction = (floor >= status.floor ?
						Direction::UP : Direction::DOWN);
}

Elevators::Elevators(int elevNum) :_elevVec(elevNum)
{
	for (int i = 0; i < _elevVec.size(); ++i)
	{
		_elevVec[i] = Elevator(i);
	}
}

void Elevators::step()
{
	//move elevators
	for (auto& elev : _elevVec)
	{
		elev.moveOnce();
	}
}

void Elevators::assign(int eid, const Request& req)
{
	Elevator& elev = _elevVec[eid];
	
	assert(elev.status.state != State::PICKUP);
	
	if (elev.status.state == State::READY)
	{//a ready elevator accept a pick up request
		elev.status.state = State::PICKUP;
		elev.pickup = req.currFloor;
		elev.updateDirection(req.currFloor);
		elev.dest = req.destFloor;
		elev.stops[req.currFloor] = true;
	}
	else
	{//a buzy elevator pick up a passenger when it passes by
		if (elev.status.direction == Direction::UP)
		{
			elev.dest = std::max(elev.dest, req.destFloor);
		}
		else
		{
			elev.dest = std::min(elev.dest, req.destFloor);
		}
		elev.stops[req.currFloor] = true;
	}
	
	elev.stops[req.destFloor] = true;
}

ElevList Elevators::getElev(State state)
{
	ElevList ret;
	for (auto& elev : _elevVec)
	{
		if (elev.status.state & state)
		{
			ret.push_back(&elev);
		}
	}
	return ret;
}

ElevList Elevators::getAllElev()
{
	ElevList ret;
	for (auto& elev : _elevVec)
	{
		ret.push_back(&elev);
	}
	return ret;
}