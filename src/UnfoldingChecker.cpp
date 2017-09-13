#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//#include "src/mc/mc_state.h"

#include "UnfoldingChecker.hpp"
//#include "xbt/ex.h"
//#include "xbt/asserts.h"

namespace simgrid {
namespace mc {
int nb_events;
EventSet U, G;
//class UnfoldingEvent;

Transition::Transition(int read_write, int access_var) {
	this->read_write = read_write;
	this->access_var = access_var;
}

void Transition::execute() {
	executed = true;
}

bool Transition::isDependingTransExecuted(std::set<Actor> actors) {
	if (this->depending_actor < 0)
		return true;
	for (auto act : actors)
		if (act.id == this->actor_id) {
			if (act.trans[this->depending_tran].executed)
				return true;
			break;

		}
	std::cout << " thang t" << this->id << "-p" << this->actor_id
			<< " phu thuoc vao t" << this->depending_tran << "p"
			<< this->depending_actor << " \n";
	std::cout << " return false \n";
	return false;
}

bool Transition::isDependent(Transition other) {
	int Mode = 1;  // Mode ==1-> mutexed model
	if (this->actor_id == other.actor_id)
		return true;

	//FIXME change value of Mode when changing the model
	if (Mode == 1 and this->read_write < 2)
		return false;

	// check mutexed transitions
	if (this->access_var == other.access_var) {
		if ((this->read_write >= 2 and other.read_write >= 2)) {
			return true;
		}
	}

	// check not mutexed transitions
	if (this->access_var == other.access_var) {
		if ((this->read_write == 1) or (other.read_write == 1)) {
			return true;
		}
	}
	return false;
}

Actor::Actor(int id, int nb_trans, std::array<Transition, 10> &trans) {
	this->id = id;
	this->nb_trans = nb_trans;
	this->trans = trans;
	int tid = 0;

	for (int i = 0; i < nb_trans; i++) {
		this->trans[i].id = tid++;
		this->trans[i].actor_id = id;

	}

}

bool Actor::operator<(const Actor& other) const {
	return (this->id < other.id);
}

State::State(int nb_actor, std::set<Actor> actors) {
	this->nb_actor = nb_actor;
	this->actors = actors;
}
State State::execute(Transition t) {
	std::set<Actor> sp, sp1;
	sp = this->actors;
	sp1 = this->actors;

	for (auto p : sp)
		if (p.id == t.actor_id) {
			sp1.erase(p);
			p.trans[t.id].executed = true;
			sp1.insert(p);

		}

	State nextState(this->nb_actor, sp1);

	return nextState;
}

std::set<Transition> State::getEnabledTransition() {

	std::set<Transition> set_t;
	for (auto p : this->actors)
		for (int j = 0; j < p.nb_trans; j++)
			if (not p.trans[j].executed) {
				//if (p.trans[j].isDependingTransExecuted(actors))
				if (p.trans[j].read_write < 2)
					set_t.insert(p.trans[j]);
				// if p.trans[j] if lock transition, make sure that only one lock can occupy one mutex

				else {
					bool chk = true;
					for (auto trans : set_t)
						if (trans.access_var == p.trans[j].access_var and trans.read_write ==2 and  p.trans[j].read_write ==2) {
							chk = false;
							break;
						}
					if (chk)
						set_t.insert(p.trans[j]);

				}

				break;
			}

	return set_t;
}

void State::print() {
	std::cout << "s = (";
	for (auto p : this->actors)
		for (int j = 0; j < p.nb_trans; j++)
			if (p.trans[j].executed)
				std::cout << "t" << j << "-p" << p.id << " is executed";
	std::cout << ")";

}

/*
 bool EventSet::contains(UnfoldingEvent *e) {
 return events_.find(e) != events_.end();
 }
 */

bool EventSet::contains(UnfoldingEvent *e) {
	for (auto evt : this->events_)
		if (*evt == *e)
			return true;
	return false;
}

/** @brief Check if I'm dependent with another EventSet */
bool EventSet::depends(EventSet s2) {

	for (auto e1 : this->events_)
		for (auto e2 : s2.events_)
			if (*e1 == *e2)
				continue;
			else if (e1->transition.isDependent(e2->transition))
				return true;

	return false;
}

bool EventSet::isConfig() {
	if ((this->size() == 1) && (this->begin()->causes.empty()))
		return true;

	// checking conflict relation between one event and all other events in the set
	for (auto e1 : events_) {
		for (auto e2 : events_) {
			if (e1 == e2)
				continue;

			if (e1->isConflict(e2))
				return false;
		}
		// Every event of the history should be in the set
		for (auto ancestor : e1->getHistory().events_)
			if (not (this->contains(ancestor)))
				return false;
	}

	return true;
}

EventSet EventSet::makeUnion(EventSet s1, EventSet s2) {
	EventSet res;
	res.events_.insert(s1.events_.begin(), s1.events_.end());
	res.events_.insert(s2.events_.begin(), s2.events_.end());
	return res;
}

EventSet EventSet::makeIntersection(EventSet s1, EventSet s2) {
	EventSet res;
	std::set_intersection(s1.events_.begin(), s1.events_.end(),
			s2.events_.begin(), s2.events_.end(),
			std::inserter(res.events_, res.events_.begin()));
	return res;
}

EventSet EventSet::minus(UnfoldingEvent *evt) {

	EventSet res;
	res.events_ = this->events_;
	for (auto e : res.events_)
		if (*e == *evt)
			res.erase(e);

	return res;
}

EventSet EventSet::plus(UnfoldingEvent *evt) {
	EventSet res;
	res.events_ = this->events_;
	res.events_.insert(evt);
	return res;
}

Configuration Configuration::plus(UnfoldingEvent *evt) {
	Configuration res;
	res.events_ = this->events_;
	res.maxEvent = this->maxEvent;
	res.events_.insert(evt);

	return res;
}

size_t EventSet::size() {
	return events_.size();
}

bool EventSet::empty() const {
	return this->events_.empty();
}

UnfoldingEvent* EventSet::begin() const {
	return *events_.begin();
}

UnfoldingEvent* EventSet::end() const {
	return *events_.end();
}

bool EventSet::operator==(const EventSet& other) const {

	return this->events_ == other.events_;

}
void EventSet::insert(UnfoldingEvent* e) {
	events_.insert(e);
}

void EventSet::erase(UnfoldingEvent *e) {
	this->events_.erase(e);
}

UnfoldingEvent::UnfoldingEvent(int nb_events, Transition t, EventSet causes) {

	this->id = nb_events;
	this->causes = causes;
	this->transition = t;
}

void UnfoldingEvent::print() {

	std::cout << "e" << this->id << " =  < t" << this->transition.id << "-p"
			<< this->transition.actor_id << "," << "(";
	if (this->causes.empty())
		std::cout << "-) >";
	else {
		for (auto evt : this->causes.events_)
			std::cout << "e" << evt->id << ",";
		std::cout << " ) >";
	}

}

// Recursively compute the history of a given event by adding the causes of all ancestors
EventSet UnfoldingEvent::getHistory() const {
	if (causes.empty()) // No direct ancestor => empty history
		return causes;
	else {
		EventSet res = causes;
		for (auto ancestor : causes.events_) {
			EventSet h1 = ancestor->getHistory();
			//res.events_.insert(h1.begin(), h1.end());
			res = res.makeUnion(res, h1);
		}
		return res;
	}
}

/** @brief check for conflict in the history of current and provided events
 *
 * In the paper, this.isConflict(other) is written "this # other"
 */

bool UnfoldingEvent::isConflict(UnfoldingEvent* otherEvent) {
	UnfoldingEvent *e = new UnfoldingEvent(this->id, this->transition,
			this->causes);
	// event e should not conflict with itself
	if (*e == *otherEvent)
		return false;

	EventSet h1, h2;
	h1 = this->getHistory();
	h2 = otherEvent->getHistory();

	// checking causality relation, if they are in a causality relation return false
	if (h1.contains(otherEvent))
		return false;

	if (h2.contains(e))
		return false;

	// check direct conflict
	if (this->transition.isDependent(otherEvent->transition))
		return true;
	//  if 2 event they have the same causes, just check their last transition
	if (causes == otherEvent->causes)
		return this->transition.isDependent(otherEvent->transition);

	// if not, then check dependent relation on their full histories

	else {
		h1.insert(e);
		h2.insert(otherEvent);
		EventSet his = h1;
		//FIXME remove all common events
		for (auto evt : his.events_)
			if (h2.contains(evt)) {
				h1.erase(evt);
				h2.erase(evt);
			}
		return h1.depends(h2);

	}
	return false;
}

/** @brief Checks if current event is in immediate conflict with the provided one
 *
 * For that, there is two conditions to meet:
 *  - both events are in conflict (there is a conflict in their histories)
 *  -      Union(hist1,       hist2, evt2) is a valid configuration
 *    AND  Union(hist1, evt1, hist2)       is a valid configuration
 *
 * In the paper, e1.isImmediate(e2) will be written "e1 #ⁱ e2"
 */

bool UnfoldingEvent::isImmediateConflict(UnfoldingEvent *evt2) {

	// The first condition is easy to check
	if (not isConflict(evt2))
		return false;
	// Now, check the second condition
	EventSet hist1 = this->getHistory();
	EventSet hist2 = evt2->getHistory();

	// First compare the existing configurations
	for (auto e1 : hist1.events_)
		for (auto e2 : hist2.events_)
			if (e1->isConflict(e2))
				return false; // hist1 U hist2 is not a config => no immediate conflict
	// Compare the first config to the second new event
	for (auto e1 : hist1.events_)
		if (e1->isConflict(evt2))
			return false;
	// Compare the second config to the first new event
	for (auto e2 : hist2.events_)
		if (e2->isConflict(this))
			return false;

	// Every tests passed
	return true;
}

// checking conflict relation between one event and one configuration or one history, it used when computing enC
// there is a better way by checking the event with maximal events in the configuration, (change type of enC )
bool UnfoldingEvent::conflictWithConfig(Configuration config) {
	// TODO: we don't really need to check the whole config. The maximal event should be enough.
	for (auto evt : config.maxEvent.events_)
		if (this->isConflict(evt))
			return true;
	return false;
}

/*    void UnfoldingEvent::getEnabledTransition(std::set<Transition*>* whereto) {
 THROW_UNIMPLEMENTED
 ;
 // FIXME
 }*/

// this operator is used for ordering in a set (need a key)
bool UnfoldingEvent::operator<(const UnfoldingEvent& other) const {
	if ((this->transition.actor_id < other.transition.actor_id)
			or (this->transition.id < other.transition.id)
			or (not (this->causes == other.causes)))
		return true;
	return false;
}

/** @brief check semantic equality (same transition, same causes) */
bool UnfoldingEvent::operator==(const UnfoldingEvent& other) const {
	return ((this->transition.actor_id == other.transition.actor_id)
			&& (this->transition.id == other.transition.id)
			&& (this->causes == other.causes));

}

/*   void Configuration::getEnabledTransition(std::set<Transition*>* whereto) {
 for (auto e : maxEvent)
 e.getEnabledTransition(whereto);
 }*/

void Configuration::getEnabledTransition(std::set<Transition> whereto) {
}

void Configuration::updateMaxEvent(UnfoldingEvent *e) {
	this->lastEvent = e;
	// removing only the causes is enough

	for (auto evt : e->causes.events_)
		maxEvent.erase(evt);
	maxEvent.insert(e);
}

bool Transition::operator<(const Transition& other) const {

	return ((this->id < other.id) || (this->actor_id < other.actor_id));

}
bool Transition::operator==(const Transition& other) const {

	return ((this->id == other.id) && (this->actor_id == other.actor_id));

}
/* this function is used to generate events from a candidate history
 */

void Configuration::generateEvents(EventSet& result, Transition t,
		EventSet causuality_events, EventSet cause, EventSet candidateHistory) {
	if (candidateHistory.empty()) {

		nb_events++;
		EventSet cause1 = cause;
		//cause1.insert(preEvt);
		cause1 = cause1.makeUnion(cause1, causuality_events);
		UnfoldingEvent *e = new UnfoldingEvent(nb_events, t, cause1);
		result.insert(e);

		return;
	}

	else {
		UnfoldingEvent *a = candidateHistory.begin();
		EventSet evtSet1, evtSet2, evtSet3, evtSet4;
		evtSet1 = cause;
		evtSet3 = candidateHistory;
		evtSet1.insert(a);
		evtSet3.erase(a);
		generateEvents(result, t, causuality_events, evtSet1, evtSet3);
		evtSet2 = cause;
		evtSet4 = candidateHistory;
		evtSet4.erase(a);
		generateEvents(result, t, causuality_events, evtSet2, evtSet4);
	}

}

void UnfoldingChecker::computeAlt(EventSet& J, EventSet D, Configuration C,
		EventSet U1, EventSet Uc) {
	if (not J.empty())
		return;

	if (Uc.empty()) {
		EventSet C1 = C.makeUnion(C, U1);

		if (C1.isConfig()) {
			size_t count = 0;
			for (auto evt : D.events_)
				for (auto evt1 : C1.events_)
					if (evt->isImmediateConflict(evt1) and U.contains(evt1)) {

						count++;
						break;
					}

			if (count == D.size()) {
				J = U1;

			}
		}
		return;
	}

	else {
		UnfoldingEvent *a = Uc.begin();
		EventSet evtSet1, evtSet2, evtSet3, evtSet4;
		evtSet1 = U1;
		evtSet3 = Uc;
		evtSet1.insert(a);
		evtSet3.erase(a);
		computeAlt(J, D, C, evtSet1, evtSet3);

		evtSet2 = U1;
		evtSet4 = Uc;
		evtSet4.erase(a);

		computeAlt(J, D, C, evtSet2, evtSet4);
	}

}

/*bool UnfoldingEvent::checkMutexedTransition(int access_var) {
	int nbLock = 0, nbUnlock = 0;
	EventSet evtHisty = this->getHistory();
	for (auto evt : evtHisty.events_)
		if (evt->transition.access_var == access_var)
			if (evt->transition.read_write == 2)
				nbLock++;
			else if (evt->transition.read_write == 3)
				nbUnlock++;

	if (this->transition.access_var == access_var)
		if (this->transition.read_write == 2)
			nbLock++;
		else if (this->transition.read_write == 3)
			nbUnlock++;

	if (nbLock <= nbUnlock)
		return true;
	return false;
}*/

/* for each event in C, search all enabled transition in the state of that event
 then creating new event based on the transition and configuration C.maxEvent*/

void UnfoldingChecker::extend(std::set<Actor> actors, Configuration C,
		EventSet & exC, EventSet& enC) {

	// in the initial state each actor creates one event

	EventSet causes;
	if (C.empty()) {
		for (auto p : actors)
			if (p.trans[0].isDependingTransExecuted(actors)) {
				nb_events++;
				UnfoldingEvent *newEvent = new UnfoldingEvent(nb_events,
						p.trans[0], causes);

				if (not U.contains(newEvent))
					U.insert(newEvent);
				if (not enC.contains(newEvent))
					enC.insert(newEvent);
				if (not exC.contains(newEvent))
					exC.insert(newEvent);
			}

	} else {

		// get all enabled transitions at current appState

		std::set<Transition> enabledTransitions;
		enabledTransitions = C.lastEvent->appState.getEnabledTransition();

		// try to create new events from a transition and maximal events of C

		for (auto trans : enabledTransitions) {
			if (not trans.isDependent(C.lastEvent->transition))
				continue; // don't consider this transition

			EventSet exC1, hisCdt, causality_events, historyCandidate;
			// in maxEvent of C, we only consider events have transitions that are dependent with trans

			if (trans.read_write < 2) {
				for (auto evt : C.maxEvent.events_)
					if (trans.isDependent(evt->transition))
						historyCandidate.insert(evt);

				historyCandidate.erase(C.lastEvent);
				causality_events.insert(C.lastEvent);

				// compute causality events (events must happen to make new events are executable)
				// previous event in the same actor must be added to causality set

				for (auto evt : historyCandidate.events_)
					if (trans.actor_id == evt->transition.actor_id) {
						causality_events.insert(evt);
						historyCandidate.erase(evt);
						break;
					}

				/* generate new events from the enabled transition and historyCandidate
				 each subset of historyCandidate can be a history candidate*/
				C.generateEvents(exC1, trans, causality_events, hisCdt,
						historyCandidate);
			}

			// if trans is lock/unlock, add 2 (if possible) events to ex(C)
			else {

				bool chk = true;
				EventSet cause;
				for (auto evt : C.maxEvent.events_)
					if (trans.actor_id == evt->transition.actor_id) {
						// generate a new event that's causes includes only one event in the same actor

						cause.insert(evt);
						nb_events++;
						UnfoldingEvent *e = new UnfoldingEvent(nb_events, trans, cause);

						///std::cout << "\n them : ";
						//e->print();

						exC.insert(e);
						chk = false;

						//std::cout << "\n vào exC \n";
						break;
					}

				if (trans.read_write == 2) {

					if (chk and trans.access_var == C.lastEvent->transition.access_var
							and C.lastEvent->transition.read_write == 3) {
						EventSet cause1;
						cause1.insert(C.lastEvent);
						nb_events++;
						UnfoldingEvent *e = new UnfoldingEvent(nb_events, trans,
								cause1);
						exC.insert(e);
					}

					// try to generate a new event that's causes includes 2 events: event in the same actor and unlock event
					for (auto evt : C.maxEvent.events_)
						if (trans.access_var == evt->transition.access_var
								and evt->transition.read_write == 3
								and trans.actor_id
										!= evt->transition.actor_id) {
							nb_events++;
							cause.insert(evt);
							UnfoldingEvent *e = new UnfoldingEvent(nb_events,
									trans, cause);
							exC.insert(e);
							break;
						}
				}
			}

			// Extension of C = previous  extension union the events that are created from the maximal events
			exC = exC.makeUnion(exC, exC1);

		}

		for (auto evt : exC.events_) {
			if (not U.contains(evt))
				U.insert(evt);

			/* add new event evt to enC if evt's transition is not dependent with any transition of a event
			 which is in C and is not in history of evt */

			bool chk = true;
			EventSet evtHisty = evt->getHistory();

			for (auto evt1 : C.events_)
				if (not evtHisty.contains(evt1)
						and evt1->transition.isDependent(evt->transition)) {
					chk = false;
					break;
				}
			if (chk)
				enC.insert(evt);
			/*
			 if (not (evt->conflictWithConfig(C)) and not enC.contains(evt))
			 enC.insert(evt);*/
		}

	}
}

void UnfoldingChecker::explore(Configuration C, EventSet D, EventSet A,
		UnfoldingEvent* currentEvt, EventSet prev_exC, std::set<Actor> actors) {

	UnfoldingEvent *e;
	EventSet enC, exC = prev_exC;
	exC.erase(currentEvt);

	// exC = previous exC - currentEvt + new events

	extend(actors, C, exC, enC);

	if (enC.empty()) {

		std::cout << " exploring executions :\n";
		for (auto evt : C.events_) {
			std::cout << " --> ";
			evt->print();
		}
		std::cout << "\n\n";

		return;
	}

	if (A.empty())
		e = enC.begin();
	else
		// if A is not empty, chose one event in the intersection of A and enC
		for (auto evt : A.events_)
			if (enC.contains(evt)) {
				e = evt;
				break;
			}
	//this->getSession().execute(e->transition);
	//currentEvt.execute(e->transition);
	/*   e->appState = std::unique_ptr < simgrid::mc::State
	 > (new simgrid::mc::State(++expandedStatesCount_));
	 */

	State newState = currentEvt->appState.execute(e->transition);
	e->appState = newState;

	// UnfoldingEvent* newEvent = e + e.transition;
	// Config::plus returns a new config with the evt added to the maxEvent
	// Config::minus returns a new config with the evt removed from the set

	Configuration C1 = C.plus(e);
	C1.updateMaxEvent(e);
	explore(C1, D, A.minus(e), e, exC.minus(e), actors);
	EventSet J, U1;
	EventSet Uc = U;

	computeAlt(J, D.plus(e), C, U1, Uc);

	if (not J.empty()) {

		EventSet dif;
		std::set_difference(J.events_.begin(), J.events_.end(),
				C.events_.begin(), C.events_.end(),
				std::inserter(dif.events_, dif.events_.end()));

		explore(C, D.plus(e), dif, currentEvt, prev_exC.minus(e), actors);
	}

	remove(e, C, D);

}

void UnfoldingChecker::remove(UnfoldingEvent* e, Configuration C, EventSet D) {

	EventSet unionSet, res, res1;
	unionSet = unionSet.makeUnion(C, D);

	// building Qcdu
	for (auto e1 : U.events_)
		for (auto e2 : unionSet.events_) {
			if (e1->isImmediateConflict(e2))
				// add e1 which is immediate conflict with one event in C u D to res
				res.insert(e1);
			break;
		}

	for (auto e1 : res.events_) {
		EventSet h = e1->getHistory();
		res = res.makeUnion(res, h);
	}

	res = res.makeUnion(res, unionSet);
	// move e from U to G if the condition is satisfied

	if (not res.contains(e)) {
		U.erase(e);
		G.insert(e);
	}

	// move history of ê from U to G
	EventSet U1 = U;
	for (auto evt : U1.events_)
		if (evt->isImmediateConflict(e)) {

			EventSet h = evt->getHistory();
			h.insert(evt);

			for (auto e2 : h.events_)
				if (not res.contains(e2)) {

					U.erase(e2);
					G.insert(e2);
				}

		}

}

/*    Checker* createUnfoldingChecker(Session& session) {
 return new UnfoldingChecker(session);
 }*/
}
}

