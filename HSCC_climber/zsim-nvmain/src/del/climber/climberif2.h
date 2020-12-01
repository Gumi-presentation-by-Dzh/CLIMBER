#include <cstdlib>
#include <cstdio>
#include "climber.h"
#include "Python.h"
class climberif{
  public:
    climberobj* climberp;
    climberif(uint64_t memorysize, const char* systype){
        climberp = new climberobj(memorysize);
    }
    uint64_t* access(uint64_t address,int iswrite){
        uint64_t arg1 = (address & 0xffffffff);
        uint64_t arg2 = ((address>>32) & 0xffffffff);

        uint64_t*ans = climberp->access(arg1,arg2,iswrite);
        //uint64_t* readdress = (uint64_t*)malloc(10*sizeof(uint64_t));
        uint64_t* returnvalue = (uint64_t*)malloc(10*sizeof(uint64_t));
        returnvalue[0] = ans[0];
        returnvalue[1] = (((uint64_t)ans[1])&0xffffffff)|((((uint64_t)ans[2])<<32)&0xffffffff00000000);
        returnvalue[2] = (((uint64_t)ans[3])&0xffffffff)|((((uint64_t)ans[4])<<32)&0xffffffff00000000);
        returnvalue[3] = (((uint64_t)ans[6])&0xffffffff)|((((uint64_t)ans[7])<<32)&0xffffffff00000000);
        returnvalue[4] = (((uint64_t)ans[8])&0xffffffff)|((((uint64_t)ans[9])<<32)&0xffffffff00000000);
        return returnvalue;
    }   
    ~climberif(){
        delete(climberp);
    }
};