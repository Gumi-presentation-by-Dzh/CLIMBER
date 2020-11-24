#include <cstdlib>
#include <cstdio>
#include<algorithm>
#include <ctime>
#include "bwlmm_climber.h"
#include <cmath>
#include <limits>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
extern "C"{
#define random(x) rand()%(x)
bool cmpclimber(lifenode a, lifenode b){
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
        hashkey = (uint64_t*)malloc(areasize*sizeof(uint64_t));
        hashkey[0] = 0;
        for(int i = 1; i< areasize;i++){
            hashkey[i] = random(areasize);
        }
        this->groupnums = uint64_t( areasize >> groupshift) + 1;
        this->counter = (uint64_t**)malloc(this->groupnums*sizeof(uint64_t*));
        for (uint64_t i = 0; i <this->groupnums ; i++){
            this->counter[i] = new uint64_t[counternums];
        }
        for(uint64_t i =0;i<this->groupnums;i++){
            for(uint64_t j = 0; j< counternums;j++)
               this->counter[i][j] = 0;
         }
        this->counternums = counternums;
        this->hotlist = (uint64_t***)malloc(this->groupnums*sizeof(uint64_t**));
        this->ishotlist = (bool***)malloc(this->groupnums*sizeof(bool**));
        for(uint64_t i = 0; i<this->groupnums;i++){
            this->hotlist[i] = (uint64_t**)malloc(3*sizeof(uint64_t*));
            this->ishotlist[i] = (bool**)malloc(3*sizeof(bool*));
            for (uint64_t j = 0; j<3;j++){
                this->hotlist[i][j] = new uint64_t[128];
                this->ishotlist[i][j] = new bool[128];
                for (uint64_t k = 0; k<128;k++){
                    this->ishotlist[i][j][k] = 0;
                    this->hotlist[i][j][k] = -1;
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
        clear();
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
        uint64_t count_groupindex = 0;
        for(uint64_t i = 0; i < hashnums; i++){
            hashvalue[i] = ((addr_temp^hashkey[addr_temp])%(1<<groupshift)+i) % counternums;
            count_groupindex = ((addr_temp^hashkey[addr_temp]) >> groupshift);
            if(isgetclimb == 0){
                isclimb = this->counter[count_groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[count_groupindex][hashvalue[i]])
                isclimb = this->counter[count_groupindex][hashvalue[i]];
        }
        return isclimb;
    }
    uint64_t bloomfilter::count(uint64_t addr_temp){
        uint64_t hashvalue[hashnums];
        uint64_t groupindex = (addr_temp >> groupshift);
        uint64_t isclimb = cyclethreshold;
        int isgetclimb = 0;
        uint64_t count_groupindex = 0;
        for(uint64_t i=0; i<hashnums;i++){
            hashvalue[i] = ((addr_temp^hashkey[addr_temp])%(1<<groupshift)+i) % counternums;
            count_groupindex = ((addr_temp^hashkey[addr_temp]) >> groupshift);
            this->counter[count_groupindex][hashvalue[i]] =  this->counter[count_groupindex][hashvalue[i]] + 1;
            if(isgetclimb == 0){
                isclimb = this->counter[count_groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[count_groupindex][hashvalue[i]]){
                isclimb = this->counter[count_groupindex][hashvalue[i]];
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
        uint64_t count_groupindex = 0;
        for(uint64_t i = 0; i < hashnums; i++){
            hashvalue[i] = ((addr^hashkey[addr])%(1<<groupshift)+i) % counternums;
            count_groupindex = ((addr^hashkey[addr]) >> groupshift);
            this->counter[count_groupindex][hashvalue[i]] =  this->counter[count_groupindex][hashvalue[i]] + 1;
            if (this->counter[count_groupindex][hashvalue[i]] >= hotthreshold){
                ishot = ishot + 1;
            }
            if(isgetclimb == 0){
                isclimb = this->counter[count_groupindex][hashvalue[i]];
                isgetclimb = 1;
            }
            else if (isclimb > this->counter[count_groupindex][hashvalue[i]]){
                isclimb = this->counter[count_groupindex][hashvalue[i]];
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


climberobj::climberobj(uint64_t areasize, uint64_t attacktype, int climberenable, int randomenable, uint64_t climbershift, uint64_t climberthreshold){
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
        this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
        this->climbtime = 0;
        this->disclimbtime =0;
        this->climbtop = 0;
        this->climbup = 0;
        this->climbmid = 0;
        this->climbdown = 0;
        this->climbered = (bool*)malloc(this->maxpagenums*sizeof(bool));
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
        counternums = 1024;
        hashnums = 4;
        hotthreshold = 1000;
        hotlistsize =list1size + list2size + list3size;       

        halfinterval = cyclethreshold;        

        isbreak = 0;
        double* x = (double*)malloc(this->maxpagenums*sizeof(double));
        //char lifepath[20];
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
        sort(this->sortedlist,(this->sortedlist)+this->maxpagenums, cmpclimber);
        printf("sort pages end\n");
        for( uint64_t i=0; i < this->maxpagenums; i++){
            this->climbla2hot[this->sortedlist[i].addr] = i;
            this->sortednow[i] = this->sortedlist[i].addr;
            this->climberlocthre[i] = (uint64_t)(climbethreshold*(this->lifelist2[i].life/this->minlifetime));
            //#this->climberlocthre[i] = climbethreshold
        }
        printf("map begin\n");
        for ( uint64_t i=0; i < this->maxpagenums; i++){
            this->maplist[i] = i;
            this->reverselist[i] = i;
            //this->countlist[i].addr = i;
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
        this->intoclimb = 0;
        printf("bloomfilter begin\n");
        this->bloomfilter1 = new bloomfilter(groupshift, this->maxpagenums, hashnums, counternums,list1size, list2size, list3size,halfinterval);
        printf("bloomfilter1 end\n");
        this->bloomfilter2 = new bloomfilter(groupshift, (this->maxpagenums >> groupshift), hashnums, counternums,list1size, list2size, list3size,halfinterval);
        printf("bloomfilter2 end\n");
        //getchar();
        
}
    }
    uint64_t climberobj::getrandom(uint64_t start, uint64_t end){
        return start+random(end-start+1);
    };
    int climberobj::climber(uint64_t addr_temp, uint64_t counterv){
        //printf("into climber\n");
        uint64_t addr = this->maplist[addr_temp];
        uint64_t localclimbethreshold = this->climberlocthre[addr];
        uint64_t maxup = 0;
        //printf("%lu\n",localclimbethreshold);
        if ((counterv/4 % (localclimbethreshold) == 0) && (this->climberenable >= 1)){
            climbered[addr_temp] = climbered[addr_temp]^1;
            //climbered[addr_temp] = 1;
            this->intoclimb++;
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
                //####new begin
                if (climberareaindex > this->climberpoint){
                    targetindex = this->climberpoint;
                }
                else{
                    targetindex = getrandom(climberareaindex + 1, this->climberpoint);
                }
                if (targetindex == this->climberpoint){
                    randommax = maxup - (targetindex << this->climbershift);
                }
                //###new end
                randomaddr = getrandom(0,(randommax)-1);
                targetaddr = this->sortednow[(targetindex<< this->climbershift) + randomaddr];
            }
            uint64_t tarla = this->reverselist[targetaddr];
            uint64_t targetcount = this->bloomfilter1->getcount(tarla);
            //#if targetcount <= counterv:
            double wearratecompare = this->lifelist[targetaddr].life / this->lifelist2[targetaddr].life - this->lifelist[addr].life / this->lifelist2[addr].life;
            long long countercompare = targetcount - counterv;
            if ((wearratecompare > 0 && countercompare <= 0) ||(wearratecompare < 0 && countercompare > 0)){

                this->lifelist[addr].life = this->lifelist[addr].life - 1;
                if (this->lifelist[addr].life < 0){
                    return -1;
                }
                //climber sl
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
                //printf("climb\n");
                return 1;
            }
            //printf("this->disclimbtime:%lu,%lu\n",counterv,localclimbethreshold);
            this->disclimbtime = this->disclimbtime + 1;
        }
        return 0;
    };
    int climberobj::access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite){
        this->totaltime = this->totaltime + 1;
        double maxwear = 0;
        double totalwear = 0;
        double wearnow = 0;
        if(this->totaltime % 10000000 == 0){
            for (uint64_t i = 0; i < this->maxpagenums; i++){
                wearnow = 1 - lifelist[i].life/lifelist2[i].life;
                totalwear += wearnow;
                if(maxwear < wearnow){
                    maxwear = wearnow;
                }
            }
            //printf("wearrate:%f,%f\n",maxwear,totalwear/this->maxpagenums);
            fprintf(logfile,"wearrate:%f,%f\n",maxwear,totalwear/this->maxpagenums);
        }
        //#addr = this->maplist[addr_temp]
        uint64_t addr_temp = addr_temp1;
        if (this->totalcount < remapthreshold){
            this->visitcount[addr_temp].life = this->visitcount[addr_temp].life + 1;
        }
        this->totalcount = this->totalcount + 1;
        uint64_t isclimb = this->bloomfilter1->access(addr_temp, hotthreshold,this->totalcount);
        this->bloomfilter2->access((addr_temp >> groupshift), hotthreshold,this->totalcount);
        if (this->climber(addr_temp,isclimb) == -1){
            return -1;
        }
        uint64_t addr = this->maplist[addr_temp];
        this->lifelist[addr].life = this->lifelist[addr].life - 1;
        if (this->lifelist[addr].life < 0){
            return -1;
        }
        if (this->totalcount == cyclethreshold){
            this->totalcount = 0;
        }
        if (this->totalcount == remapthreshold && this->climberenable == 1){
            this->start = 1;
            if(this->climberenable == 1){
                //printf("swap be\n");
                if(doswap(addr_temp,1)==-1){
                    return -1;
                }
                //printf("swap end\n");
            }
            return 1;
            //#print("remap end")
        }
        else{
            return 0;
        }
    }
    int climberobj::doswap(uint64_t addr_temp,int isswap){
        //#this->climberpoint = this->climberpoint - 1
        this->maxSL = 0;
        this->climberpoint = this->climberpoint - 1;
        //this->rank2addrp = 0;
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
            sort(this->lifenowlist,(this->lifenowlist)+this->maxpagenums, cmpclimber);
            for( uint64_t i=0; i < this->maxpagenums; i++){
                this->sortednow[i] = lifenowlist[i].addr;
            }
            //#print(lifenowlist[-1][0])
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
                                        //printf("bemapaddr:%lu\n",bemapaddr);
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
                                        //#if this->randomenable == 1 and index < (1<<this->randomshift):
                                        if (this->randomenable == 1 && index < (((int(this->maxpagenums/2))>>this->randomshift)<<this->randomshift)){
                                        //#if this->randomenable == 1 and index < (this->maxpagenums/2):
                                            index = index ^ this->randomkey;
                                        }
                                        if (this->maplist[bemapaddr] != lifenowlist[(index)].addr || this->maplist[bemapaddr] == addr){
                                            this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                                            if (this->lifelist[this->maplist[bemapaddr]].life < 0){
                                                return -1;
                                            }
                                        }

                                        this->maplist[bemapaddr] = lifenowlist[(index)].addr;
                                        this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                                        this->climbla2hot[bemapaddr] = (index);
                                        //#this->life2sorted[bemapaddr] = len(this->sortedlist) - 1 - mapindex
                                        mapindex = mapindex + 1 ;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ///printf("%lu\n",mapindex);
            for(uint64_t noindex2 = 0; noindex2 < this->bloomfilter2->areasize; noindex2++){
                //#print('noindex2：%d'%(noindex2))
                if (this->bloomfilter2->isaddr2hot[noindex2] == 0){
                    for(uint64_t listindex1 = 0; listindex1 < 3; listindex1++){
                        //#print('-1noindex2：%d'%(noindex2))
                        for(uint64_t noinde1 = 0; noinde1 < this->bloomfilter1->listsize[listindex1]; noinde1++){
                            if (this->bloomfilter1->ishotlist[noindex2][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - noinde1] != 0){
                                //#print('noinde1：%d'%(noinde1))
                                uint64_t bemapaddr = rank1[noindex2][listindex1][this->bloomfilter1->listsize[listindex1] - 1 - noinde1];
                                //printf("benap:%lu\n",bemapaddr);
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
                                //#if this->randomenable == 1 and index < (1<<this->randomshift):
                                if (this->randomenable == 1 && index < (((int(this->maxpagenums/2))>>this->randomshift)<<this->randomshift)){
                                //#if this->randomenable == 1 and index < (this->maxpagenums/2):
                                    index = index ^ this->randomkey;
                                }
                                if (this->maplist[bemapaddr] != lifenowlist[(index)].addr or this->maplist[bemapaddr] == addr){
                                    this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                                    if (this->lifelist[this->maplist[bemapaddr]].life < 0){
                                        return -1;
                                    }
                                }

                                this->maplist[bemapaddr] = lifenowlist[(index)].addr;
                                this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                                this->climbla2hot[bemapaddr] = (index);
                                //#this->life2sorted[bemapaddr] = len(this->sortedlist) - 1 - mapindex
                                mapindex = mapindex + 1 ;
                            }
                        }
                    }
                }
            }
            //#print(mapindex)
            //printf("%lu\n",mapindex);
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
                        index = index ^ this->randomkey;
                    }
                    if (this->maplist[bemapaddr] != lifenowlist[(index)].addr || this->maplist[bemapaddr] == addr){
                        this->lifelist[this->maplist[bemapaddr]].life = this->lifelist[this->maplist[bemapaddr]].life - 1;
                        if (this->lifelist[this->maplist[bemapaddr]].life < 0){
                            return -1;
                        }
                    }
                    this->maplist[bemapaddr] = lifenowlist[(index)].addr;
                    this->reverselist[lifenowlist[(index)].addr] = bemapaddr;
                    this->climbla2hot[bemapaddr] = (index);
                    //#this->life2sorted[bemapaddr] = len(this->sortedlist) - 1 - mapindex
                    mapindex = mapindex + 1 ;
                }
            }
            this->remaptimes = this->remaptimes + 1;
            this->bloomfilter1->clear();
            this->bloomfilter2->clear();
            return 1;
        }
        this->bloomfilter1->clear();
        this->bloomfilter2->clear();
        return 0;
    }
    void climberobj::printstat(){
        FILE*lifefile = fopen(this->endlifepath,"w");
        for(uint64_t i = 0; i < this->maxpagenums; i++){
            fprintf(lifefile,"%lu,%f,%f\n",this->lifelist[i].addr, this->lifelist[i].life, this->lifelist2[i].life);
        }
        fprintf(logfile,"overhead:%lu,0,%lu\n",this->climbtime, this->disclimbtime);
        fclose(lifefile);
        fclose(logfile);
    }
