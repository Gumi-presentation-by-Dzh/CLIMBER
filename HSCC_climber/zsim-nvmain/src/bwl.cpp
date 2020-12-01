#include <cstdlib>
#include <cstdio>
//#include <math.h>
#include<algorithm>
#include <ctime>
#include "bwl.h"
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
bool cmpclimber1(lifenode a, lifenode b){
    return a.life<b.life;
};
bloomfilter::bloomfilter(uint64_t groupshift, uint64_t areasize, uint64_t hashnums, uint64_t counternums,uint64_t list1size, uint64_t list2size, uint64_t list3size,uint64_t halfinterval){
        this->halfinterval = halfinterval;
        this->groupshift = groupshift;
        this->areasize = areasize;
        this->hashnums = hashnums;
        this->list1size = list1size;
        this->list2size = list2size;
        this->list3size = list3size;
        listsize[0] = list1size;
        listsize[1] = list2size;
        listsize[2] = list3size;
        hotlistsize = list1size+ list2size + list3size;
        this->groupnums = uint64_t( areasize >> groupshift) + 1;
        this->counter = (uint64_t**)malloc(this->groupnums*sizeof(uint64_t*));
        for (uint64_t i = 0; i <this->groupnums ; i++){
            this->counter[i] = new uint64_t[256];
        }
        this->counternums = counternums;
        this->hotlist = (uint64_t***)malloc(this->groupnums*sizeof(uint64_t**));
        this->ishotlist = (bool***)malloc(this->groupnums*sizeof(bool**));
        //printf("3\n");
        for(uint64_t i = 0; i<this->groupnums;i++){
            this->hotlist[i] = (uint64_t**)malloc(3*sizeof(uint64_t*));
            this->ishotlist[i] = (bool**)malloc(3*sizeof(bool*));
            for (uint64_t j = 0; j<3;j++){
                this->hotlist[i][j] = new uint64_t[128];
                this->ishotlist[i][j] = new bool[128];
                for (uint64_t k = 0; k<128;k++){
                    this->ishotlist[i][j][k] = 0;
                }
            }
        }
        this->list1point = (uint64_t*)malloc(this->groupnums*sizeof(uint64_t));
        this->list2point = (uint64_t*)malloc(this->groupnums*sizeof(uint64_t));
        this->list3point = (uint64_t*)malloc(this->groupnums*sizeof(uint64_t));
        this->addr2hot = (uint64_t*)malloc(this->areasize*sizeof(uint64_t));
        this->isaddr2hot = (bool*)malloc(this->areasize*sizeof(bool));
        for(uint64_t i = 0;i<this->groupnums;i++){
            this->list3point[i] = 0;
        }
        for(uint64_t i = 0;i<this->areasize;i++){
            this->addr2hot[i] = 0;
            this->isaddr2hot[i] = 0;
        }
        //printf("6\n");
    }
    uint64_t*** bloomfilter::rank(){
        return this->hotlist;
    }
    void bloomfilter::clear(){
        for(uint64_t i = 0; i < this->groupnums; i++){
            this->list3point[i] = 0;
            for(uint64_t j = 0; j < this->counternums; j++){
                this->counter[i][j] = 0;
            }
            for(uint64_t j = 0; j < 3; j++){
                for(uint64_t k = 0; k < 128; k++){
                    this->hotlist[i][j][k] = -1;
                    this->ishotlist[i][j][k] = 0;
                }
            }
        }
        for(uint64_t i = 0; i < this->areasize; i++){
            this->addr2hot[i] = 0;
            this->isaddr2hot[i] = 0;
        }
    }
    uint64_t bloomfilter::getcount(uint64_t addr_temp){
        uint64_t hashvalue[hashnums];
        uint64_t groupindex = (addr_temp >> groupshift);
        uint64_t isclimb = cyclethreshold;
        int isgetclimb = 0;
        for(uint64_t i = 0; i < hashnums; i++){
            hashvalue[i] = (addr_temp%(1<<groupshift)+i) % counternums;
            if(isgetclimb == 0){
                isclimb = this->counter[groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[groupindex][hashvalue[i]])
                isclimb = this->counter[groupindex][hashvalue[i]];
        }
        return isclimb;
    }
    uint64_t bloomfilter::count(uint64_t addr_temp){
        uint64_t hashvalue[hashnums];
        uint64_t groupindex = (addr_temp >> groupshift);
        uint64_t isclimb = cyclethreshold;
        int isgetclimb = 0;
        for(uint64_t i=0; i<hashnums;i++){
            hashvalue[i] = (addr_temp%(1<<groupshift)+i) % counternums;
            this->counter[groupindex][hashvalue[i]] =  this->counter[groupindex][hashvalue[i]] + 1;
            if(isgetclimb == 0){
                isclimb = this->counter[groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[groupindex][hashvalue[i]]){
                isclimb = this->counter[groupindex][hashvalue[i]];
            }
        }
        return isclimb;
    }
    uint64_t bloomfilter::access(uint64_t addr,uint64_t hotthreshold,uint64_t totaltime){
        uint64_t hashvalue[hashnums];
        uint64_t groupindex = (addr >> groupshift);
        int ishot = 0;
        uint64_t isclimb = cyclethreshold;
        bool inhot = 0;
        uint64_t addr_temp = addr;
        if (this->isaddr2hot[addr_temp] != 0){
            inhot = 1;
            if (this->addr2hot[addr_temp] < list3size){
                if (this->ishotlist[groupindex][1][0] != 0){
                    this->addr2hot[this->hotlist[groupindex][1][0]] = this->addr2hot[addr_temp];
                    this->isaddr2hot[hotlist[groupindex][1][0]] = this->isaddr2hot[addr_temp];
                }
                this->hotlist[groupindex][2][this->addr2hot[addr_temp]] = this->hotlist[groupindex][1][0];
                this->ishotlist[groupindex][2][this->addr2hot[addr_temp]] = this->ishotlist[groupindex][1][0];
                this->addr2hot[addr_temp] = list3size;
                this->isaddr2hot[addr_temp] = 1;
                this->hotlist[groupindex][1][0] = addr_temp;
                this->ishotlist[groupindex][1][0] = 1;
            }
            else if(this->addr2hot[addr_temp] < list3size + list2size - 1){
                if(this->ishotlist[groupindex][1][this->addr2hot[addr_temp] - list3size +1] != 0){
                    this->addr2hot[this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1]] = this->addr2hot[addr_temp];
                    this->isaddr2hot[this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1]] = this->isaddr2hot[addr_temp];
                }
                this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size] = this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1];
                this->ishotlist[groupindex][1][this->addr2hot[addr_temp] - list3size] = this->ishotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1];
                this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1] = addr_temp;
                this->ishotlist[groupindex][1][this->addr2hot[addr_temp] - list3size+1] = 1;
                this->addr2hot[addr_temp] = this->addr2hot[addr_temp] + 1;
                this->isaddr2hot[addr_temp] = 1;
            }
            else if (this->addr2hot[addr_temp] == list3size + list2size - 1){
                if (this->ishotlist[groupindex][0][0] != 0){
                    this->addr2hot[this->hotlist[groupindex][0][0]] = this->addr2hot[addr_temp];
                    this->isaddr2hot[this->hotlist[groupindex][0][0]] = this->isaddr2hot[addr_temp];
                }
                this->hotlist[groupindex][1][this->addr2hot[addr_temp] - list3size] = this->hotlist[groupindex][0][0];
                this->ishotlist[groupindex][1][this->addr2hot[addr_temp] - list3size] = this->ishotlist[groupindex][0][0];
                this->hotlist[groupindex][0][0] = addr_temp;
                this->ishotlist[groupindex][0][0] = 1;
                this->addr2hot[addr_temp] = list3size + list2size;
                this->isaddr2hot[addr_temp] = 1;
            }
            else if (this->addr2hot[addr_temp] < hotlistsize - 1){
                if (this->ishotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size +1] != 0){
                    this->addr2hot[this->hotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1]] = this->addr2hot[addr_temp];
                    this->isaddr2hot[this->hotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1]] = this->isaddr2hot[addr_temp];
                }
                this->hotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size] = this->hotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1];
                this->ishotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size] = this->ishotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1];
                this->hotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1] = addr_temp;
                this->ishotlist[groupindex][0][this->addr2hot[addr_temp]-list3size-list2size+1] = 1;
                this->isaddr2hot[addr_temp] = 1;
                this->addr2hot[addr_temp] = this->addr2hot[addr_temp] + 1;
            }
        }
        int isgetclimb = 0;
        for(uint64_t i = 0; i < hashnums; i++){
            hashvalue[i] = (addr%(1<<groupshift)+i) % counternums;
            this->counter[groupindex][hashvalue[i]] =  this->counter[groupindex][hashvalue[i]] + 1;
            if (this->counter[groupindex][hashvalue[i]] >= hotthreshold){
                ishot = ishot + 1;
            }
            if(isgetclimb == 0){
                isclimb = this->counter[groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[groupindex][hashvalue[i]]){
                isclimb = this->counter[groupindex][hashvalue[i]];
            }
        }
        if (ishot >= hashnums){
            if (inhot == 0){
                if (this->ishotlist[groupindex][2][this->list3point[groupindex]] != 0){
                    this->isaddr2hot[this->hotlist[groupindex][2][this->list3point[groupindex]]] = 0;
                }
                this->hotlist[groupindex][2][this->list3point[groupindex]] = addr;
                this->ishotlist[groupindex][2][this->list3point[groupindex]] = 1;
                this->addr2hot[addr] = this->list3point[groupindex];
                this->isaddr2hot[addr] = 1;
                this->list3point[groupindex] = (this->list3point[groupindex] + 1) % list3size;
            }
        }
        return isclimb;
    }


