#include <cstdlib>
#include <cstdio>
//#include <math.h>
#include<algorithm>
#include <ctime>
#include<stdint.h>
using namespace std;
#ifndef __CLIMBER__
#define __CLIMBER__
//#define random(x) rand()%(x)
//#ifdef __cplusplus  
extern "C"{
//#endif
typedef struct lifenode0{
	uint64_t addr;
	double life;
}lifenode;
typedef struct countnode0{
	uint64_t addr;
	uint64_t life;
}countnode;
class climberobj
{
  public:
  	    uint64_t maxpagenums;
        uint64_t climbershift ;
        uint64_t randomshift ;
        uint64_t climberenable ;
        uint64_t remapthreshold;
        uint64_t cyclethreshold;
        int randomenable;
        uint64_t randomkey;
        uint64_t areanums ;
        unsigned attacktype;
        unsigned no;
        uint64_t* life2sorted;
        double* p;

        double minlifetime;
        double maxlifetime;
        lifenode* lifelist;
        lifenode* lifelist2;
        lifenode* sortedlist;
        lifenode* lifenowlist;
        countnode* visitcount;

        uint64_t* maplist;
        uint64_t* reverselist;
        uint64_t* sortednow;
        uint64_t* climbla2hot;
        uint64_t* climberlocthre;
        uint64_t* climberstart;
        uint64_t climbethreshold;
        int maxSL;
        int start; 
        uint64_t totalcount;
        uint64_t remaptimes;
        uint64_t totaltime;
        uint64_t climberpoint;
        uint64_t climbtime;
        uint64_t disclimbtime;
        uint64_t climbtop;
        uint64_t climbup;
        uint64_t climbmid;
        uint64_t climbdown;
        char endlifepath[50];
        uint64_t map2weakaddr;
        FILE* logfile;
  	climberobj(uint64_t areasize, uint64_t attacktype, int climberenable, int randomenable, uint64_t climbershift, uint64_t climberthreshold);
  	uint64_t getrandom(uint64_t start, uint64_t end);
  	int climber(uint64_t addr_temp, uint64_t counterv);
    void halfclear(void);
    int access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite);
    void printstat();
    int doswap(uint64_t addr_temp, int isswap,countnode* visitsortedlist);
};
//#ifdef __cplusplus
}
 
//#endif
#endif