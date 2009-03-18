#ifndef TGO4LOG_H
#define TGO4LOG_H

#include "TString.h"

class TMutex;

#define __MESSAGETEXTLENGTH__ 1024

// define previous trace macro void
#define TRACE(X) ;

// optional definitions
#define GO4MESSAGE( X ) TGo4Log::Message X;
#define GO4INFO(X) TGo4Log::Info X ;
#define GO4WARN(X) TGo4Log::Warn X ;
#define GO4ERROR(X) TGo4Log::Error X ;

/**
 * This class handles all logging messages inside Go4.
 * Should replace the previous Trace mechanism.
 * @author J. Adamczewski
 * @since 17-jun-2003
 */

class TGo4Log {
  public:
    static TGo4Log *Instance();

    /** Define threshold for output */
    static void SetIgnoreLevel(Int_t level);

    /** Get threshold for output */
    static Int_t GetIgnoreLevel();

    /** switch output on or off */
    static void OutputEnable(Bool_t on=kTRUE);

    /** get current output state */
    static Bool_t IsOutputEnabled();

    /** switch writing to logfile on or off */
    static void LogfileEnable(Bool_t on=kTRUE);

    /** get current logfile state */
    static Bool_t IsLogfileEnabled();

    /** set boolean for  user action */
    static void AutoEnable(Bool_t on=kTRUE);

    /** get current user boolean */
    static Bool_t IsAutoEnabled();

   /** Open file of name for logmessage output. If name=0, use
    *  defaultname containing pid. Optionally, headercomment text
    *  is set at beginning of file. If appendmode is true, existing
    *  file of same name is reused and extended by new messages.*/
    static void OpenLogfile(const char* name=0,
                            const char* headercomment=0,
                            Bool_t appendmode=kFALSE);

    /**  Close logfile if existing*/
    static void CloseLogfile();

    /**  get name of last opened logfile*/
    static const char* GetLogname();

  /** Display a message. Argument pri defines message priority:
    * 0: debug / trace output
    * 1: info message
    * 2: warning message
    * >=3: error message
    * Method returns formatted message string as printed out for further use */
   static const char* Message(Int_t prio, const char* text,...);

   /** User shortcut for message with prio 0 */
   static void Debug(const char* text,...);

   /** User shortcut for message with prio 1 */
   static void Info(const char* text,...);

   /** User shortcut for message with prio 2 */
   static void Warn(const char* text,...);

   /** User shortcut for message with prio 3 */
   static void Error(const char* text,...);


    /** Write text to current logfile if this is open. Prefix current
      datime in each line if "withtime" is true */
    static void WriteLogfile(const char* text,
                             Bool_t withtime=kTRUE);

    /** dummy for compatibility */
    static void set_trace_level(int) {}

    /** dummy for compatibility */
    static void SetTracePriority(int) {}

    /** For backward compatibility */
    static void StartTracing(){Instance(); SetIgnoreLevel(0);}

    /** For backward compatibility */
    static void StopTracing(){Instance(); SetIgnoreLevel(1);}

    /** Prompt character left side */
    static const char* fgcLEFT; //!

    /** Prompt character right  side */
    static const char* fgcRIGHT;//!

    /** System debug message indicator */
    static const char* fgcDEBUG; //!

    /** Info message indicator */
    static const char* fgcINFO; //!

    /** Warning message indicator */
    static const char* fgcWARN;//!

    /** Error message indicator */
    static const char* fgcERR; //!

    /** Name of default logfile */
    static const char* fgcDEFAULTLOG; //!

    /** Maximum message length allowd */
    enum { fguMESLEN = __MESSAGETEXTLENGTH__ };

    virtual ~TGo4Log();

  private:
    TGo4Log();

    static TGo4Log* fgxInstance; //!

    /** we use own mutex to protect the logging instance */
    static TMutex* fgxMutex; //!

    /** all Messages with lower level are suppressed */
    static Int_t fgiIgnoreLevel;  //!

    /** General switch on/off log output */
    static Bool_t fgbOutputEnabled;   //!

    /** General switch on/off log file output */
    static Bool_t fgbLogfileEnabled;   //!

    /** User boolean to write log on demand or not */
    static Bool_t fgbAutoMode;    //!

    /** Keeps latest message with all format indicators */
    static char fgcMessagetext[__MESSAGETEXTLENGTH__]; //!

    /** optional logfile to store messages with time */
    static void* fgxLogfile;   //!

    /** Name of last logfile set*/
    static TString fgxLogName;    //!

  ClassDef(TGo4Log,1)
};

/* for backward compatibility in user code: */
typedef TGo4Log TGo4Trace;

#endif //TGO4LOG_H
