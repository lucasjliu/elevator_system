//
//  test.cpp
//
//  by Lucas (Jiahuan.Liu)
//	jiahaun.liu@outlook.com
//
//  02/12/2017
//

#include "ElevatorSystem.h"

class TestCase
{
public:
	TestCase(int elevNum = 16, vector<vector<Request>>&& steps = {{}})
		:elevatorNum(elevNum), steps(std::forward<vector<vector<Request>>>(steps)){}
	
	virtual void run()
	{
		ElevatorSystem es(elevatorNum);
		for (vector<Request>& step : steps)
		{
			for (Request& req : step)
			{
				es.pickup(req);
			}
			es.step();
		}
		int loop = 30; //system does not self-terminate, so make a loop for testing
		while (loop--)
		{
			es.step();
		}
	}
private:
	int elevatorNum;
	vector<vector<Request>> steps;
};

int main()
{
	vector<vector<Request>> test0 =
	{
		{
			{1, 5, UP},
			{2, 6, UP},
			{1, 4, UP},
		},
		{
			{1, 7, UP},
			{1, 7, UP},
		},
	};
	TestCase(2, std::move(test0)).run();
	
	/*
	elevator #0 picks up req #0 at floor 1
	elevator #1 picks up req #1 at floor 2
	elevator #0 stops at floor 5
	elevator #1 stops at floor 6
	elevator #0 picks up req #2 at floor 1
	elevator #1 picks up req #3 at floor 1
	elevator #0 stops at floor 4
	elevator #0 picks up req #4 at floor 1
	elevator #1 stops at floor 7
	elevator #0 stops at floor 7
	*/
	
	return 0;
}