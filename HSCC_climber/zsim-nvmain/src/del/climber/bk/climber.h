#include <cstdlib>
#include <cstdio>
#include <math.h>
#include<algorithm>
#include <ctime>
using namespace std;
#ifndef __CLIMBER__
#define __CLIMBER__
#define random(x) rand()%(x)
typedef struct lifenode2{
	uint64_t addr;
	double life;
}lifenode;
bool cmpclimber(lifenode a, lifenode b){
	return a.life<b.life;
};
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
    double  gaussrand()
    {
        static  double  V1, V2, S;
        static  int  phase = 0;
        double  X;
        srand((int)time(0));
        if  ( phase == 0 ) {
            do{
                double  U1 = ( double ) rand () / RAND_MAX;
                double  U2 = ( double ) rand () / RAND_MAX;
                V1 = 2 * U1 - 1;
                V2 = 2 * U2 - 1;
                S = V1 * V1 + V2 * V2;
             }while (S >= 1 || S == 0);
             
            X = V1 *  sqrt (-2 *  log (S) / S);
        }else
            X = V2 *  sqrt (-2 *  log (S) / S);      
        phase = 1 - phase;
        return  X;
    };
  	climberobj(uint64_t areasize){
  		this->maxpagenums = areasize;
  		climbethreshold = 10;
        this->climbershift = 17;
        this->climberenable = 2;
        this->areanums = this->maxpagenums >> 10;
        this->attacktype = 0;
        printf("maxpagenums:%lu\n",(this->maxpagenums));
        this->minlifetime = 10000000000;
        this->maxlifetime = 0.0;
        this->climbtop = 0;
        this->climbup = 0;
        this->climbmid = 0;
        this->climbdown = 0;
        //this->reverseenable = reverseenable
        //this->stallenable = stallenable
        this->life2sorted = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));

        //np.random.seed(0)
        printf("gen life distribution begin");
        p = (double*)malloc(2*areanums*sizeof(double));
        for(uint64_t i=0;i<2*areanums;i++){
        	p[i] = gaussrand();
        }
        //p = np.random.normal(loc = mu, scale = sigma, size = 2*areanums)
        sort(p,p+2*areanums,less<double>());

        double x[this->maxpagenums];
        for(uint64_t i=0;i<this->maxpagenums;i++){
            x[i] = pow(p[areanums+(i>>10) - 1],-12)*90.345;
        }
        lifelist= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
        lifelist2= (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));
            //for j in range(1<<10):
            //x[(ps[areanums+i][0]<<10)+j] = math.pow(ps[areanums+i][1],-12)*90.345
        //x = np.random.normal(loc = 100000000.0, scale = 100000000*0.11, size = this->maxpagenums)
        //x = np.linspace(3000000,170000000,this->maxpagenums)
        printf("gen life distribution end");
        for(uint64_t i = 0; i<this->maxpagenums; i++){
            if(this->minlifetime > x[i]){
                this->minlifetime = x[i];
            }
            if(this->maxlifetime < x[i]){
                this->maxlifetime = x[i];
            }
            this->lifelist[i].addr = i;
            this->lifelist[i].life = x[i];//////页面i寿命为x[i]
            this->lifelist2[i].addr = i;
            this->lifelist2[i].life = x[i];//////页面i寿命为x[i]
            this->life2sorted[i] = i;
            this->sortedlist[i].addr = i;
            this->sortedlist[i].life = x[i];
        }
        printf("minlifetime =%f,maxlifetime =%f\n",(this->minlifetime),(this->maxlifetime));
        this->maplist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->reverselist = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));////////climber
        this->sortednow = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->climbla2hot = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));//////climber
        this->climberlocthre= (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->climberstart = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->maxSL = 0;
        this->start = 0; //climber start flag
        printf("sort pages begin\n");
        sort(this->sortedlist,(this->sortedlist)+this->maxpagenums, cmpclimber);////////按照寿命排序后，进行配对，配对公式：
        for( uint64_t i=0; i < this->maxpagenums; i++){
            this->life2sorted[this->sortedlist[i].addr] = i;
            this->sortednow[i] = this->sortedlist[i].addr;
            this->climbla2hot[this->sortedlist[i].addr] = i;
            this->climberlocthre[i] = (uint64_t)(climbethreshold*(this->lifelist2[i].life/this->minlifetime));
        }    //this->climberlocthre[i] = climbethreshold
        printf("sort pages end\n");
        printf("map begin\n");
        for( uint64_t i=0; i < this->maxpagenums; i++){
            this->maplist[i] = i;
            this->reverselist[i] = i;////////climber
        }
        printf("map end\n");
        this->visitcount = (lifenode*)malloc(this->maxpagenums * sizeof(lifenode));//////////////////////记录每个页从上次交换起被访问的次数
        for( uint64_t i=0; i < this->maxpagenums; i++){
            this->visitcount[i].addr = i;
            this->visitcount[i].life = 0;
        }
        this->totalcount = 0;
        this->remaptimes = 0;
        this->totaltime = 0;////////循环内次数
        this->climberpoint = (this->maxpagenums - 1) >> this->climbershift;
        this->climbtime = 0;
        this->disclimbtime =0;
        this->climbup = 0;
        this->climbmid = 0;
        this->climbdown = 0;
        //this->rank2addrp = 0;
        //this->rank2addr = (uint64_t*)malloc(this->maxpagenums*sizeof(uint64_t));
        this->map2weakaddr = this->maxpagenums - 1;
  	};
  	uint64_t getrandom(uint64_t start, uint64_t end){
  		srand((int)time(0));
  		return start+random(end-start+1);
  	};
  	uint64_t* climber(uint64_t addr_temp, uint64_t counterv){
        //if counterv % climbethreshold == 0 and this->start == 1:
        uint64_t addr = this->maplist[addr_temp];
        uint64_t* ans = (uint64_t*) malloc(10*sizeof(uint64_t));
        ans[0] = 0;
        ans[1] = addr & 0xffffffff;
        ans[2] = (addr >>32) & 0xffffffff;
        uint64_t localclimbethreshold = this->climberlocthre[addr];
        //uint64_t maxup = 0;
        //uint64_t* ans = (uint64_t*) malloc(10*sizeof(uint64_t));
        if ((counterv % (localclimbethreshold) == 0) and (this->climberenable >= 1)){
        	
            //step = int(counterv / localclimbethreshold)
            //uint64_t randommax = 1<<this->climbershift;
            uint64_t climberareaindex = this->climbla2hot[addr_temp] >> this->climbershift;
            uint64_t indexmax = (this->maxpagenums - 1) >> this->climbershift;
            //uint64_t targetindex = climberareaindex;
            uint64_t targetindex1 = -1;
            uint64_t wearratecompare1 = -1;
            uint64_t hotrandomaddr1;
            uint64_t targetaddr1;
            uint64_t tarla1;
            uint64_t targetcount1;
            uint64_t countercompare1;

            uint64_t targetindex2 = -1;
            uint64_t wearratecompare2 = -1;
            uint64_t hotrandomaddr2;
            uint64_t targetaddr2;
            uint64_t tarla2;
            uint64_t targetcount2;
            uint64_t countercompare2;

            uint64_t targetindex3 = -1;
            uint64_t wearratecompare3 = -1;
            uint64_t hotrandomaddr3;
            uint64_t targetaddr3;
            uint64_t tarla3;
            uint64_t targetcount3;
            uint64_t countercompare3;

            bool targetbool0 = 0;
            bool targetbool1 = 0;
            bool targetbool2 = 0;
            bool targetbool3 = 0;

            bool tarlabool0 = 0;
            bool tarlabool1 = 0;
            bool tarlabool2 = 0;
            bool tarlabool3 = 0;

            uint64_t targetindex0 = indexmax;
            targetbool0 = 1;
            uint64_t hotrandomaddr0 = getrandom(1, (1<<this->climbershift) - 1);
            uint64_t targetaddr0 = this->sortedlist[(targetindex0 << this->climbershift) + hotrandomaddr0].addr;
            uint64_t tarla0 = this->reverselist[targetaddr0];
            uint64_t targetcount0 = this->visitcount[tarla0].life;
            uint64_t wearratecompare0 = this->lifelist[targetaddr0].life / this->lifelist2[targetaddr0].life - this->lifelist[addr].life / this->lifelist2[addr].life;
            uint64_t countercompare0 = targetcount0 - counterv;

            if (climberareaindex == indexmax){// ###当前位置+1
                targetindex1 = -1;
                wearratecompare1 = 0;
            }
            else{
            	targetbool1 = 1;
                if (climberareaindex == indexmax - 1){
                    targetindex1 = indexmax;
                }
                else{
                    targetindex1 = getrandom(climberareaindex + 1, this->climberpoint);
                }
                hotrandomaddr1 = getrandom(1, (1<<this->climbershift) - 1);
                targetaddr1 = (targetindex1 << this->climbershift) + hotrandomaddr1;
                tarla1 = this->reverselist[targetaddr1];
                targetcount1 = this->visitcount[tarla1].life;
                wearratecompare1 = this->lifelist[targetaddr1].life / this->lifelist2[targetaddr1].life - this->lifelist[addr].life / this->lifelist2[addr].life;
                countercompare1 = targetcount1 - counterv;
            }
            targetindex2 = climberareaindex;
            targetbool2 = 1;
            hotrandomaddr2 = getrandom(1, (1<<this->climbershift) - 1);
            targetaddr2 = this->sortednow[(this->climbla2hot[addr_temp] + hotrandomaddr2) % (1<<this->climbershift) + (targetindex2<<this->climbershift)];
            tarla2 = this->reverselist[targetaddr2];
            targetcount2 = this->visitcount[tarla2].life;
            wearratecompare2 = this->lifelist[targetaddr2].life / this->lifelist2[targetaddr2].life - this->lifelist[addr].life / this->lifelist2[addr].life;
            countercompare2 = targetcount2 - counterv;
            if (climberareaindex == 0){ //###当前位置+1
                targetindex3 = -1;
                wearratecompare3 = 0;
            }
            else{
            	targetbool3 = 1;
                if (climberareaindex == 1){
                    targetindex3 = 0;
                }
                else{
                    targetindex3 = getrandom(0, climberareaindex - 1);
                }
                hotrandomaddr3 = getrandom(1, (1<<this->climbershift) - 1);
                targetaddr3 = (targetindex3 << this->climbershift) + hotrandomaddr3;

                tarla3 = this->reverselist[targetaddr3];
                targetcount3 = this->visitcount[tarla3].life;
                wearratecompare3 = this->lifelist[targetaddr3].life / this->lifelist2[targetaddr3].life - this->lifelist[addr].life / this->lifelist2[addr].life;
                countercompare3 = targetcount3 - counterv;
            }
            uint64_t tarla = -1;
            bool tarbool = 0;
            uint64_t targetaddr = -1;
            uint64_t tarla_temp0 = -1;
            uint64_t targetaddr_temp0 = -1;
            uint64_t tarla_temp1 = -1;
            uint64_t targetaddr_temp1 = -1;
            uint64_t tarla_temp2 = -1;
            uint64_t targetaddr_temp2 = -1;
            uint64_t tarla_temp3 = -1;
            uint64_t targetaddr_temp3 = -1;

            if (targetbool0 == 1 and ((wearratecompare0 > 0 and countercompare0 <= 0) or(wearratecompare0 < 0 and countercompare0 > 0))){
                tarla_temp0 = tarla0;
                tarlabool0 = 1;
                targetaddr_temp0 = targetaddr0;
            }
            if (targetbool1 == 1 and ((wearratecompare1 > 0 and countercompare1 <= 0) or(wearratecompare1 < 0 and countercompare1 > 0))){
                tarla_temp1 = tarla1;
                tarlabool1 = 1;
                targetaddr_temp1 = targetaddr1;
            }
            if (targetbool2 ==1 and ((wearratecompare2 > 0 and countercompare2 <= 0) or(wearratecompare2 < 0 and countercompare2 > 0))){
                tarla_temp2 = tarla2;
                tarlabool2 = 1;
                targetaddr_temp2 = targetaddr2;
            }
            if (targetbool3 == 1 and ((wearratecompare3 > 0 and countercompare3 <= 0) or(wearratecompare3 < 0 and countercompare3 > 0))){
                tarla_temp3 = tarla3;
                tarlabool3 = 1;
                targetaddr_temp3 = targetaddr3;
            }

            if (tarlabool0 == 1 and (wearratecompare0 >= wearratecompare1 or tarlabool1 == 0) and (wearratecompare0 >= wearratecompare2 or tarlabool2 == 0) and (wearratecompare0 >= wearratecompare3 or tarlabool3 == 0)){
                tarla = tarla_temp0;
                tarbool = 1;
                targetaddr = targetaddr_temp0;
                this->climbtop = this->climbtop + 1;
            }
            else if (tarlabool1 == 1 and (wearratecompare1 >= wearratecompare2 or tarlabool2 == 0) and (wearratecompare1 >= wearratecompare3 or tarlabool3 == 0) and (wearratecompare1 >= wearratecompare0 or tarlabool0 == 0)){
                tarla = tarla_temp1;
                tarbool = 1;
                targetaddr = targetaddr_temp1;
                this->climbup = this->climbup + 1;
            }
            else if (tarlabool2 == 1 and (wearratecompare2 >= wearratecompare1 or tarlabool1 == 0) and (wearratecompare2 >= wearratecompare3 or tarlabool3 == 0) and (wearratecompare2 >= wearratecompare0 or tarlabool0 == 0)){
                tarla = tarla_temp2;
                tarbool = 1;
                targetaddr = targetaddr_temp2;
                this->climbmid = this->climbmid + 1;
            }
            else if (tarlabool3 == 1 and (wearratecompare3 >= wearratecompare1 or tarlabool1 == 0) and (wearratecompare3 >= wearratecompare2 or tarlabool2 == 0) and (wearratecompare3 >= wearratecompare0 or tarlabool0 == 0)){
                tarla = tarla_temp3;
                tarbool = 1;
                targetaddr = targetaddr_temp3;
                this->climbdown = this->climbdown + 1;
             }

            if (tarbool == 1){
                this->lifelist[addr].life = this->lifelist[addr].life - 1;
                ans[0] = 1;
                ans[3] = ans[1];
                ans[4] = ans[2];
                ans[1] = targetaddr & 0xffffffff;
                ans[2] = (targetaddr >>32) & 0xffffffff;
                //if (this->lifelist[addr].life < 0){
                //    return -1;
                //}
                this->reverselist[targetaddr] = addr_temp;
                this->reverselist[addr] = tarla;
                this->maplist[addr_temp] = targetaddr;
                this->maplist[tarla] = addr;
                uint64_t swaptemp = this->climbla2hot[addr_temp];
                this->climbla2hot[addr_temp] = this->climbla2hot[tarla];
                this->climbla2hot[tarla] = swaptemp;
                this->climbtime = this->climbtime + 1;
            }
            else{
                this->disclimbtime = this->disclimbtime + 1;
            }
        }
        return ans;
    };
    void clear(void){
        for(uint64_t i=0;i<this->maxpagenums; i++){
            this->visitcount[i].life = 0;
        }
    };
    uint64_t* access(uint64_t addr_temp1,uint64_t addr_temp2,bool iswrite){
        uint64_t addr_temp = ((addr_temp1) & 0xffffffff)|(((addr_temp2)<<32)& 0xffffffff00000000);
        //this->totaltime = this->totaltime + 1;
        uint64_t* ans = (uint64_t*) malloc(10*sizeof(uint64_t));
        if (iswrite == 0){
        	ans[0] = 0;
        	ans[1] = this->maplist[addr_temp] & 0xffffffff;
        	ans[2] = (this->maplist[addr_temp] >>32) & 0xffffffff;
            return ans;
        }
        this->visitcount[addr_temp].life = this->visitcount[addr_temp].life + 1;
        ans = climber(addr_temp,this->visitcount[addr_temp].life);
        uint64_t addr = this->maplist[addr_temp];
        this->lifelist[addr].life = this->lifelist[addr].life - 1;
        return ans;
    };
};
#endif