//
//  ElevatorSystem.h
//
//  by Lucas (Jiahuan.Liu)
//	jiahaun.liu@outlook.com
//
//  02/12/2017
//

#include <vector>
#include <queue>
#include <list>
#include <functional>
#include <algorithm>

using std::vector;
using std::queue;
using std::list;
using std::function;
using std::pair;

enum State {UP = 1, DOWN = -1, READY = 0};

const int FLOOR_NUM = 80;

struct Status
{
	int		floor;
	State	state;
};

struct Elevator
{
	bool moveOnce();
	void addStop(int stopFloor);
	
	int		eid;
	Status	status;
	int		dest;
	bool	stops[FLOOR_NUM];
};

//a user request
struct Request
{
	int		floor;
	State	direction;
};

using ElevPtr = Elevator*;
using ElevList = list<ElevPtr>;
using ReqQueue = queue<Request>;
using DistFunc = function<int(const Elevator&, const Request&)>;
using DistVec = vector<pair<int, ElevPtr>>;

//elevators container and controller logic
class Elevators
{
public:
	void			step();
	void			update(int eid, const Status& status);
	void			assign(ElevPtr pElev, const Request& req);
	ElevList		getElev(State state);
	ElevList		getAllElev();
	std::string		getStatusStr() const;
private:
	vector<Elevator> _elevVec;
};

//main algorithm of control system
class ElevatorSystem
{
public:
	//time step. move elevator forward and handle requests
	void step()
	{
		ElevList readyElev = _elevators.getElev(State::READY);
		
		_handleRequest(readyElev, _pendReqs);
		
		ElevList allElev = _elevators.getAllElev();
		
		_handleRequest(allElev, _currReqs);
	}
	
	//accept a passenger pickup request
	void pickup(int floot, State dir);
	
	//update status of an elevator by id
	void update(int eid, const Status& status);
	
	//get status of an elevator by id
	Status status(int eid);
	
	//display current status of all elevators
	void printStatus();
	
private:
	void _handleRequest(ElevList& elevs, ReqQueue& requests)
	{
		Request req;
		ElevPtr elev;
		
		while (!requests.empty() && !elevs.empty())
		{
			req = requests.back();
			requests.pop();
			
			elev = _selectElev(elevs, req);
			
			_elevators.assign(elev, req);
		}
	}
	
	static int _dist(const Elevator& elevator, const Request& request);
	
	//select best elevator and remove from the queue
	static ElevPtr _selectElev(ElevList& elevs, const Request& request, const DistFunc& distFunc = _dist)
	{
		vector<pair<int, ElevPtr>> scoreVec;
		
		for (ElevList::iterator itr = elevs.begin(); itr != elevs.end(); itr++)
		{
			scoreVec.push_back({_dist(**itr, request), *itr});
		}
		
		sort(scoreVec.begin(), scoreVec.end());
		
		ElevPtr elev = scoreVec.back().second;
		
		for (ElevList::iterator itr = elevs.begin(); itr != elevs.end();)
		{
			if (elev->eid == (*itr)->eid)
				elevs.erase(itr++);
			else
				itr++;
		}
		
		return elev;
	}
	
private:
	int			_elevNum;
	Elevators	_elevators;
	ReqQueue	_currReqs;
	ReqQueue	_pendReqs;
};