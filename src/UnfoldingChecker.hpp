#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <array>
#include <set>
#include<list>
namespace simgrid {
namespace mc {

class UnfoldingEvent;
class EventSet;
class Transition;
class Actor;
class IntSet;
extern int nb_events;
extern EventSet U, G;

class Transition {
public:

	int actor_id = 0;
	int id = 0;
	//int isMutex = 0;  //  default value is not mutexed transition, 1 is lock , 2 unlock

	bool executed = false;
	int read_write = 0; // default value is read (= 0), write =1, if mutex lock = 2, unlock = 3
	int access_var = 0; // default this transition access variable 0

	// i can be executed if only depending transition has been executed
	int depending_actor = -1;
	int depending_tran = -1;


	bool isDependent(Transition other);
	Transition() {	}	;

	bool operator<(const Transition& other) const;
	bool operator==(const Transition& other) const;

	Transition(int read_write, int access_var);
	void execute();
	bool isDependingTransExecuted( std::set<Actor> actors);
	//Transition *depending_tran;

};
class Actor {
public:
	int id = 0;
	int nb_trans = 0;
	std::array<Transition, 10> trans;
	Actor() {
	}
	;
	Actor(int id, int nb_trans, std::array<Transition, 10> &trans);
	bool operator<(const Actor& other) const;

};

class State {
public:
	int nb_actor = 0;
	bool initialState = false;
	std::set<Actor> actors;

	State() {
	}
	;
	State(int nb_actor, std::set<Actor> actors);

	std::set<Transition> getEnabledTransition();
	State execute(Transition t);
	void print();
};

class EventSet {
public:
	bool contains(UnfoldingEvent *e);
	bool depends(EventSet s2);
	bool isConfig();
	static EventSet makeUnion(EventSet s1, EventSet s2);
	static EventSet makeIntersection(EventSet s1, EventSet s2);

	const UnfoldingEvent* first();
	size_t size();
	bool empty() const;
	UnfoldingEvent* begin() const;
	UnfoldingEvent* end() const;

	void insert(UnfoldingEvent*);
	void erase(UnfoldingEvent*); // adding
	bool operator==(const EventSet& other) const;

	std::set<UnfoldingEvent*> events_;
	EventSet minus(UnfoldingEvent *evt);
	EventSet plus(UnfoldingEvent *);

};

class Configuration: public EventSet {
public:
	EventSet maxEvent; // Events recently added to events_
	//std::list<std::set<int> > *maxEventHis;// store history of maxEvent

	UnfoldingEvent * lastEvent; // The last added event
	void getEnabledTransition(std::set<Transition> whereto);
	Configuration plus(UnfoldingEvent *);
	void createEvt(EventSet& result, Transition t, EventSet causuality_events,
			EventSet cause, EventSet candidateHistory);
	void updateMaxEvent(UnfoldingEvent *);

};
class IntSet{
public:
 std::set<int> ints;
  IntSet(){};
  void insert(int e);
  bool inculude(IntSet other);

};

class UnfoldingEvent {
public:
	int id = -1;
	State appState;
	Transition transition; // The last transition made to reach that state
	EventSet causes;        // used to store directed ancestors of event e
	UnfoldingEvent() {
	}
	;
	UnfoldingEvent(int nb_events, Transition t, EventSet causes);

	EventSet getHistory() const;
	EventSet minimalConfig() const;

	bool isConflict(UnfoldingEvent *otherEvent);
	bool isImmediateConflict(UnfoldingEvent* other);
	bool conflictWithConfig(Configuration config);

	bool checkMutexedTransition(int mutexId);

	void getEnabledTransition(std::set<Transition*>* whereto);
	void execute();

	bool operator<(const UnfoldingEvent& other) const;
	bool operator==(const UnfoldingEvent& other) const;
	void print();
};

class UnfoldingChecker {
	EventSet A, D;
	Configuration C;
	unsigned long expandedStatesCount_ = 0;
	int Mode = 1; // Mode = 1 is a mutexed model
//  static Session& getSession();

public:
	void explore(Configuration C, std::list<IntSet*> maxEvtHistory, EventSet D, EventSet A,
			UnfoldingEvent *currentEvt, EventSet prev_enC,
			std::set<Actor> proc);


	void extend(std::set<Actor> proc, Configuration C, std::list<IntSet*> maxEvtHistory, EventSet & ExC,
			EventSet& enC);
	void remove(UnfoldingEvent *e, Configuration C, EventSet D);
	void computeAlt(EventSet& J, EventSet D, Configuration C, EventSet U1,
			EventSet U);
	void genEventFromCandidate(EventSet& result, Transition t,
			UnfoldingEvent* preEvt, EventSet U1, EventSet Uc);
//  static Session& session;
};
}
}

