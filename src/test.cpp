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
	EventSet A, D;
	EventSet prev_exC;
	Configuration C;
	State *initState;
	std::set<Actor> sa;
//--------------------------------
	/*UnfoldingEvent  * ee =NULL;
	UnfoldingEvent  ee1;
	//ee=&ee1;

 std::cout<<" gia tri la null"<<ee1.id<<" "<<ee->id;*/
//--------------------------------





	int example =0;
	std::cout<<" enter example:";
	std::cin >> example;
	switch (example) {
	case 1: { // the first example (in the paper)
		Transition t1(1, 0);
		Transition t2(0, 0);
		Transition t3(0, 0);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans2[0] = t2;
		trans3[0] = t3;
		Actor p1(1, 1, trans1);
		Actor p2(2, 1, trans2);
		Actor p3(3, 1, trans3);

		sa.insert(p1);
		sa.insert(p2);
		sa.insert(p3);

		initState = new State(3, sa);

			}
	break;

	case 2: { // the second example
		Transition t1(1, 0);
		Transition t2(1, 1);
		Transition t3(1, 0);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans2[0] = t2;
		trans2[1] = t3;
		Actor p1(1, 1, trans1);
		Actor p2(2, 2, trans2);

		sa.insert(p1);
		sa.insert(p2);
		initState = new State(2, sa);
	}
	break;

	case 3: { //the third example
		Transition t1(1, 0);
		Transition t2(1, 1);
		Transition t3(1, 2);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans2[0] = t2;
		trans3[0] = t3;
		Actor p1(1, 1, trans1);
		Actor p2(2, 1, trans2);
		Actor p3(3, 1, trans3);

		sa.insert(p1);
		sa.insert(p2);
		sa.insert(p3);
		initState = new State(3, sa);
	}
	break;

	case 4: { //the fourth example
		Transition t1(1, 0);
		Transition t2(1, 0);
		Transition t3(1, 0);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans2[0] = t2;
		trans3[0] = t3;
		Actor p1(1, 1, trans1);
		Actor p2(2, 1, trans2);
		Actor p3(3, 1, trans3);

		sa.insert(p1);
		sa.insert(p2);
		sa.insert(p3);
		initState = new State(3, sa);
	}
	break;

	case 5: { // the fifth example
		Transition t1(1, 0);
		Transition t2(1, 1);

		Transition t3(1, 0);
		Transition t4(1, 1);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans1[1] = t2;

		trans2[0] = t3;
		trans2[1] = t4;
		Actor p1(1, 2, trans1);
		Actor p2(2, 2, trans2);

		sa.insert(p1);
		sa.insert(p2);
		initState = new State(2, sa);
	}
	break;


	case 6: { // 6th example
		Transition t1(1, 0);
		Transition t2(1, 1);

		Transition t3(1, 1);
		Transition t4(1, 0);

		std::array<Transition, 10> trans1, trans2, trans3;
		trans1[0] = t1;
		trans1[1] = t2;

		trans2[0] = t3;
		trans2[1] = t4;
		Actor p1(1, 2, trans1);
		Actor p2(2, 2, trans2);

		sa.insert(p1);
		sa.insert(p2);
		initState = new State(2, sa);
	}
	break;

	case 7: { //7th example
				Transition t1(1, 0);
				Transition t2(1, 1);
				Transition t3(1, 1);
				Transition t4(1, 0);


				std::array<Transition, 10> trans1, trans2, trans3;
				trans1[0] = t1;
				trans2[0] = t2;
				trans3[0] = t3;
				trans3[1] = t4;

				Actor p1(1, 1, trans1);
				Actor p2(2, 1, trans2);
				Actor p3(3, 2, trans3);


				//set happen-before relation
			//	p3.trans[0].depending_actor =2;
			//	p3.trans[0].depending_tran =0;


				sa.insert(p1);
				sa.insert(p2);
				sa.insert(p3);
				initState = new State(3, sa);
			}
			break;


	case 8: { // mutex example
		Transition t1(2, 0);
		Transition t2(3, 0);

		Transition t3(2, 0);
		Transition t4(3, 0);

		std::array<Transition, 10> trans1, trans2;
		trans1[0] = t1;
		trans1[1] = t2;

		trans2[0] = t3;
		trans2[1] = t4;
		Actor p2(2, 2, trans2);
		Actor p1(1, 2, trans1);

		sa.insert(p1);
		sa.insert(p2);
		initState = new State(2, sa);
	}
	break;



	case 9: { // mutex example
		Transition t0(0, 0);

		Transition t1(2, 1);
		Transition t2(3, 1);

		Transition t3(2, 1);
		Transition t4(3, 1);

		std::array<Transition, 10> trans1, trans2;
		trans1[0] = t0;
		trans1[1] = t1;
		trans1[2] = t2;

		trans2[0] = t3;
		trans2[1] = t4;
		Actor p1(1, 3, trans1);

		Actor p2(2, 2, trans2);

		sa.insert(p1);
		sa.insert(p2);
		initState = new State(2, sa);
	}
	break;

	}


	initState->initialState = true;
	UnfoldingEvent *e = new UnfoldingEvent();
	e->appState = *initState;
	UC.explore(C, D, A, e, prev_exC, sa);

	std::cout << " \n main() finished ";
	return 0;
}
