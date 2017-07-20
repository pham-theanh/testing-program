//============================================================================
// Name        : test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "UnfoldingChecker.hpp"

#include <iostream>
using namespace std;
using namespace simgrid::mc;

int main() {

	nb_events = 0;
	UnfoldingChecker UC;
	Configuration C;

/*
	 /// the first example (in the paper)
	 Transition t1(0, 1, 1, 0);
	 Transition t2(0, 2, 0, 0);
	 Transition t3(0, 3, 0, 0);

	 std::array<Transition, 10> trans1, trans2, trans3;
	 trans1[0] = t1;
	 trans2[0] = t2;
	 trans3[0] = t3;
	 Actor p1(1, 1, trans1);
	 Actor p2(2, 1, trans2);
	 Actor p3(3, 1, trans3);

	 EventSet A, D;
	 EventSet prev_exC;
	 std::set<Actor> sa;
	 sa.insert(p1);
	 sa.insert(p2);
	 sa.insert(p3);

	 State initState(3, sa);

*/
/*

	 // the second example
	 Transition t1(0, 1, 1, 0);
	 Transition t2(0, 2, 1, 1);
	 Transition t3(1, 2, 1, 0);

	 std::array<Transition, 10> trans1, trans2, trans3;
	 trans1[0] = t1;
	 trans2[0] = t2;
	 trans2[1] = t3;
	 Actor p1(1, 1, trans1);
	 Actor p2(2, 2, trans2);

	 EventSet A, D;
	 EventSet prev_exC;
	 std::set<Actor> sa;
	 sa.insert(p1);
	 sa.insert(p2);
	 State initState(2, sa);
*/



		//the third example
	 Transition t1(0, 1, 1, 0);
	 Transition t2(0, 2, 1, 1);
	 Transition t3(0, 3, 1, 2);

	 std::array<Transition, 10> trans1, trans2, trans3;
	 trans1[0] = t1;
	 trans2[0] = t2;
	 trans3[0] = t3;
	 Actor p1(1, 1, trans1);
	 Actor p2(2, 1, trans2);
	 Actor p3(3, 1, trans3);

	 EventSet A, D;
	 EventSet prev_exC;
	 std::set<Actor> sa;
	 sa.insert(p1);
	 sa.insert(p2);
	 sa.insert(p3);
	 State initState(3, sa);

/*
		//the fourth example
	 Transition t1(0, 1, 1, 0);
	 Transition t2(0, 2, 1, 0);
	 Transition t3(0, 3, 1, 0);

	 std::array<Transition, 10> trans1, trans2, trans3;
	 trans1[0] = t1;
	 trans2[0] = t2;
	 trans3[0] = t3;
	 Actor p1(1, 1, trans1);
	 Actor p2(2, 1, trans2);
	 Actor p3(3, 1, trans3);

	 EventSet A, D;
	 EventSet prev_exC;
	 std::set<Actor> sa;
	 sa.insert(p1);
	 sa.insert(p2);
	 sa.insert(p3);
	 State initState(3, sa);
*/
	/*
	 // the fifth example
	Transition t1(0, 1, 1, 0);
	Transition t2(1, 1, 1, 1);

	Transition t3(0, 2, 1, 0);
	Transition t4(1, 2, 1, 1);

	std::array<Transition, 10> trans1, trans2, trans3;
	trans1[0] = t1;
	trans1[1] = t2;

	trans2[0] = t3;
	trans2[1] = t4;
	Actor p1(1, 2, trans1);
	Actor p2(2, 2, trans2);

	EventSet A, D;
	EventSet prev_exC;
	std::set<Actor> sa;
	sa.insert(p1);
	sa.insert(p2);
	State initState(2, sa);
	 */
	//------------------------------------------------

	initState.initialState = true;
	UnfoldingEvent *e = new UnfoldingEvent();
	e->appState = initState;
	UC.explore(C, D, A, e, prev_exC, sa);

	std::cout << " \n main() finished";
	return 0;
}
