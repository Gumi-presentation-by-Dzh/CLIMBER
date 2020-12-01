#coding:utf-8
import random
import numpy as np
import math
##############################
##############################
class memorymodel:
    def __init__(self, areasize):
        ##areasize:最大页号，attacktype:攻击类型；no:序号；areashift：相对于页号的粒度移位4MB = 10
        #self.areashift = areashift
        #self.maxpagenums = (areasize >> areashift)
        self.maxpagenums = areasize
        self.climbershift = 17
        self.climberenable = 2
        areanums = self.maxpagenums >> 10
        self.attacktype = 0
        #print('maxpagenums:' + str(self.maxpagenums))
        self.no = 0
        self.randomkey = 0
        self.randomshift = self.climbershift
        self.randomenable = 1
        self.life2sorted = [0 for x in range(self.maxpagenums)]
        np.random.seed(0)
        #print("gen life distribution begin")
        mu = 0.3 #lifetime mean  1*10^8
        sigma = mu*0.11 #
        p = np.random.normal(loc = mu, scale = sigma, size = 2*areanums)
        #p1 = [[0,0] for i in range(2*areanums) ]
        #for i in range(len(p)):
        #    p1[i][0] = i
        #    p1[i][1] = p[i]
        p.sort()
        #ps = sorted(p1, key = lambda x:x[1])

        x = [0 for i in range(self.maxpagenums)]
        for i in range(self.maxpagenums):
            x[i] = math.pow(p[areanums+(i>>10) - 1],-12)*90.345

            #for j in range(1<<10):
            #x[(ps[areanums+i][0]<<10)+j] = math.pow(ps[areanums+i][1],-12)*90.345
        #x = np.random.normal(loc = 100000000.0, scale = 100000000*0.11, size = self.maxpagenums)
        #x = np.linspace(3000000,170000000,self.maxpagenums)
        print("gen life distribution end")
        self.minlifetime = 10000000000
        self.maxlifetime = 0.0
        self.lifelist = [[0,0] for y in range(len(x))]
        self.lifelist2 = [[0,0] for y in range(len(x))]
        for i in range(len(x)) :
            if(self.minlifetime > x[i]):
                self.minlifetime = x[i]
            if(self.maxlifetime < x[i]):
                self.maxlifetime = x[i]
            self.lifelist[i][0] = i
            self.lifelist[i][1] = x[i]###页面i寿命为x[i]
            self.lifelist2[i][0] = i
            self.lifelist2[i][1] = x[i]###页面i寿命为x[i]
            self.life2sorted[i] = i
        x = []
        print("minlifetime =%d,maxlifetime =%d"%(int(self.minlifetime),int(self.maxlifetime)))
        self.maplist = [0 for x in range(self.maxpagenums)]
        self.reverselist = [0 for x in range(self.maxpagenums)]####climber
        self.sortednow = [0 for  y in range(self.maxpagenums)]
        self.climbla2hot = [0 for  y in range(self.maxpagenums)]###climber
        self.climberlocthre= [0 for  y in range(self.maxpagenums)]
        self.climberstart = [1 for  y in range(self.maxpagenums)]
        self.maxSL = 0
        self.start = 0 #climber start flag
        print("sort pages begin")
        self.sortedlist = sorted(self.lifelist2, key = lambda x:x[1])####按照寿命排序后，进行配对，配对公式：
        for i in range(len(self.sortedlist)):
            self.life2sorted[self.sortedlist[i][0]] = i
            self.sortednow[i] = self.sortedlist[i][0]
            self.climbla2hot[self.sortedlist[i][0]] = i
            self.climberlocthre[i] = int(10*(self.lifelist2[i][1]/self.minlifetime))
            #self.climberlocthre[i] = climbethreshold
        print("sort pages end")
        print("map begin")
        for i in range(len(self.sortedlist)):
            self.maplist[i] = i
            self.reverselist[i] = i####climber
        print("map end")
        self.visitcount = [[0 for x in range(2)] for y in range(self.maxpagenums)]###########记录每个页从上次交换起被访问的次数
        for i in range(len(self.visitcount)):
            self.visitcount[i][0] = i
        self.totalcount = 0
        self.remaptimes = 0
        self.totaltime = 0####循环内次数
        self.climberpoint = (self.maxpagenums - 1) >> self.climbershift
        self.climbtime = 0
        self.disclimbtime =0
        self.climbtop = 0
        self.climbup = 0
        self.climbmid = 0
        self.climbdown = 0
        self.rank2addrp = 0
        self.rank2addr = [0 for  y in range(self.maxpagenums)]
        self.map2weakaddr = self.maxpagenums - 1
    def climber(self,addr_temp,counterv):

        ans = [0,0,0,0,0]
        addr= self.maplist[addr_temp]
        ans[1] = addr & int('0xffffffff', 16)
        ans[2] = (addr>>32) & int('0xffffffff', 16)
        localclimbethreshold = self.climberlocthre[addr]
        maxup = 0
        if ((counterv) % (localclimbethreshold) == 0) and (self.climberenable >= 1):
            #step = int(counterv / localclimbethreshold)
            randommax = 1<<self.climbershift
            climberareaindex = self.climbla2hot[addr_temp] >> self.climbershift
            print("into climber:" + str(climberareaindex))
            indexmax = (self.maxpagenums - 1) >> self.climbershift
            targetindex = climberareaindex

            targetindex0 = indexmax
            hotrandomaddr0 = random.randint(0, (1<<self.climbershift) - 1)
            targetaddr0 = self.sortedlist[(targetindex0 << self.climbershift) + hotrandomaddr0][0]
            tarla0 = self.reverselist[targetaddr0]
            targetcount0 = self.visitcount[tarla0][1]
            wearratecompare0 = self.lifelist[targetaddr0][1] / self.lifelist2[targetaddr0][1] - self.lifelist[addr][1] / self.lifelist2[addr][1]
            countercompare0 = targetcount0 - counterv

            if climberareaindex == indexmax: ###当前位置+1
                targetindex1 = -1
                wearratecompare1 = 0
            else:
                if climberareaindex == indexmax - 1:
                    targetindex1 = indexmax
                else:
                    targetindex1 = random.randint(climberareaindex + 1, self.climberpoint)
                hotrandomaddr1 = random.randint(1, (1<<self.climbershift) - 1)
                targetaddr1 = self.sortedlist[(targetindex1 << self.climbershift) + hotrandomaddr1][0]
                tarla1 = self.reverselist[targetaddr1]
                targetcount1 = self.visitcount[tarla1][1]
                wearratecompare1 = self.lifelist[targetaddr1][1] / self.lifelist2[targetaddr1][1] - self.lifelist[addr][1] / self.lifelist2[addr][1]
                countercompare1 = targetcount1 - counterv
            targetindex2 = climberareaindex
            hotrandomaddr2 = random.randint(1, (1<<self.climbershift) - 1)
            targetaddr2 = self.sortednow[(self.climbla2hot[addr_temp] + hotrandomaddr2) % (1<<self.climbershift) + (targetindex2<<self.climbershift)]
            tarla2 = self.reverselist[targetaddr2]
            targetcount2 = self.visitcount[tarla2][1]
            wearratecompare2 = self.lifelist[targetaddr2][1] / self.lifelist2[targetaddr2][1] - self.lifelist[addr][1] / self.lifelist2[addr][1]
            countercompare2 = targetcount2 - counterv
            if climberareaindex == 0: ###当前位置+1
                targetindex3 = -1
                wearratecompare3 = 0
            else:
                if climberareaindex == 1:
                    targetindex3 = 0
                else:
                    targetindex3 = random.randint(0, climberareaindex - 1)
                hotrandomaddr3 = random.randint(1, (1<<self.climbershift) - 1)
                targetaddr3 = self.sortedlist[(targetindex3 << self.climbershift) + hotrandomaddr3][0]

                tarla3 = self.reverselist[targetaddr3]
                targetcount3 = self.visitcount[tarla3][1]
                wearratecompare3 = self.lifelist[targetaddr3][1] / self.lifelist2[targetaddr3][1] - self.lifelist[addr][1] / self.lifelist2[addr][1]
                countercompare3 = targetcount3 - counterv

            tarla = -1
            targetaddr = -1
            tarla_temp0 = -1
            targetaddr_temp0 = -1
            tarla_temp1 = -1
            targetaddr_temp1 = -1
            tarla_temp2 = -1
            targetaddr_temp2 = -1
            tarla_temp3 = -1
            targetaddr_temp3 = -1

            if targetindex0 != -1 and ((wearratecompare0 > 0 and countercompare0 <= 0) or(wearratecompare0 < 0 and countercompare0 > 0)):
                tarla_temp0 = tarla0
                targetaddr_temp0 = targetaddr0
            if targetindex1 != -1 and ((wearratecompare1 > 0 and countercompare1 <= 0) or(wearratecompare1 < 0 and countercompare1 > 0)):
                tarla_temp1 = tarla1
                targetaddr_temp1 = targetaddr1
            if ((wearratecompare2 > 0 and countercompare2 <= 0) or(wearratecompare2 < 0 and countercompare2 > 0)): 
                tarla_temp2 = tarla2
                targetaddr_temp2 = targetaddr2
            if targetindex3 != -1 and ((wearratecompare3 > 0 and countercompare3 <= 0) or(wearratecompare3 < 0 and countercompare3 > 0)):
                tarla_temp3 = tarla3
                targetaddr_temp3 = targetaddr3

            if tarla_temp0 != -1 and (wearratecompare0 >= wearratecompare1 or tarla_temp1 == -1) and (wearratecompare0 >= wearratecompare2 or tarla_temp2 == -1) and (wearratecompare0 >= wearratecompare3 or tarla_temp3 == -1):
                tarla = tarla_temp0
                targetaddr = targetaddr_temp0
                self.climbtop = self.climbtop + 1
            elif tarla_temp1 != -1 and (wearratecompare1 >= wearratecompare2 or tarla_temp2 == -1) and (wearratecompare1 >= wearratecompare3 or tarla_temp3 == -1) and (wearratecompare1 >= wearratecompare0 or tarla_temp0 == -1):
                tarla = tarla_temp1
                targetaddr = targetaddr_temp1
                self.climbup = self.climbup + 1
            elif tarla_temp2 != -1 and (wearratecompare2 >= wearratecompare1 or tarla_temp1 == -1) and (wearratecompare2 >= wearratecompare3 or tarla_temp3 == -1) and (wearratecompare2 >= wearratecompare0 or tarla_temp0 == -1):
                tarla = tarla_temp2
                targetaddr = targetaddr_temp2
                self.climbmid = self.climbmid + 1
            elif tarla_temp3 != -1 and (wearratecompare3 >= wearratecompare1 or tarla_temp1 == -1) and (wearratecompare3 >= wearratecompare2 or tarla_temp2 == -1) and (wearratecompare3 >= wearratecompare0 or tarla_temp0 == -1):
                tarla = tarla_temp3
                targetaddr = targetaddr_temp3
                self.climbdown = self.climbdown + 1

