#include <cstdlib>
#include <cstdio>
//#include <math.h>
#include<algorithm>
#include <ctime>
using namespace std;
#ifndef __CLIMBER__
#define __CLIMBER__
//#define random(x) rand()%(x)
//#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
extern "C"{
//#endif
typedef struct lifenode2{
	uint64_t addr;
	double life;
}lifenode;
class climberobj
{
  public:
  	    uint64_t maxpagenums = 0;
        uint64_t climbershift ;
        uint64_t climberenable ;
        uint64_t areanums ;//= maxpagenums >> 10;
        unsigned attacktype;
        //print("maxpagenums:%lu",maxpagenums);
        unsigned no = no;
        //uint64_t* life2sorted[maxpagenums];
        uint64_t* life2sorted;
        //uint64_t areanums;
        double* p;

        double minlifetime;
        double maxlifetime;
        //double* lifelist[2];
        //double* lifelist2[2];
        lifenode* lifelist;
        lifenode* lifelist2;
        lifenode* sortedlist;
        lifenode* visitcount;

        uint64_t* maplist;
        uint64_t* reverselist;////////climber
        uint64_t* sortednow;
        uint64_t* climbla2hot;//////climber
        uint64_t* climberlocthre;
        uint64_t* climberstart;
        uint64_t climbethreshold;
        int maxSL;
        int start; //climber start flag
        uint64_t totalcount;
        uint64_t remaptimes;
        uint64_t totaltime;////////循环内次数
        uint64_t climberpoint;
        uint64_t climbtime;
        uint64_t disclimbtime;
        uint64_t climbtop;
        uint64_t climbup;
        uint64_t climbmid;
        uint64_t climbdown;
        //uint64_t rank2addrp;
        //uint64_t* rank2addr;
        uint64_t map2weakaddr;
    double  gaussrand();
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