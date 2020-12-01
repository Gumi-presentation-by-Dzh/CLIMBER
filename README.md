# CLIMBER: Defending Phase Change Memory Against Inconsistent Write Attacks


&#160; &#160; &#160; &#160; CLIMBER is a wear leveling scheme to counteract inconsistent write attacks. It  dynamically changes harmful address mappings in which heavily written addresses are mapped to weak cells. Thus, CLIMBER can correct prediction errors of wear-leveling algorithms and redirect intensive writes to strong cells based on a threshold of page hotness. CLIMBER can be easily integrated into previous PV-aware wear leveling schemes to defend against inconsistent write attacks and further improve the lifetime of PCM.

CLIMBER Dependencies, Compiling, Running, and Result
------------
**1.External Dependencies**  
&#160; &#160; &#160; &#160; Before running CLIMBER codes, it's essential that you have already install dependencies listing below.
* g++ (GCC) 4.8.5
* Dependencies of Zsim-NVMain(HSCC) (We use Zsim to collect memory access trace and envalute IPCs. You can also use other simulators to collect trace). You can find dependencies of HSCC in HSCC(https://github.com/CGCL-codes/HSCC)

**2.Compiling**
Use 'make' to compile source codes.
```javascript
[root @node1 CLIMBER]# make
```
**3.Running**

* First, get trace files by:
```javascript
[root @node1 CLIMBER/trace]# cat trace.tar.gz.* > trace.tar.gz
[root @node1 CLIMBER/trace]# tar -zxvf trace.tar.gz
```
* Then, make sure the trace you want to use is in your workspace and life_4G.dat(endurance distribution file) is in workspace/../life_4G.dat.
* Run CLIMBER by:
```javascript
[root @node1 workspace]# pathtoCLIMBER/attack_X_climber arg1 arg2 arg3 arg4
[root @node1 workspace]# pathtoCLIMBER/noattack_X_climber arg1 arg2 arg3 arg4
```
* X is wear-leveling schems; 'attack' is running with Inconsistent Write Attack (doesn't need trace files); 'noattack' is running without Inconsistent Write Attack (need trace files).
* Arg1 and arg2 are used to enable our climber and WPRM schemes. Arg3 is climber-regionsize(10-19) and arg4 is climber-threshold. These args(1,2,3,4) are useless for TWL. CLIMBER can be running without other schemes by setting arg1 = 2 for attack_hc_climber and noattack_hc_climber.

**4.Result**  
&#160; &#160; &#160; &#160; The wear rate result is recorded in X_c.dat and the overhead result is recorded in X_c.log.


