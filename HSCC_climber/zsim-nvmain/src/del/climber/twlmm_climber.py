#coding:utf-8
import random
import numpy as np
import math


tracepath = 'whb_trace.dat'
##############################
##############################
pageshift = 12

interinterval = 128
swapthreshold = 32 
##########################################################
####config################################################
mu = 0.3  #
sigma = mu*0.11 #

class memorymodel:
    def __init__(self, areasize):
        ##areasize:最大页号，attacktype:攻击类型；no:序号；areashift：相对于页号的粒度移位4MB = 10
        #self.areashift = areashift
        #self.maxpagenums = (areasize >> areashift)
        self.maxpagenums = areasize
        self.attacktype = 0
        print('maxpagenums' + str(self.maxpagenums))
        np.random.seed(0)
        print("gen life distribution begin")
        #x = np.random.normal(loc = mu, scale = sigma, size = self.maxpagenums)
        #x = np.linspace(3000000,170000000,self.maxpagenums)
        areanums = self.maxpagenums >> 10
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
        print("gen life distribution end")
        self.lifelist = [[0,x[0]] for y in range(len(x))]
        self.lifelist2 = [[0,x[0]] for y in range(len(x))]
        #self.ideallifelist = [0 for y in range(len(x))]
        self.interswapcount = [0 for y in range(len(x))]
        minlifetime = 1000000000
        maxlifetime = 0
        for i in range(len(x)) :
            if(minlifetime > x[i]):
                minlifetime = x[i]
            if(maxlifetime < x[i]):
                maxlifetime = x[i]
            self.lifelist[i][0] = i
            self.lifelist[i][1] = x[i]###页面i寿命为x[i]
            self.lifelist2[i][0] = i
            self.lifelist2[i][1] = x[i]###页面i寿命为x[i]
            #ideallifelist[i] = x[i]###页面i寿命为x[i]
        print("minlifetime =%d,maxlifetime =%d"%(int(minlifetime),int(maxlifetime)))
        x = []
        self.pairlist = [0 for m in range(self.maxpagenums)]
        print("sort pages begin")
        self.sortedlist = sorted(self.lifelist2, key = lambda x:x[1])####按照寿命排序后，进行配对，配对公式：
        print("sort pages end")
        print("pair pages begin")
        for i in range(len(self.sortedlist)):
            self.pairlist[self.sortedlist[i][0]] = i
        print("pair pages end")
        self.intermaptable = [0 for m in range(int(self.maxpagenums))]###记录对间映射
        self.reversemaptable = [0 for m in range(int(self.maxpagenums))]###记录反向对间映射
        for i in range(len(self.intermaptable)):
            self.intermaptable[i] = i
            self.reversemaptable[i] = i
        self.isswap =  [0 for m in range(int(self.maxpagenums/2))] ##记录该对内部是否是处于交换状态
        self.swapvisitcount = [0 for m in range(int(self.maxpagenums))]###########记录每个页从上次交换起被访问的次数
        #self.hot_record = [0 for m in range(self.maxpagenums)]
        self.swaptimes = 0
        self.interswaptimes = 0
        self.returnstat = 0
        self.returnaddr1 = 0
        self.returnaddr2 = 0
        self.returnaddr3 = 0
        self.returnaddr4 = 0
    
        self.no = 0
    
        self.totalcount = 0
        self.remaptimes = 0
        self.totaltime = 0####循环内次数
    def getpairaddr(self, interswapcount,clifelist,areasize,addr_temp, intermaptable, reversemaptable,sourceaddr,iswrite):
    #global interinterval
        raddr = 0
        sourceaddr2 = 0
        pairaddr = 0
        pairaddr0 = 0
        sourceaddr2 = 0
        sourceaddr0 = 0
        if sourceaddr == 1:
            addr = self.maxpagenums - 1 - addr_temp
        else:
            addr = addr_temp
        interswapcount[addr] = interswapcount[addr] + 1
        addr_next = 0
        if interswapcount[addr] >= interinterval:
            self.interswaptimes = self.interswaptimes + 1
            interswapcount[addr] = 0
            addr_next = random.randint(0,areasize-1)
            if intermaptable[addr] >= self.maxpagenums / 2:
                pairaddr0 = self.maxpagenums - 1 - intermaptable[addr]
                sourceaddr0 = 1
            else:
                pairaddr0 = intermaptable[addr]
                sourceaddr0 = 0
            while addr_next == intermaptable[addr]:
                addr_next = random.randint(0,areasize-1)           
                ####只写一次
