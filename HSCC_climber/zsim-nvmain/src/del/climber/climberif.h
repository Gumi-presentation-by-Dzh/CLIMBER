#include <cstdlib>
#include <cstdio>
#include "Python.h"
class climberif{
  public:
    PyObject * pModule;
    PyObject * pFunc;
    PyObject * pArg;
    PyObject * pClass;
    PyObject * pObject;
    climberif(uint64_t memorysize, const char* systype){
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./')"); 
        pModule = PyImport_ImportModule(systype);
        /*if ( !pModule )
        {
          //printf("can't find pytest.py");
          getchar();
        }*/
        pClass  = PyObject_GetAttrString(pModule, "memorymodel");//得到那个类 
        pArg = PyTuple_New(1);
        PyTuple_SetItem(pArg, 0, Py_BuildValue("i", memorysize));
        pObject = PyEval_CallObject(pClass, pArg);//生成一个对象，或者叫作实例
        pFunc = PyObject_GetAttrString(pObject, "access");//得到该实例的成员函数

    }
    uint64_t* access(uint64_t address,int iswrite){
        PyObject * pArgcall;
        PyObject * result;
        pArgcall = PyTuple_New(3);
        int arg1 = (int)(address & 0xffffffff);
        int arg2 = (int)((address>>32) & 0xffffffff);
        PyTuple_SetItem(pArgcall, 0, Py_BuildValue("i", arg1));
        PyTuple_SetItem(pArgcall, 1, Py_BuildValue("i", arg2));
        PyTuple_SetItem(pArgcall, 2, Py_BuildValue("i", iswrite));
        result = PyEval_CallObject(pFunc, pArgcall);//执行该实例的成员函数
        int ans[20];
        //uint64_t* readdress = (uint64_t*)malloc(10*sizeof(uint64_t));
        uint64_t* returnvalue = (uint64_t*)malloc(10*sizeof(uint64_t));
        if(PyList_Check(result)){
            int len = PyList_Size(result);
            for(int j = 0; j < len; ++j){
                PyObject * item = PyList_GetItem(result,j);
                PyArg_Parse(item, "i", &(ans[j]));
                //printf("ans[%d]:%d\n",j,ans[j]);
            }
        }
        else{
            printf("error result");
        }
        returnvalue[0] = ans[0];
        returnvalue[1] = (((uint64_t)ans[1])&0xffffffff)|((((uint64_t)ans[2])<<32)&0xffffffff00000000);
        returnvalue[2] = (((uint64_t)ans[3])&0xffffffff)|((((uint64_t)ans[4])<<32)&0xffffffff00000000);
        returnvalue[3] = (((uint64_t)ans[6])&0xffffffff)|((((uint64_t)ans[7])<<32)&0xffffffff00000000);
        returnvalue[4] = (((uint64_t)ans[8])&0xffffffff)|((((uint64_t)ans[9])<<32)&0xffffffff00000000);
        //returnvalue[3] = (((uint64_t)ans[5])&0xffffffff)|((((uint64_t)ans[6])<<32)&0xffffffff00000000);
//        if(ans[0]==0){
//            readdress[0] = 0;
//            readdress[1] = 1;
//            readdress[2] = returnvalue[1];
//            //printf("return[0]:%llu,return[1]:%llu,return[2]:%llu\n",returnvalue[0],returnvalue[1],returnvalue[2]);
//            
//        } 
//        else if(ans[0]==1){
//            readdress[0] = 1;
//            readdress[1] = returnvalue[2];
//            readdress[2] = 2;
//            readdress[3] = returnvalue[1];
//            readdress[4] = returnvalue[2];
//        }
//        else if(ans[0]==2){///错误
//            readdress[0] = 1;
//            readdress[1] = returnvalue[1];
//            readdress[2] = 0;
//        }
//        else if(ans[0]==3){
//            readdress[0] = 0;
//            readdress[1] = 1;
//            readdress[2] = returnvalue[1];
//        }
//        else if(ans[0]==4){
//            readdress[0] = 1;
//            readdress[1] = returnvalue[2];
//            readdress[2] = 2;
//            readdress[3] = returnvalue[1];
//            readdress[4] = returnvalue[2];
//        }
//        else if(ans[0]==5){
//            readdress[0] = 2;
//            readdress[1] = returnvalue[2];
//            readdress[2] = returnvalue[3];
//            readdress[3] = 3;
//            readdress[4] = returnvalue[1];
//            readdress[5] = returnvalue[2];
//            readdress[6] = returnvalue[3];
//        }
        return returnvalue;
    }   
    ~climberif(){
        Py_Finalize();
    }
};