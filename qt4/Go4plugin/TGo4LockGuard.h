#ifndef LOCKGUARD_H
#define LOCKGUARD_H

// dummy definition for lockguard for designer plugin

//typedef int TGo4LockGuard;
// this one will give "unused" warnings, better use:
class TGo4LockGuard
{ 
public:
    TGo4LockGuard(): dummy(0){};
    
private: 
    int dummy;
    
};

#endif