bwlobj::bwlobj(uint64_t areasize, uint64_t attacktype, int climberenable, int randomenable, uint64_t climbershift, uint64_t climberthreshold){
        this->climbethreshold = climberthreshold;
        this->maxpagenums = areasize;
        this->climbershift = climbershift;
        this->climberenable = climberenable;
        areanums = this->maxpagenums >> 10;
        printf("this->maxpagenums:%lu\n",this->maxpagenums);
        this->attacktype = attacktype;
        this->list1size = 64;
        this->list2size = 64;
        this->list3size = 128;
        swapaddrnums = 0;
        this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
        this->climbtime = 0;
        this->disclimbtime =0;
        this->climbtop = 0;
        this->climbup = 0;
        this->climbmid = 0;
        returnaddr = NULL;
        returnp = 0;
        this->climbdown = 0;
        srand((int)(0));
        //this->rank2addrp = 0;
        this->rank2addr = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->climberstart = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->maxSL = 0;
        this->map2weakaddr = this->maxpagenums - 1 ;
        remapthreshold = 2000000;
        cyclethreshold = 20000000;        

        lineshift = 6;
        groupshift = 12;
        counternums = 256;
        hashnums = 2;
        hotthreshold = 1000;
        hotlistsize =list1size + list2size + list3size;       

        halfinterval = cyclethreshold;        

        isbreak = 0;
        double* x = (double*)malloc(this->maxpagenums*sizeof(double));
        string lifeline;
        uint64_t pagesize = this->maxpagenums / 1024 / 1024 * 4;
        sprintf(lifepath,"../life_%luG.dat",pagesize);
        printf("lifepath:%s\n",lifepath);
        ifstream in(lifepath);
        uint64_t randomkey =(uint64_t) random((int)(this->maxpagenums));
        isswapaddr = (int*) malloc(this->maxpagenums*sizeof(int));
        for(uint64_t i=0;i<this->maxpagenums;i++){
           getline (in, lifeline);
           x[i] = atof(lifeline.c_str());
           isswapaddr[i] = 0;
        }
        in.close();
        
        printf("gen life distribution end\n");
        this->minlifetime = 10000000000;
        this->maxlifetime = 0.0;
        //this->no = no ;
        this->randomkey = 0;//  ###randmap;
        this->randomenable = randomenable;
        this->randomshift = climbershift;
        string randompath[2] = {"", "random_"};
        string climberpath[3] = {"", "climber_", "oclimber_"};
        sprintf(endlifepath,"type%lu_bwlmm_%s%s%lu_%lu_wearrate_c.log",this->attacktype,climberpath[this->climberenable].c_str(),\
                   randompath[this->randomenable].c_str(),this->climbershift,climbethreshold);
        logfile = fopen(endlifepath,"w");
        sprintf(endlifepath,"type%lu_bwlmm_%s%s%lu_%lu_endlife_c.dat",this->attacktype,climberpath[this->climberenable].c_str(),\
                   randompath[this->randomenable].c_str(),this->climbershift,climbethreshold);
        lifelist= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
        lifelist2= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
        sortedlist = (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
        lifenowlist = (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
        this->climbla2hot = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->start = 0;
        //#this->life2sorted = [0 for y in range(this->maxpagenums)]
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
            //this->life2sorted[i] = i;
            this->sortedlist[i].addr = i;
            this->sortedlist[i].life = x[i];
        }
        free(x);
        printf("minlifetime =%f,maxlifetime =%f\n",this->minlifetime,this->maxlifetime);
        this->maplist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->reverselist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->climberlocthre= (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->sortednow = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        //this->voidreturn = {{0,0}};
        printf("sort pages begin\n");
        sort(this->sortedlist,(this->sortedlist)+this->maxpagenums, cmpclimber1);
        printf("sort pages end\n");
        for( uint64_t i=0; i < this->maxpagenums; i++){
            this->climbla2hot[this->sortedlist[i].addr] = i;
            this->sortednow[i] = this->sortedlist[i].addr;
            this->climberlocthre[i] = (uint64_t)(climbethreshold*(this->lifelist2[i].life/this->minlifetime));
        }
        printf("map begin\n");
        for ( uint64_t i=0; i < this->maxpagenums; i++){
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
        printf("bloomfilter begin\n");
        this->bloomfilter1 = new bloomfilter(groupshift, this->maxpagenums, hashnums, counternums,list1size, list2size, list3size,halfinterval);
        printf("bloomfilter1 end\n");
        this->bloomfilter2 = new bloomfilter(groupshift, (this->maxpagenums >> groupshift), hashnums, counternums,list1size, list2size, list3size,halfinterval);
        printf("bloomfilter2 end\n");
        ans = (uint64_t*)malloc(10*sizeof(uint64_t));
        //getchar();
        
}
    }
    uint64_t bwlobj::getrandom(uint64_t start, uint64_t end){
        return start+random(end-start+1);
    };
    int bwlobj::climber(uint64_t addr_temp, uint64_t counterv){
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
            uint64_t targetcount = this->bloomfilter1->getcount(tarla);
            double wearratecompare = this->lifelist[targetaddr].life / this->lifelist2[targetaddr].life - this->lifelist[addr].life / this->lifelist2[addr].life;
            long long countercompare = targetcount - counterv;
            if ((wearratecompare > 0 && countercompare <= 0) ||(wearratecompare < 0 && countercompare > 0)){

                this->lifelist[addr].life = this->lifelist[addr].life - 1;
                if (this->climberstart[addr] == 0 && this->maxSL < (this->climbla2hot[tarla] >> this->climbershift)){
                    this->maxSL = (this->climbla2hot[tarla] >> this->climbershift);
                }
                //#####
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
    uint64_t * bwlobj::access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite){
        this->totaltime = this->totaltime + 1;
        double maxwear = 0;
        double totalwear = 0;
        double wearnow = 0;
        uint64_t addr_temp = addr_temp1;
        if (this->totalcount < remapthreshold){
            this->visitcount[addr_temp].life = this->visitcount[addr_temp].life + 1;
        }
        this->totalcount = this->totalcount + 1;
        uint64_t isclimb = this->bloomfilter1->access(addr_temp, hotthreshold,this->totalcount);
        this->bloomfilter2->access((addr_temp >> groupshift), hotthreshold,this->totalcount);
        this->climber(addr_temp,isclimb);
        uint64_t addr = this->maplist[addr_temp];
        this->lifelist[addr].life = this->lifelist[addr].life - 1;
        if (this->totalcount == cyclethreshold){
            this->totalcount = 0;
        }
        else if(this->totalcount == remapthreshold){
            if(returnaddr !=NULL){
                free(returnaddr);
            }
            doswap(addr_temp,1);
            swapaddrnums = 0;
            for(uint64_t i = 0; i< this->maxpagenums;i++){
                if (isswapaddr[i] == 1){
                    swapaddrnums += 1;
                }
            }
            returnaddr = (uint64_t*)malloc(2*(swapaddrnums+2)*sizeof(uint64_t));
            returnaddr[0] = 3;
            returnaddr[1] = swapaddrnums;
            swapaddrnums = 2;
            for(uint64_t i = 0; i< this->maxpagenums;i++){
                if (isswapaddr[i] == 1){
                    returnaddr[swapaddrnums++] = i;
                    isswapaddr[i] = 0;
                }
            }
            returnp = 0;
            swapaddrnums = 0;
            return returnaddr;
        }
        ans[8] == 0;
        ans[9] == 0;
        if(returnaddr!=NULL && returnp!=returnaddr[1]){
            ans[0] = 3;
            ans[1] = returnaddr[returnp+2] & 0xffffffff;
            ans[2] = (returnaddr[returnp+2] >>32) & 0xffffffff;
            returnp++;
        }
        else{
            ans[0] = 0;
            ans[1] = this->maplist[addr_temp] & 0xffffffff;
            ans[2] = (this->maplist[addr_temp] >>32) & 0xffffffff;
        }
        if(returnaddr!=NULL && returnp!=returnaddr[1]){
            ans[3] = returnaddr[returnp+2] & 0xffffffff;
            ans[4] = (returnaddr[returnp+2] >>32) & 0xffffffff;
            returnp++;
            ans[8] = 1;
        }
        if(returnaddr!=NULL && returnp!=returnaddr[1]){
            ans[5] = returnaddr[returnp+2] & 0xffffffff;
            ans[6] = (returnaddr[returnp+2] >>32) & 0xffffffff;
            returnp++;
            ans[9] = 1;
        }
        ans[7] = 0;
        return ans;
    }
    uint64_t* bwlobj::doswap(uint64_t addr_temp,int isswap){
        this->maxSL = 0;
        this->climberpoint = this->climberpoint - 1;
        if (this->climberpoint < ((uint64_t)(((this->maxpagenums / 2) ))>>this->climbershift)){
            this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
        }
        if (isswap != 0){
            uint64_t addr = this->maplist[addr_temp];
            if (this->randomenable == 1){
                this->randomkey = getrandom(0, (1<<this->randomshift)-1);
            }
            uint64_t mapindex = 0;
            uint64_t*** rank1 = this->bloomfilter1->rank();
            uint64_t*** rank2 = this->bloomfilter2->rank();
            for( uint64_t i=0; i < this->maxpagenums; i++){
                lifenowlist[i].addr = this->lifelist[i].addr;
                lifenowlist[i].life = this->lifelist[i].life;
            }
            sort(this->lifenowlist,(this->lifenowlist)+this->maxpagenums, cmpclimber1);
            for( uint64_t i=0; i < this->maxpagenums; i++){
                this->sortednow[i] = lifenowlist[i].addr;
            }
            double maxwearrate = 0.0;
            double wearrate = 0.0;
            double maxlife = 0;
            double maxlife2 = 0;
            uint64_t maxi = 0;
            for( uint64_t i=0; i < this->maxpagenums; i++){
                wearrate = 1.0- (this->lifelist[i].life/this->lifelist2[i].life);
                if (maxwearrate < wearrate){
                    maxwearrate = wearrate;
                    maxlife = this->lifelist[i].life;
                    maxlife2 = this->lifelist2[i].life;
                    maxi = i;
                }
            }
            for(uint64_t listindex1 = 0; listindex1 < 3; listindex1++){
                for(uint64_t gpindex2 = 0; gpindex2 < this->bloomfilter2->groupnums; gpindex2++){
                    for(uint64_t listindex2 = 0; listindex2 < 3; listindex2++){
                        for(uint64_t index2 = 0; index2< this->bloomfilter2->listsize[listindex2]; index2++){
                            if (this->bloomfilter2->ishotlist[gpindex2][listindex2][this->bloomfilter2->listsize[listindex2] - 1 - index2] != 0){
                                uint64_t gpindex1 = rank2[gpindex2][listindex2][this->bloomfilter2->listsize[listindex2] - 1 - index2];
                                //#for listindex1 in range (len(rank2[gpindex2])):
                                for(uint64_t index1 = 0; index1< this->bloomfilter1->listsize[listindex1]; index1++){
                                    if (this->bloomfilter1->ishotlist[gpindex1][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - index1] != 0){
                                        uint64_t bemapaddr = rank1[gpindex1][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - index1];
                                        uint64_t index = 0;
                                        if (isswap == 1){
                                            index = this->maxpagenums - 1 - mapindex;
                                        }
                                        else{
                                            index = mapindex;
                                        }
                                        if (index < (1<<this->randomshift)){
                                            this->climberstart[lifenowlist[index].addr] = 0;
                                        }
                                        else{
                                            this->climberstart[lifenowlist[index].addr] = 1;
                                        }
                                        this->rank2addr[bemapaddr] = this->maxpagenums - 1 - mapindex;
                                        if (this->randomenable == 1 && index < (((int(this->maxpagenums/2))>>this->randomshift)<<this->randomshift)){
                                            index = index ^ this->randomkey;
                                        }
                                        if (this->maplist[bemapaddr] != lifenowlist[(index)].addr || this->maplist[bemapaddr] == addr){
                                            this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                                            
                                        }
                                        isswapaddr[this->maplist[bemapaddr]] = 1;
                                        isswapaddr[lifenowlist[(index)].addr] = 1;
                                        this->maplist[bemapaddr] = lifenowlist[(index)].addr;

                                        this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                                        this->climbla2hot[bemapaddr] = (index);
                                        mapindex = mapindex + 1 ;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for(uint64_t noindex2 = 0; noindex2 < this->bloomfilter2->areasize; noindex2++){
                if (this->bloomfilter2->isaddr2hot[noindex2] == 0){
                    for(uint64_t listindex1 = 0; listindex1 < 3; listindex1++){
                        for(uint64_t noinde1 = 0; noinde1 < this->bloomfilter1->listsize[listindex1]; noinde1++){
                            if (this->bloomfilter1->ishotlist[noindex2][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - noinde1] != 0){
                                uint64_t bemapaddr = rank1[noindex2][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - noinde1];
                                uint64_t index = 0;
                                if (isswap == 1){
                                    index = this->maxpagenums - 1 - mapindex;
                                }
                                else{
                                    index = mapindex;
                                }
                                if (index < (1<<this->randomshift)){
                                    this->climberstart[lifenowlist[index].addr] = 0;
                                }
                                else{
                                    this->climberstart[lifenowlist[index].addr] = 1;
                                }
                                this->rank2addr[bemapaddr] = this->maxpagenums - 1 - mapindex;
                                if (this->randomenable == 1 && index < (((int(this->maxpagenums/2))>>this->randomshift)<<this->randomshift)){
                                    index = index ^ this->randomkey;
                                }
                                if (this->maplist[bemapaddr] != lifenowlist[(index)].addr or this->maplist[bemapaddr] == addr){
                                    this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                                    
                                }
                                isswapaddr[this->maplist[bemapaddr]] = 1;
                                isswapaddr[lifenowlist[(index)].addr] = 1;
                                this->maplist[bemapaddr] = lifenowlist[(index)].addr;
                                this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                                this->climbla2hot[bemapaddr] = (index);
                                mapindex = mapindex + 1 ;
                            }
                        }
                    }
                }
            }
            for(uint64_t index3 = 0; index3 < this->bloomfilter1->areasize; index3++){
                if (this->bloomfilter1->isaddr2hot[index3] == 0){
                    uint64_t bemapaddr = index3;
                    uint64_t index = 0;
                    if (isswap == 1){
                        index = this->maxpagenums - 1 - mapindex;
                    }
                    else{
                        index = mapindex;
                    }
                    if (index < (1<<this->randomshift)){
                        this->climberstart[lifenowlist[index].addr] = 0;
                    }
                    else{
                        this->climberstart[lifenowlist[index].addr] = 1;
                    }
                    this->rank2addr[bemapaddr] = this->maxpagenums - 1 - mapindex;
                    if (this->randomenable == 1 && index < ((((uint64_t)(this->maxpagenums/2))>>this->randomshift)<<this->randomshift)){
                    //#if this->randomenable == 1 and index < (this->maxpagenums/2):
                       index = index ^ this->randomkey;
                    }
                    if (this->maplist[bemapaddr] != lifenowlist[(index)].addr || this->maplist[bemapaddr] == addr){
                        this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                        
                    }
                    //#print(bemapaddr)
                    isswapaddr[this->maplist[bemapaddr]] = 1;
                    isswapaddr[lifenowlist[(index)].addr] = 1;
                    this->maplist[bemapaddr] = lifenowlist[(index)].addr;
                    this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                    this->climbla2hot[bemapaddr] = (index);
                    mapindex = mapindex + 1 ;
                }
            }
            this->remaptimes = this->remaptimes + 1;
            this->bloomfilter1->clear();
            this->bloomfilter2->clear();
            return NULL;
        }
        this->bloomfilter1->clear();
        this->bloomfilter2->clear();
        return NULL;
    }
    void bwlobj::printstat(){
        FILE*lifefile = fopen(this->endlifepath,"w");
        for(uint64_t i = 0; i < this->maxpagenums; i++){
            fprintf(lifefile,"%lu,%f,%f\n",this->lifelist[i].addr, this->lifelist[i].life, this->lifelist2[i].life);
        }
        fprintf(logfile,"overhead:%lu,0,%lu\n",this->remaptimes, this->disclimbtime);
        fclose(lifefile);
        fclose(logfile);
    }
