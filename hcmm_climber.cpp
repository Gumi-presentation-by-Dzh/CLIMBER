#include <cstdlib>
#include <cstdio>
//#include <math.h>
#include<algorithm>
#include <ctime>
#include "hcmm_climber.h"
#include <cmath>
#include <limits>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
//#ifdef __cplusplus 
extern "C"{
//#endif
#define random(x) rand()%(x)
bool cmpclimber(lifenode a, lifenode b){
	return a.life<b.life;
};
bool cmpcounter(countnode a, countnode b){
    return a.life<b.life;
};
climberobj::climberobj(uint64_t areasize, uint64_t attacktype, int climberenable, int randomenable, uint64_t climbershift, uint64_t climberthreshold){
 	this->maxpagenums = areasize;
	this->climbethreshold = climberthreshold;
    this->climbershift = climbershift;
    this->climberenable = climberenable;
    this->randomenable = randomenable;
    this->randomshift = climbershift;
    this->areanums = this->maxpagenums >> 10;
    this->attacktype = attacktype;
    printf("maxpagenums:%lu\n",(this->maxpagenums));
    string randompath[2] = {"", "random_"};
    string climberpath[3] = {"", "climber_", "oclimber_"};
    string stallpath[2] = {"", "stall_"};
    sprintf(endlifepath,"type%lu_idealmm_%s%s%lu_%lu_wearrate_c.log",this->attacktype,climberpath[this->climberenable].c_str(),\
                   randompath[this->randomenable].c_str(),this->climbershift,climbethreshold);
    logfile = fopen(endlifepath,"w");
    sprintf(endlifepath,"type%lu_idealmm_%s%s%lu_%lu_endlife_c.dat",this->attacktype,climberpath[this->climberenable].c_str(),\
                   randompath[this->randomenable].c_str(),this->climbershift,climbethreshold);
    this->minlifetime = 10000000000;
    this->maxlifetime = 0.0;
    this->climbtop = 0;
    this->climbup = 0;
    this->climbmid = 0;
    this->climbdown = 0;

    printf("gen life distribution begin\n");
    double* x = (double*)malloc(this->maxpagenums*sizeof(double));
    char lifepath[20];
    string lifeline;
    uint64_t pagesize = this->maxpagenums / 1024 / 1024 * 4;
    sprintf(lifepath,"../life_%luG.dat",pagesize);
    printf("lifepath:%s\n",lifepath);
    ifstream in(lifepath);
    uint64_t randomkey =(uint64_t) random((int)(this->maxpagenums));
    for(uint64_t i=0;i<this->maxpagenums;i++){
       getline (in, lifeline);
       x[i] = atof(lifeline.c_str());
    }
    in.close();
    lifelist= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
    lifelist2= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
    lifenowlist = (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
    sortedlist = (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
     printf("gen life distribution end\n");
     for(uint64_t i = 0; i<this->maxpagenums; i++){
         if(this->minlifetime > x[i]){
             this->minlifetime = x[i];
         }
         if(this->maxlifetime < x[i]){
             this->maxlifetime = x[i];
         }
         this->lifelist[i].addr = i;
         this->lifelist[i].life = x[i];
         this->lifelist2[i].addr = i;
         this->lifelist2[i].life = x[i];
         this->sortedlist[i].addr = i;
         this->sortedlist[i].life = x[i];
     }
     printf("minlifetime =%f,maxlifetime =%f\n",(this->minlifetime),(this->maxlifetime));
     this->maplist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->reverselist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->sortednow = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->climbla2hot = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->climberlocthre= (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->climberstart = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
     this->maxSL = 0;
     this->start = 0; 
     printf("sort pages begin\n");
     sort(this->sortedlist,(this->sortedlist)+this->maxpagenums, cmpclimber);
     for( uint64_t i=0; i < this->maxpagenums; i++){
         this->sortednow[i] = this->sortedlist[i].addr;
         this->lifenowlist[i].addr = this->sortedlist[i].addr;
         this->lifenowlist[i].life = this->sortedlist[i].life;
         this->climbla2hot[this->sortedlist[i].addr] = i;
         this->climberlocthre[i] = (uint64_t)(climbethreshold*(this->lifelist2[i].life/this->minlifetime));
     }  
     printf("sort pages end\n");
     printf("map begin\n");
     for( uint64_t i=0; i < this->maxpagenums; i++){
         this->maplist[i] = i;
         this->reverselist[i] = i;
     }
     printf("map end\n");
     this->visitcount = (countnode*)malloc(this->maxpagenums * sizeof(countnode));
     for( uint64_t i=0; i < this->maxpagenums; i++){
         this->visitcount[i].addr = i;
         this->visitcount[i].life = 0;
     }
     this->totalcount = 0;
     this->remaptimes = 0;
     this->totaltime = 0;
     this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
     this->climbtime = 0;
     this->disclimbtime =0;
     this->climbup = 0;
     this->climbmid = 0;
     this->climbdown = 0;
    remapthreshold = 2000000;
    cyclethreshold = 20000000;    
    srand((int)(0));
     this->map2weakaddr = this->maxpagenums - 1;
     printf("genend\n");
};
uint64_t climberobj::getrandom(uint64_t start, uint64_t end){
  		return start+random(end-start+1);
  	};
int climberobj::climber(uint64_t addr_temp, uint64_t counterv){
        uint64_t addr = this->maplist[addr_temp];
        uint64_t localclimbethreshold = this->climberlocthre[addr];
        uint64_t maxup = 0;
        if ((counterv % (localclimbethreshold) == 0) and (this->climberenable >= 1)){
            uint64_t randommax = 1<<this->climbershift;
            uint64_t climberareaindex = this->climbla2hot[addr_temp] >> this->climbershift;
            uint64_t targetindex = climberareaindex;
            uint64_t randomaddr = 0;
            uint64_t targetaddr = 0;
            uint64_t hotrandomaddr = 0;
            if (this->climberpoint == (this->maxpagenums - 1) >> this->climbershift){
                maxup = this->maxpagenums;
            }
            else{
                maxup = (this->climberpoint + 1) <<  this->climbershift;
            }
            if (climberareaindex == this->climberpoint){
                hotrandomaddr = getrandom(1, (1<<this->climbershift) - 1);
                targetaddr = this->sortednow[(this->climbla2hot[addr_temp] + hotrandomaddr) % (1<<this->climbershift) + (maxup - (1<<this->climbershift))];
            }
            else{
                if (climberareaindex > this->climberpoint){
                    targetindex = this->climberpoint;
                }
                else{
                    targetindex = getrandom(climberareaindex + 1, this->climberpoint);
                }
                if (targetindex == this->climberpoint){
                    randommax = maxup - (targetindex << this->climbershift);
                }
                randomaddr = getrandom(0,(randommax)-1);
                targetaddr = this->sortednow[(targetindex<< this->climbershift) + randomaddr];
            }
            uint64_t tarla = this->reverselist[targetaddr];
            uint64_t targetcount = this->visitcount[tarla].life;
            //#if targetcount <= counterv:
            double wearratecompare = this->lifelist[targetaddr].life / this->lifelist2[targetaddr].life - this->lifelist[addr].life / this->lifelist2[addr].life;
            long long countercompare = targetcount - counterv;
            if ((wearratecompare > 0 && countercompare <= 0) ||(wearratecompare < 0 && countercompare > 0)){

                this->lifelist[addr].life = this->lifelist[addr].life - 1;
                if (this->lifelist[addr].life < 0){
                    return -1;
                }
                if (this->climberstart[addr] == 0 && this->maxSL < (this->climbla2hot[tarla] >> this->climbershift)){
                    this->maxSL = (this->climbla2hot[tarla] >> this->climbershift);
                }
                this->reverselist[targetaddr] = addr_temp;
                this->reverselist[addr] = tarla;
                this->maplist[addr_temp] = targetaddr;
                this->maplist[tarla] = addr;
                uint64_t swaptemp = this->climbla2hot[addr_temp];
                this->climbla2hot[addr_temp] = this->climbla2hot[tarla];
                this->climbla2hot[tarla] = swaptemp;
                this->climbtime = this->climbtime + 1;
                return 1;
            }
            this->disclimbtime = this->disclimbtime + 1;
        }
        return 0;
    };
void climberobj::halfclear(void){
        for(uint64_t i=0;i<this->maxpagenums; i++){
            this->visitcount[i].life = this->visitcount[i].life*3/4;
        }
};
int climberobj::access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite){
        uint64_t addr_temp =addr_temp1;
        this->totaltime = this->totaltime + 1;
        double maxwear = 0;
        double totalwear = 0;
        double wearnow = 0;
        if(this->totaltime % 100000000 == 0){
            for (uint64_t i = 0; i < this->maxpagenums; i++){
                wearnow = lifelist[i].life/lifelist2[i].life;
                totalwear += wearnow;
                if(maxwear < wearnow){
                    maxwear = wearnow;
                }
            }
            fprintf(logfile,"wearrate:%f,%f\n",maxwear,totalwear/this->maxpagenums);
        }
        this->visitcount[addr_temp].life = this->visitcount[addr_temp].life + 1;
        if (climber(addr_temp,this->visitcount[addr_temp].life) == -1){
            return -1;
        }
        uint64_t addr = this->maplist[addr_temp];
        this->totalcount = this->totalcount + 1;
        this->lifelist[addr].life = this->lifelist[addr].life - 1;
        if (this->lifelist[addr].life < 0){
            return -1;
        }
        if (this->totalcount == cyclethreshold){
            this->totalcount = 0;
        }
        if (this->totalcount == remapthreshold && this->climberenable == 1){
            this->start = 1;
            countnode*visitsortedlist = (countnode*)malloc(this->maxpagenums*sizeof(countnode));
            for(uint64_t i = 0 ; i< this->maxpagenums; i++){
                visitsortedlist[i].addr = this->visitcount[i].addr;
                visitsortedlist[i].life = this->visitcount[i].life;
            }
            sort(visitsortedlist,visitsortedlist+this->maxpagenums, cmpcounter);
            int doans = doswap(addr_temp,1,visitsortedlist);
            free(visitsortedlist);
            if (doans == -1){
                return -1;
            }
            return 1;
        }
        else if(this->totalcount == remapthreshold){
            this->climberpoint = this->climberpoint - 1;
            if (this->climberpoint < ((uint64_t)(((this->maxpagenums / 2) ))>>this->climbershift)){
                this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
            }
            this->halfclear();
            return 0;
        }
        else{
            return 0;
        }
};
int climberobj::doswap(uint64_t addr_temp, int isswap, countnode* visitsortedlist){
        this->maxSL = 0;
        this->climberpoint = this->climberpoint - 1;
        //this->rank2addrp = 0;
        if (this->climberpoint < ((uint64_t)(((this->maxpagenums / 2) ))>>this->climbershift)){
            this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
        }
        if (isswap != 0){
            if (this->randomenable == 1){
                this->randomkey = random((1<<this->randomshift)-1);
            }
            uint64_t addr = this->maplist[addr_temp];
            for( uint64_t i=0; i < this->maxpagenums; i++){
                lifenowlist[i].addr = this->lifelist[i].addr;
                lifenowlist[i].life = this->lifelist[i].life;
            }
            sort(this->lifenowlist,(this->lifenowlist)+this->maxpagenums, cmpclimber);
            for( uint64_t i=0; i < this->maxpagenums; i++){
                this->sortednow[i] = lifenowlist[i].addr;
            }
            double maxwearrate = 0.0;
            double wearrate = 0.0;
            double maxlife = 0;
            double maxlife2 = 0;
            uint64_t maxi = 0;
            for(uint64_t i = 0; i < this->maxpagenums; i++){
                wearrate = 1.0- (this->lifelist[i].life/this->lifelist2[i].life);
                if (maxwearrate < wearrate){
                    maxwearrate = wearrate;
                    maxlife = this->lifelist[i].life;
                    maxlife2 = this->lifelist2[i].life;
                    maxi = i;
                }
            }
            uint64_t zeropoint = 0;
            uint64_t index = 0;
            uint64_t vindex = 0;
            for(uint64_t i = 0;i < this->maxpagenums;i++){
                if (visitsortedlist[this->maxpagenums - 1 - i].life == 0 && zeropoint < this->maxpagenums - 1 - i)
                    zeropoint = (this->maxpagenums - 1 - i);
                if (isswap == 1){
                    index = this->maxpagenums - 1 - i;
                }
                else{
                    index = i;
                }
                if (this->maxpagenums - 1 - i > zeropoint){
                    vindex = this->maxpagenums - 1 - i;
                }
                else{
                    vindex = zeropoint - (this->maxpagenums - 1 - i);
                }
                if (this->maxpagenums - 1 - i == 0){
                    this->map2weakaddr = visitsortedlist[vindex].addr;
                }
                if (this->randomenable == 1 && index < ((((this->maxpagenums>>1))>>this->randomshift)<<this->randomshift)){
                    index = index ^ this->randomkey;
                }
                if (index < (1<<this->randomshift)){
                    this->climberstart[lifenowlist[index].addr] = 0;
                }
                else{
                    this->climberstart[lifenowlist[index].addr] = 1;
                }
                if (this->maplist[visitsortedlist[vindex].addr] != lifenowlist[index].addr && this->maplist[visitsortedlist[vindex].addr] != addr){
                    this->lifelist[this->maplist[visitsortedlist[vindex].addr]].life = this->lifelist[this->maplist[visitsortedlist[vindex].addr]].life - 1;
                    if (this->lifelist[this->maplist[visitsortedlist[vindex].addr]].life < 0){
                        return -1;
                    }
                }
                this->visitcount[visitsortedlist[vindex].addr].life = this->visitcount[visitsortedlist[vindex].addr].life*3/4;
                this->maplist[visitsortedlist[vindex].addr] = lifenowlist[index].addr;
                this->reverselist[lifenowlist[index].addr] = visitsortedlist[vindex].addr;
                this->climbla2hot[visitsortedlist[vindex].addr] = index;
            }
            this->remaptimes = this->remaptimes + 1;
            return 1;
        }
        return 0;
}
void climberobj::printstat(){
    FILE*lifefile = fopen(endlifepath,"w");
    for(uint64_t i = 0; i < this->maxpagenums; i++){
        fprintf(lifefile,"%lu,%f,%f\n",this->lifelist[i].addr, this->lifelist[i].life, this->lifelist2[i].life);
    }
    fprintf(logfile,"overhead:%lu,0,%lu\n",this->climbtime, this->disclimbtime);
    fclose(lifefile);
    fclose(logfile);
}

//#ifdef __cplusplus
}
 
//#endif
