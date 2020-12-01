#include <cstdlib>
#include <cstdio>
#include<algorithm>
#include <ctime>
#include<stdint.h>
using namespace std;
#ifndef __BWL__
#define __BWL__
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
class bloomfilter{
public:
    uint64_t remapthreshold;
    uint64_t cyclethreshold;        

    uint64_t lineshift;
    uint64_t groupshift;
    uint64_t counternums;
    uint64_t hashnums;
    uint64_t hotthreshold;
    uint64_t list1size;
    uint64_t list2size;
    uint64_t list3size;
    uint64_t hotlistsize;         

    bool isbreak;
    //uint64_t *listsize;
    uint64_t halfinterval;
    uint64_t areasize;
    uint64_t groupnums;
    //print(this->groupnums)
    uint64_t** counter;
    uint64_t*** hotlist;
    bool*** ishotlist;
    //uint64_t* voidlist;
    uint64_t* list1point;
    uint64_t* list2point;
    uint64_t* list3point;
    uint64_t* addr2hot;
    uint64_t listsize[3];
    bool* isaddr2hot;
    bloomfilter(uint64_t groupshift, uint64_t areasize, uint64_t hashnums, uint64_t counternums,uint64_t list1size, uint64_t list2size, uint64_t list3size,uint64_t halfinterval);
    uint64_t*** rank();
    void clear();
    uint64_t getcount(uint64_t addr_temp);
    uint64_t count(uint64_t addr_temp);
    uint64_t access(uint64_t addr,uint64_t hotthreshold,uint64_t totaltime);
    FILE* logfile;
};  

class bwlobj
{
  public:
        uint64_t maxpagenums;
        uint64_t climbershift ;
        uint64_t randomshift ;
        uint64_t remapthreshold;
        uint64_t cyclethreshold;
        uint64_t climberenable ;
        uint64_t areanums ;
        unsigned attacktype;
        int isbreak;
        uint64_t randomkey;
        int randomenable;
        uint64_t* life2sorted;
        double* p;

        double minlifetime;
        double maxlifetime;

        uint64_t list1size;
        uint64_t list2size;
        uint64_t list3size;
        uint64_t hotlistsize;
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
        char lifepath[50];
        bloomfilter* bloomfilter1;
        bloomfilter* bloomfilter2;
        uint64_t* rank2addr;
        int* isswapaddr;
        uint64_t* returnaddr;
        uint64_t* ans;
        uint64_t returnp;
        uint64_t lineshift;
        uint64_t groupshift;
        uint64_t counternums;
        uint64_t hashnums;
        uint64_t hotthreshold;
        FILE* logfile;
        uint64_t halfinterval;        
        int swapaddrnums;
        uint64_t map2weakaddr;
    bwlobj(uint64_t areasize, uint64_t attacktype, int climberenable, int randomenable, uint64_t climbershift, uint64_t climberthreshold);
    uint64_t getrandom(uint64_t start, uint64_t end);
    int climber(uint64_t addr_temp, uint64_t counterv);
    void clear(void);
    uint64_t* doswap(uint64_t addr_temp, int isswap);
    uint64_t* access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite);
    void printstat();
};
//#ifdef __cplusplus
}
 
//#endif
#endif
