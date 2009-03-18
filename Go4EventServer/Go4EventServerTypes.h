// master include file to get all go4 event class services

#ifndef GO4EVENTSERVERTYPES_H
#define GO4EVENTSERVERTYPES_H


/*
*    Enumeration specifying the id values for the standard event sources/ event stores.
*    To be checked in the event factories.
*/
enum
{
   GO4EV_NULL,                // no event store/source
   GO4EV_FILE,                // root file with own tree
   GO4EV_TREE,                // branch of singleton tree
   GO4EV_MBS_FILE,             // mbs listmode file (input only)
   GO4EV_MBS_STREAM,           // mbs stream server (input only)
   GO4EV_MBS_TRANSPORT,        // mbs transport server (input only)
   GO4EV_MBS_EVENTSERVER,      // mbs event server  (input only)
   GO4EV_MBS_REVSERV,           // remote event server (input only)
   GO4EV_BACK,            // backstore in memory (pseudo-ringbuffer?)
   GO4EV_USER,             // user defined source class
   GO4EV_MBS_RANDOM            // random generated mbs event
};


#endif //GO4EVENTSERVERTYPES_H

