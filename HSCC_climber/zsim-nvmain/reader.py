import random
import numpy as np
import sys
import xlwt
import os
work_book = xlwt.Workbook(encoding='utf-8')
#work_book1 = xlwt.Workbook(encoding='utf-8')
#work_book2 = xlwt.Workbook(encoding='utf-8')
benchmark = ['bfs','cactusADM','fluidanimate','graph500','matching','mis','neighbors','setCover','spmv',\
    'bodytrack','cannel','dict','freqmine','isort','mcf','mst','soplex','streamcluster','linpack','hpccg']
systype = ['climber','noclimb','twl']
swap2num = [[0 for i in range(len(benchmark))] for j in range(len(systype))]
swap3num = [[0 for i in range(len(benchmark))] for j in range(len(systype))]
cycle = [[0 for i in range(len(benchmark))] for j in range(len(systype))]
instr = [[0 for i in range(len(benchmark))] for j in range(len(systype))]
sheet = []
for st in range(len(systype)):
    for bm in range(len(benchmark)):
        filepath = systype[st]+'/'+benchmark[bm]+'/'
        if not os.access(filepath+'zsim.log.0',os.F_OK):
            continue
        with open (filepath+'zsim.log.0') as zl:
            for line in zl:
                line1 = line.split()[2].split(',')
                if line1[0].split(':')[0] == 'swap2':
                    swap2num[st][bm] = int(line1[0].split(':')[1])
                    swap3num[st][bm] = int(line1[1].split(':')[1])
                    break
        if not os.access(filepath+'zsim.out',os.F_OK):
            continue
        with open (filepath+'zsim.out') as zo:
            for line in zo:
                line1 = line.split()
                if len(line1) >= 5:
                    #print(line1)
                    if line1[0] == 'cycles:':
                        cycle[st][bm] = cycle[st][bm] + int(line1[1])
                    if line1[0] == 'instrs:':
                        instr[st][bm] = instr[st][bm] + int(line1[1])
for st in range(len(systype)):
    sheet.append(work_book.add_sheet(systype[st]))
for i in range(len(sheet)):
    sheet[i].write(0,0,'benchmark')
    sheet[i].write(0,1,'cycles')
    sheet[i].write(0,2,'instrs')
    sheet[i].write(0,3,'IPC')
    sheet[i].write(0,4,'swap2')
    sheet[i].write(0,5,'swap3')
    sheet[i].write(0,6,'totalswap')
    for j in range(len(benchmark)):
        sheet[i].write(j+1,0,benchmark[j])
        sheet[i].write(j+1,1,cycle[i][j])
        sheet[i].write(j+1,2,instr[i][j])
        if cycle[i][j] != 0:
            sheet[i].write(j+1,3,(float)(instr[i][j])/(float)(cycle[i][j]))
        else:
            sheet[i].write(j+1,3,0)
        sheet[i].write(j+1,4,swap2num[i][j])
        sheet[i].write(j+1,5,swap3num[i][j])
        sheet[i].write(j+1,6,swap2num[i][j]+swap3num[i][j])
work_book.save('recorder.xls')



