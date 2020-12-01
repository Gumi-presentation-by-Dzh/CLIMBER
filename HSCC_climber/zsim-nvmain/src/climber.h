
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
#ifndef __LIFENODE__
#define __LIFENODE__
typedef struct lifenode0{
	uint64_t addr;
	double life;
}lifenode;
typedef struct countnode0{
	uint64_t addr;
	uint64_t life;
}countnode;
#endif
class climberobj
{
  public:
  	    uint64_t maxpagenums = 0;
        uint64_t climbershift ;
        uint64_t climberenable ;
        uint64_t areanums ;
        unsigned attacktype;
        unsigned no = no;
        uint64_t* life2sorted;
        double* p;

        double minlifetime;
        double maxlifetime;
        lifenode* lifelist;
        lifenode* lifelist2;
        lifenode* sortedlist;
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
        uint64_t* ans;
        uint64_t disclimbtime;
        uint64_t climbtop;
        uint64_t climbup;
        uint64_t climbmid;
        uint64_t climbdown;
        uint64_t map2weakaddr;
    double  gaussrand(double mu, double sigma);
  	climberobj(uint64_t areasize);
  	uint64_t getrandom(uint64_t start, uint64_t end);
  	uint64_t* climber(uint64_t addr_temp, uint64_t counterv);
    void clear(void);
    uint64_t* access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite);
};
//#ifdef __cplusplus
}
 
//#endif
#endif