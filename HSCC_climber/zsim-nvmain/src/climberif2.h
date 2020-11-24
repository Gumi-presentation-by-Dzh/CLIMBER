#include <cstdlib>
#include <cstdio>
#include "climber.h"
#include "twl.h"
#include "bwl.h"
class climberif{
  public:
    climberobj* climberp;
    twlobj* twlp;
    bwlobj* bwlp;
    int climbtype; 
    uint64_t* returnvalue;
    uint64_t* bwlreturn;
    uint64_t*ans;
    climberif(uint64_t memorysize, int systype){
        if(systype == 2){
            twlp = new twlobj(memorysize);
        }
        else if(systype == 3){
            bwlp = new bwlobj(memorysize, 0, 0,0,10,10);
        }
        else{
            climberp = new climberobj(memorysize);
        }
        returnvalue = (uint64_t*)malloc(10*sizeof(uint64_t));
        climbtype = systype;
    }
    uint64_t* access(uint64_t address,int iswrite){
        uint64_t arg1 = (address & 0xffffffff);
        uint64_t arg2 = ((address>>32) & 0xffffffff);
        //uint64_t*ans = NULL;
        if (this->climbtype == 1){
            ans = climberp->access(arg1,arg2,iswrite);   
        }
        else if(this->climbtype == 2){
            ans = twlp->access(arg1,arg2,iswrite); 
        }
        else if(this->climbtype == 3){
            ans = bwlp->access(address,0,iswrite);
        }
        else{
            ans = climberp->access(arg1,arg2,iswrite);
        }  
        returnvalue[0] = ans[0];
        returnvalue[1] = (((uint64_t)ans[1])&0xffffffff)|((((uint64_t)ans[2])<<32)&0xffffffff00000000);
        returnvalue[2] = (((uint64_t)ans[3])&0xffffffff)|((((uint64_t)ans[4])<<32)&0xffffffff00000000);
        returnvalue[3] = (((uint64_t)ans[5])&0xffffffff)|((((uint64_t)ans[6])<<32)&0xffffffff00000000);
        returnvalue[4] = ans[7];
        returnvalue[5] = ans[8];
        returnvalue[6] = ans[9];
        return returnvalue;
    }   
    ~climberif(){
        delete(climberp);
    }
};
