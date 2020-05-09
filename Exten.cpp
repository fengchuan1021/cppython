
#include "Python.h"
#include "marshal.h"
#include "base64.h"
#include <string> 
#include <fstream>
#include <sstream>
#include<cpr/cpr.h> 
#include<iostream>
#include <cmath>
#include "BigIntegerLibrary.hh"

using namespace std;



// 函数主体
int add(int a, int b) {
    return a + b;
}
int get(char* url, string& result) {
    auto r = cpr::Get(cpr::Url{ url });
    result = r.text;
}
std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// 包裹函数
static PyObject* Exten_get(PyObject* self, PyObject* args) {
    char* url;
    // 获取数据，i代表int，ii代表两个int
    // 如果没有获取到，则返回NULL
    string result;
    if (!PyArg_ParseTuple(args, "s", &url)) {
        return NULL;
    }
    get(url, result);
    return (PyObject*)Py_BuildValue("s", result.c_str());
}



// 包裹函数
static PyObject* Exten_test(PyObject* self, PyObject* args) {
    ifstream in("test.pyc");

    
    if (in.is_open()) {
        std::stringstream buffer;
        buffer << in.rdbuf();
        string code;
        code = buffer.str();
        PyObject* globals = PyDict_New();
        PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
        
        string srn = "99182532697721077997602901876785554666543900491259498484616373202559395393687";
        BigUnsigned rn = stringToBigUnsigned(srn);
        string sre = "55622851317149709335549339389648233458824721480008637637406360956796787452225";
        BigUnsigned re = stringToBigUnsigned(sre);
        //const BigUnsigned tt=rn;
        vector<unsigned char> total;
        auto arr = split(code, '$');
        for(auto ar : arr){
            BigInteger tmpint = stringToBigInteger(ar);
          
            BigUnsigned desc=modexp(tmpint, re, rn);
            vector<unsigned char> out;
            while (desc>0) {
                BigUnsigned c = desc % 256;
                out.push_back((unsigned char)c.toInt());
                desc=desc/256;
            }
            int padn = 30 - out.size();
            while (padn > 0) {
                out.push_back(0);
                padn -= 1;
            }
            reverse(out.begin(), out.end());
            for (auto it = out.begin(); it != out.end(); it++) {
                total.push_back(*it);
            }
        }
        string mybuf(total.begin(),total.end());
       auto obj = PyMarshal_ReadObjectFromString(mybuf.c_str(), mybuf.length());
       PyObject* module = PyImport_ExecCodeModule("test_module", obj);
       Py_DECREF(module);
       Py_DECREF(obj);
       Py_DECREF(globals); 


    }
    return (PyObject*)Py_BuildValue("i", 0);
}

// 添加PyMethodDef ModuleMethods[]数组
static PyMethodDef ExtenMethods[] = {
    // add：可用于Python调用的函数名，Exten_add：C++中对应的函数名
    {"get",(PyCFunction)Exten_get,METH_VARARGS},
    {"test",(PyCFunction)Exten_test,METH_VARARGS},
    
    {NULL,NULL},
};


static struct PyModuleDef ExtenModule = {
    PyModuleDef_HEAD_INIT,
    "Exten",//模块名称
    NULL,
    0,
    ExtenMethods
};

//初始化函数 dll入口
PyMODINIT_FUNC PyInit_Exten() {
   
   return PyModule_Create(&ExtenModule);
}