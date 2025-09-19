#ifndef __DEVICE__
#define __DEVICE__

typedef int CODER_IN;

class encoder{
    private:
        int pinA_mum;
        int pinB_mum;
        int currentStateCLK;
        int lastStateCLK;
    public:
        encoder(CODER_IN pinA_num,CODER_IN pinB_num);
        int read();
};

#endif