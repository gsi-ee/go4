#ifndef LOCKGUARD_H
#define LOCKGUARD_H

// dummy definition for lockguard for designer plugin

//typedef int Qtrootlockguard;
// this one will give "unused" warnings, better use:
class Qtrootlockguard
{ 
public:
    Qtrootlockguard(): dummy(0){};
    
private: 
    int dummy;
    
};

#endif