#            tarla = -1
#            targetaddr = -1
#            if targetindex1 != -1 and ((wearratecompare1 > 0 and countercompare1 <= 0) or(wearratecompare1 < 0 and countercompare1 > 0)):
#                tarla = tarla1
#                targetaddr = targetaddr1
#            elif ((wearratecompare2 > 0 and countercompare2 <= 0) or(wearratecompare2 < 0 and countercompare2 > 0)): 
#                tarla = tarla2
#                targetaddr = targetaddr2
#            elif targetindex3 != -1 and ((wearratecompare3 > 0 and countercompare3 <= 0) or(wearratecompare3 < 0 and countercompare3 > 0)):
#                tarla = tarla3
#                targetaddr = targetaddr3
            if tarla != -1:
                self.lifelist[addr][1] = self.lifelist[addr][1] - 1
                ans[0] = 1
                ans[3] = ans[1]
                ans[4] = ans[2]
                ans[1] = targetaddr & int('0xffffffff', 16)
                ans[2] = (targetaddr>>32) & int('0xffffffff', 16)
                #ans[2] = targetaddr
                #if self.lifelist[addr][1] < 0:
                #    return -1
                if self.climberstart[addr] == 0 and self.maxSL < (self.climbla2hot[tarla] >> self.climbershift):
                    self.maxSL = (self.climbla2hot[tarla] >> self.climbershift)
                self.reverselist[targetaddr] = addr_temp
                self.reverselist[addr] = tarla
                self.maplist[addr_temp] = targetaddr
                self.maplist[tarla] = addr
                swaptemp = self.climbla2hot[addr_temp]
                self.climbla2hot[addr_temp] = self.climbla2hot[tarla]
                self.climbla2hot[tarla] = swaptemp
                self.climbtime = self.climbtime + 1
            else:
                self.disclimbtime = self.disclimbtime + 1
        return ans
    def access(self,addr_temp1,addr_temp2,iswrite):
        addr_temp = ((addr_temp1) & int('0xffffffff',16))|(((addr_temp2)<<32)& int('0xffffffff00000000',16));
        #addr = self.maplist[addr_temp]
        if iswrite == 0:
            return [0,self.maplist[addr_temp],0]
        self.visitcount[addr_temp][1] = self.visitcount[addr_temp][1] + 1
        print(self.visitcount[addr_temp][1])
        ans = self.climber(addr_temp,self.visitcount[addr_temp][1])
        addr = self.maplist[addr_temp]
        self.lifelist[addr][1] = self.lifelist[addr][1] - 1
        return ans

