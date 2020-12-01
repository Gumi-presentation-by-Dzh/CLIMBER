#include<stdint.h>
#include"bwlmm_climber.h"
#include<cstdlib>
#include<cstdio>
#include<string>
#include <iostream>
#include <fstream>
using namespace std;
uint64_t areashift = 0 ;
uint64_t maxpagenums = (4194304>>2);
bool isbreak = 0;
int attacktype = 1;
int attackpp = 0;
uint64_t endnums = 1000001000;
uint64_t climbershift = 17;
class AcListGenerator{
public:    
    int type;
    int attackpp;
    uint64_t areasize;
    uint64_t attackarea;
    bool* attackflag;
    bool flag;
    uint64_t index;
    uint64_t round;
    uint64_t count;
    uint64_t visitcountnow;
    uint64_t hot;
    climberobj*d1;
    uint64_t coldaddr[2];
    int initcycle;
    AcListGenerator(int type1, uint64_t areasize, int attackpp, int climberenable, int randomenable,uint64_t climbershift,uint64_t climberthreshold){
        if (areasize <= 2){
            printf("error:memorysize too small\n");
        }
        this->type = type1;
        this->attackpp = attackpp;
        this->areasize = areasize;
        this->attackarea = this->areasize;
        initcycle = 0;
        this->attackflag = (bool*)malloc(this->attackarea*sizeof(bool));
        this->flag = 0;
        this->index = 0;
        this->round = 0;
        this->count = 0;
        visitcountnow = 0;
        this->hot = 1000000;
        this->d1 = new climberobj(areasize, type, climberenable, randomenable, climbershift, climberthreshold);
        this->coldaddr[0] = areasize - 1;
        this->coldaddr[1] = areasize - 2;
    }
    ~AcListGenerator(){
        free(this->attackflag);
        delete(this->d1);
    }
    bool attackp(){
        return 1;
    }
    uint64_t getindex(int & memorystat){
        uint64_t ans = this->index;
        visitcountnow +=1;
        if(visitcountnow ==endnums){
            memorystat = -1;
        }
        if (this->flag == 0){
            if (this->index >= this->attackarea - 2){
                this->round = 1;
                this->index = 0;
            }
            else if (this->round == 0){
                this->index = this->index + 1;
            }
            else if (this->count < this->hot){
                this->count = this->count + 1;
                if (initcycle == 1){
                    ans = coldaddr[flag];
                }
            }
            else{
                this->count = 0;
                this->round = 0;
            }
        }
        else{
            if (this->index <= 1){
                this->round = 1;
                this->index = this->attackarea - 1;
            }
            else if (this->round == 0){
                this->index = this->index - 1;
            }
            else if (this->count < this->hot){
                this->count = this->count + 1;
                if (initcycle == 1){
                    ans = coldaddr[flag];
                }
            }
            else{
                this->count = 0;
                this->round = 0;
            }
        }
        return ans;
    }
    void dowhenswap(bool isswap){
        if (isswap == 1){
            this->round = 0;
            this->count = 0;
            this->initcycle = 1;
            this->flag = this->flag ^ 1;
        }
    }
};
int main(int argc,char* argv[]){
    AcListGenerator* g1 = NULL;
    if(argc == 1){
        g1 = new AcListGenerator(attacktype,maxpagenums,attackpp,0,0,17,10);
    }
    else if(argc == 2){
        g1 = new AcListGenerator(attacktype,maxpagenums,attackpp,atoi(argv[1]),0,17,10);
    }
    else if(argc == 3){
        g1 = new AcListGenerator(attacktype,maxpagenums,attackpp,atoi(argv[1]),atoi(argv[2]),17,10);
    }
    else if(argc == 4){
        g1 = new AcListGenerator(attacktype,maxpagenums,attackpp,atoi(argv[1]),atoi(argv[2]),(uint64_t)atoi(argv[3]),10);
    }
    else if(argc == 5){
        g1 = new AcListGenerator(attacktype,maxpagenums,attackpp,atoi(argv[1]),atoi(argv[2]),(uint64_t)atoi(argv[3]),(uint64_t)atoi(argv[4]));
    }
    uint64_t addr = 0;
    int memorystat = 0;
    while (isbreak == 0){
        addr = g1->getindex(memorystat);
        if (memorystat == -1){
            g1->d1->printstat();
            isbreak = 1;
            break;
        }
        memorystat = g1->d1->access(addr,0,1);
        if (memorystat == -1){
            g1->d1->printstat();
            isbreak = 1;
        }
        g1->dowhenswap(memorystat);
    }
    delete(g1);
    printf("end\n");
}
