#ifndef _ASIF_THREADS_H_
#define _ASIF_THREADS_H_


class ASIF_Thread {
  public:
    //virtual ~ASIF_Thread();
    uint8_t threadId;
    virtual void setup();
    virtual boolean isTriggered();
    virtual void trigger();
};

#endif /* _ASIF_THREADS_H_ */