#                if addr_next >=self.maxpagenums / 2:
#                    pairaddr = self.maxpagenums - 1 - addr_next
#                    sourceaddr2 = 1
#                else:
#                    pairaddr = addr_next
#                    sourceaddr2 = 0
                ##只能和对应位置交换
                #if sourceaddr0 ^ self.isswap[pairaddr0] != sourceaddr2 ^ self.isswap[pairaddr]:
                #    continue

            if sourceaddr0 ^ self.isswap[pairaddr0] == 0:
                self.returnstat = 3
                self.returnaddr1 = self.sortedlist[intermaptable[addr]][0]
                clifelist[self.sortedlist[intermaptable[addr]][0]][1] = clifelist[self.sortedlist[intermaptable[addr]][0]][1] - 1 
            else:
                self.returnstat = 3
                self.returnaddr1 = self.sortedlist[areasize - 1 - intermaptable[addr]][0]
                clifelist[self.sortedlist[areasize - 1 - intermaptable[addr]][0]][1] = clifelist[self.sortedlist[areasize - 1 - intermaptable[addr]][0]][1] - 1    
            #只写一次
            #if sourceaddr2 ^ self.isswap[pairaddr] == 0:
            #    clifelist[self.sortedlist[addr_next][0]][1] = clifelist[self.sortedlist[addr_next][0]][1] - 1 
            #else:
            #    clifelist[self.sortedlist[self.maxpagenums - 1 - addr_next][0]][1] = clifelist[self.sortedlist[self.maxpagenums - 1 - addr_next][0]][1] - 1 
            
            #clifelist[areasize * 2 - 1 - addr_next][1] = clifelist[areasize * 2 - 1 - addr_next][1] - 1    

            temp = intermaptable[addr]
            intermaptable[addr] = addr_next
            reverseaddr = reversemaptable[addr_next]
            interswapcount[reverseaddr] = 0
            intermaptable[reverseaddr] = temp
            reversemaptable[temp] = reversemaptable[addr_next]
            reversemaptable[addr_next] = addr
        raddr = intermaptable[addr]
        return raddr

    def swaparbiter (self, life1,life2):
        toss = random.random()
        if toss >= life1 / (life1 + life2):
            return 1
        else:
            return 0
    def access(self,addr_temp1,addr_temp2,iswrite):
        #addr_temp2:页号,,现在是区域号
        #addr_temp = (addr_temp2 >> self.areashift)
        ans = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        ##5,6,7,8,9;10,11,12,13,14
        self.returnaddr1 = 0
        self.returnaddr2 = 0
        addr_temp = ((addr_temp1)& int('0xffffffff',16))|(((addr_temp2)<<32)& int('0xffffffff00000000',16));
        isdoswap = 0
        pairindex_temp = self.pairlist[addr_temp]
        pairindex = 0
        sourceaddr = 0 #要写的地址是块内块0还是块1的地址，用于判断是否交换 
        if pairindex_temp >=self.maxpagenums / 2:
            pairindex = self.maxpagenums - 1 - pairindex_temp
            sourceaddr = 1
        else:
            pairindex = pairindex_temp
            sourceaddr = 0
        self.returnaddr0 = addr_temp
        newaddr = self.getpairaddr(self.interswapcount, self.lifelist, int(self.maxpagenums), pairindex, self.intermaptable, self.reversemaptable,sourceaddr,iswrite)
        if newaddr >=self.maxpagenums / 2:
            pairaddr = self.maxpagenums - 1 - newaddr
            sourceaddr = 1
        else:
            pairaddr = newaddr
            sourceaddr = 0
        addr = self.sortedlist[pairaddr][0]
        nowaddr2 = self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]
        if sourceaddr ^ self.isswap[pairaddr] == 0:
            if iswrite == 1:
                self.swapvisitcount[addr] = self.swapvisitcount[addr] + 1
            if self.swapvisitcount[addr] >= swapthreshold:
                isdoswap = 1
        else:
            if iswrite == 1:
                self.swapvisitcount[nowaddr2] = self.swapvisitcount[nowaddr2] + 1
            if self.swapvisitcount[nowaddr2] >= swapthreshold:
                isdoswap = 1
        if isdoswap == 1:
            self.swapvisitcount[addr] = 0
            self.swapvisitcount[nowaddr2] = 0
            isdoswap = 0
            swaptemp = 0
            if sourceaddr == 1:
                swaptemp = self.swaparbiter(self.lifelist[nowaddr2][1],self.lifelist[addr][1])#####在执行过程中根据当前寿命调整
            else:
                swaptemp = self.swaparbiter(self.lifelist[addr][1],self.lifelist[nowaddr2][1]) 
            if swaptemp ^ self.isswap[pairaddr] == 1:
                #block end
                #########################################
                ###进行交换，匹配对寿命降低
                self.swaptimes = self.swaptimes + 1
                self.returnstat = self.returnstat + 1
                self.returnaddr3 = self.returnaddr1
                self.returnaddr1 = addr
                self.returnaddr2 = self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]
                self.lifelist[addr][1] = self.lifelist[addr][1] - 1
                self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] = self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] -1
                self.isswap[pairaddr] = self.isswap[pairaddr] ^ 1
            else:
                if sourceaddr ^ self.isswap[pairaddr] == 0:
                    #self.returnstat1 = 0
                    self.returnaddr2 = self.returnaddr1
                    self.returnaddr1 = addr
                    self.lifelist[addr][1] = self.lifelist[addr][1] - 1####当前页寿命降低
                else:
                    #self.returnstat1 = 0
                    self.returnaddr2 = self.returnaddr1
                    self.returnaddr1 = self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]
                    self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] = self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] -1
        else:
            if sourceaddr ^ self.isswap[pairaddr] == 0:
                if iswrite == 1:
                    self.lifelist[addr][1] = self.lifelist[addr][1] - 1####当前页寿命降低
                #self.returnstat = self.returnstat + 1
                self.returnaddr2 = self.returnaddr1
                self.returnaddr1 = addr
            else:
                #self.returnstat = self.returnstat + 1
                self.returnaddr2 = self.returnaddr1
                self.returnaddr1 = self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]
                if iswrite == 1:
                    self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] = self.lifelist[self.sortedlist[self.maxpagenums - 1 - self.pairlist[addr]][0]][1] -1
        ans[0] = self.returnstat
        ans[1] = self.returnaddr1 & int('0xffffffff', 16)
        ans[2] = (self.returnaddr1>>32) & int('0xffffffff', 16) 
        ans[3] = self.returnaddr2 & int('0xffffffff', 16)
        ans[4] = (self.returnaddr2>>32) & int('0xffffffff', 16) 
        ans[5] = self.returnaddr3 & int('0xffffffff', 16)
        ans[6] = (self.returnaddr3>>32) & int('0xffffffff', 16) 
        self.returnstat = 0
        return ans
        #self.totalcount = self.totalcount + 1