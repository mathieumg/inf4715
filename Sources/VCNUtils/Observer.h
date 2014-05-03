// 
// By: David Blume
// Based on the pattern in the book "Design Patterns"
// 
// The Subject may pass messages of type T to the Observer's Update method.
//
// Classes may inherit from multiple instances of Subject and Observer,
// if they wish to communicate with different message types.
// (Ex., MouseMsg vs. KeyboardMsg.)
//
// The Monitored and Unmonitored policies are documented below.
//
// Source: http://observer.dlma.com/finale.html

#ifndef PATTERNS_OBSERVER_H
#define PATTERNS_OBSERVER_H

#include <list>
#include <algorithm>
#include <functional>

namespace patterns {

// Two forward declarations here
template < typename T, template <typename> class LifePolicy> class Subject;
template < typename T > class Monitored;

template < typename T, template <typename> class LifePolicy = Monitored>
class Observer {
public:
  virtual ~Observer() {  lifepolicy_.RemoveAll(this); }
  virtual void UpdateObserver(T& hint) = 0;
private:
  void AdviseAttach(Subject<T, LifePolicy> *p) { lifepolicy_.Add(p); }
  void AdviseDetach(Subject<T, LifePolicy> *p) { lifepolicy_.Remove(p); }
  LifePolicy<T> lifepolicy_;
  friend class Subject<T, LifePolicy>;
};


template < typename T, template <typename> class LifePolicy = Monitored >
class Subject {
public:
  virtual ~Subject() {
      std::for_each( observers_.begin(), 
          observers_.end(), 
          std::bind2nd(std::mem_fun( &Observer<T, LifePolicy>::AdviseDetach ), this));
      observers_.clear(); 
    }
  void Attach(Observer<T, LifePolicy> *o) { 
      if (std::find(observers_.begin(), observers_.end(), o) == observers_.end()) { 
        observers_.push_back(o); 
        o->AdviseAttach(this); 
      } 
    }
  void Detach(Observer<T, LifePolicy> *o) { observers_.remove(o); o->AdviseDetach(this); }
  void Notify(T& hint) {
    using namespace std::placeholders;

      std::for_each( observers_.begin(), 
          observers_.end(), 
          std::bind( std::mem_fun( &Observer<T, LifePolicy>::UpdateObserver ), std::placeholders::_1, hint) );
          //std::bind2nd(std::mem_fun( &Observer<T, LifePolicy>::Update ), *&hint) );
    }
private:
  std::list <Observer<T, LifePolicy> *> observers_;
};

//
// Policy: "Monitored"
//
// This policy keeps the lists of Observers and Subjects up-to-date
// regardless of the order of destruction of objects.  The descendant
// classes don't have to do a thing.
//
template < typename T >
class Monitored
{
public:
  Monitored() : detaching_(false) {};
  void RemoveAll(Observer<T, patterns::Monitored > *p) {
      detaching_ = true;
      std::for_each( subjects_.begin(), 
          subjects_.end(), 
          std::bind2nd(std::mem_fun( &Subject<T>::Detach ), p) );
      detaching_ = false;
    }
  void Add(Subject<T, patterns::Monitored> *p) {
      if (std::find(subjects_.begin(), subjects_.end(), p) == subjects_.end()) 
        subjects_.push_back(p); 
    }
  void Remove(Subject<T, patterns::Monitored> *p) { if (!detaching_) subjects_.remove(p); }
private:
  std::list<Subject<T, patterns::Monitored> *> subjects_;
  bool detaching_;
};


//
// Policy: "Unmonitored"
//
// The descendant classes have to manage their own logging of
// pointers to the other classes, and update them in the occurance
// of destruction.
//
template < typename T >
class Unmonitored
{
public:
  void RemoveAll(Observer<T, patterns::Unmonitored> *p) {}
  void Add(Subject<T, patterns::Unmonitored> *p) {}
  void Remove(Subject<T, patterns::Unmonitored> *p) {}
};

//
// Helper overloadable template functions follow, so that you may write:
// 
//  Notify(d1, m);
//
// over what you'd otherwise have to write, the fully-qualified member function:
//
//  d1->Subject<MouseMsg, POLICY>::Notify(m);
//
template<typename T, template <typename> class U>
void Attach(patterns::Subject<T, U> *subject, patterns::Observer<T, U> *observer)
{
  subject->Attach(observer);
}

template<typename T, template <typename> class U>
void Notify(patterns::Subject<T, U> *subject, T& hint )
{
  subject->Notify(hint);
}

}

#endif