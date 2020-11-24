#include<stdint.h>
#include"bwlmm_climber.h"
#include<cstdlib>
#include<cstdio>
#include<string>
#include <iostream>
#include <fstream>
using namespace std;
char tracepath[20];
uint64_t areashift = 0 ;
uint64_t maxpagenums = (4194304>>2);
bool isbreak = 0;
int attacktype = 0;
int attackpp = 0;
uint64_t endnums = 1000001000;
int endcycle = 1;
class AcListGenerator{
public:
    int type;
    uint64_t areasize;
    uint64_t pageshift;
    uint64_t filelength;
    uint64_t tracepoint;
    uint64_t climbershift;
    uint64_t climberthreshold;
    uint64_t visitcountnow;
    int cyclenow;
    climberobj* d1;
    uint64_t*visitlist;
    AcListGenerator(int type1, uint64_t maxpagenums, int attackpp, int climberenable, int randomenable,uint64_t climbershift,uint64_t climberthreshold){
        type = type1;
        areasize = maxpagenums;
        pageshift = 12;
        filelength = 0;
        tracepoint = 0;
        cyclenow = 0;
        visitcountnow = 0;
        uint64_t buffTmp;
        FILE* inFile;
        inFile = fopen(tracepath,"r");
        while (!feof(inFile)){
            fscanf(inFile,"%lu",&buffTmp);
            filelength += 1;
        }
        d1 = new climberobj(areasize, type, climberenable, randomenable, climbershift,climberthreshold);
        visitlist = (uint64_t *)malloc(filelength*sizeof(uint64_t));
        rewind(inFile);
        filelength = 0;
        while (!feof(inFile)){
            fscanf(inFile,"%lu",&visitlist[filelength++]);
        }
        fclose(inFile);
    }
    ~AcListGenerator(){
        delete(d1);
    }
    uint64_t getindex(int & memorystat){
        if (tracepoint >= filelength){
            cyclenow += 1;
            if (cyclenow >= endcycle){
                memorystat = -1;
            }
            tracepoint = 0;
        }
        visitcountnow += 1;
        uint64_t ans = visitlist[tracepoint] % areasize;
        tracepoint = tracepoint + 1;
        return ans;
    }
};
int main(int argc,char* argv[]){
    sprintf(tracepath,"trace.dat");
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
    }
    delete(g1);
    printf("end\n");
}

