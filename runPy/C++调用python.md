# C++调用python脚本

基本使用方法: python提供了一套C API库, 使得方法从C/C++调用python模块.

### 初始化python解释器环境

```c++
void Py_Initialize()//初始化python解释器.
int Py_Initialized()//返回python解析器的是否以及初始化完成, 若完成,返回大于0, 否则返回0
void Py_Finalize()//撤销Py_Initialize().
```

### 调用python脚本

```c++
int PyRun_SimpleString(const char*)//执行一个简单的python脚本命令函数
int PyRun_SimpleFile(FILE *fp, const char *filename)//从fp中把python脚本的内容读取到内容中并执行, filename应为fp对应的文件名.
```

### 实例

```c++
# include <Python.h>
int main()
{
    Py_Initialize(); //初始化
    PyRun_SimpleString("pint ('hello')");
    Py_Finalize();//释放资源
}
```

### 动态加载python模块并执行函数

```c++
//加载模块
PyObject* PyImport_ImportModule(char *name);//使用c字符串加载模块

PyObject* PyImport_Import(PyObject *name);//name是一个python对象, 表这个python对象需要通过PyString_FromString(const char*)来生成, 其值为要导入的模块名
PyObject* PyString_FromString(const char*);

//导入函数
PyObject* PyModule_GetDict(PyObject *module);//PyModule_GetDict()可获得python模块中的函数列表. PyModule_GetDict()返回一个字典, 字典的关键字为函数名, 值为函数的调用地址.字典里面的值可以通过PyDict_GetItemString()函数来获取, 其中p是PyModule_GetDict()的字典, 而key则是对应的函数名.

PyObject* PyObject_GetAttrString(PyObject *o, char *attr_name);
//返回模块对象中的attr_name属性或函数, 相当于python中国表达式语句o.attr_name


//调用函数相关
PyObject* PyObject_CallObject(PyObject *callabel_object, PyObject *args);
PyObject* PyObject_CallFunction(PyObject *callable_object, char *format,...);
//在c中调用python的函数. callable_object为要调用的函数对象, 也就是通过上述导入函数得到的函数对象, 而区别在于前者使用Python的tuple来传参, 后者则使用类似于C语言printf的风格进行传参.
//如果不需要参数, 那么args可能为NULL. 返回成功时调用的结果, 返回失败时返回NULL .
```

- 实例

  1. python

  ```python
  #script/sayHello.py
  def say():
      print('hello')
  ```

  2. c++

  ```c++
  #include <Python.h>
  #include <iostream>
  
  using namespace std;
  int main(){
      Py_Initialize();
      if(!Py_IsInitialized()){
          cout<<"python init fall"<<endl;
          return 0;
      }
      PyRun_SimpleString("import sys");
      PyRun_SimpleString("sys.path.apped('./script')");
      
      PyObject* pModule = PyImport_ImportModule("sayHello");
      if(pModule==NULL){
          cout<<"module not found"<<endl;
          return 1;
      }
      
      PyObject* pFunc = PyObject_GetAttrString(pModule, "say");
      if(!pFunc){
          cout<<"not found function add_num"<<endl;
          return 0;
      }
      
      PyObject_CallObject(pFunc, NULL);
      
      Py_Finalize();
      return 0;
  }
      
  ```

### 调用参数

在C/C++, 所有的Python类型都被声明为PyObject, 为能使C++能操作python的数据, python提供了python各种类型和c语言数据类型的转换

- 数字与字符\

```c++
PyObject* Py_BuildValue(const char *format,...);
//P_BuildValue()提供了类似于c语言printf的参数构造方法, format使要构造的参数的类型列表, 函数中剩余的参数即要转换的c语言的整型, 浮点型, 字符串等. 其返回值为PyObject型的指针
```

- format对应的类型列表

```c++
s(str or None)[char *];//使用'utf-8'编码将以NULL结尾的c字符串转为Python str对象. 如果C字符串为NULL,则表示None

s#(str or None)[char *, int];//使用'utf-8编码, 将C字符串及其长度转为python str对象. 如果字符串指针为NULL, 则忽略长度返回None

y(字节)[char *];//这回将C字符串转为python字节对象.

z(str or None)[char *]; //与s相同

i(int)[int];//将普通的Cint转为python整数对象

b(int)[char]; //将C char转为python整数对象

h(int)[short int];//将普通的C shor int转换位Python整数对象.

d(float)[double];//将C double转为python浮点数

f(float)[float];//将C float转为Python浮点数

O(object)[PyObject *];//不改变Python对象的传递(引用计数除外, 增加1). 如果传入的对象是NULL指针, 则假定这是由于产生参数的调用发现错误并设置了异常.因此Py_BuildValue()将返回NULL, 但不会引发异常.
```

- 列表

```c++
PyObject* PyList_New(Py_ssize_t len);//创建一个新的python列表, len为所创建的长度

int PyList_SetItem(PyObject *list, Py_ssize_t index, PyObject *item);//向列表中添加项, 当列表创建以后, 可以使用PyList_SetItem()函数向列表中添加项. list: 要添加的列表, index:所添加项的位置索引,item:所添加的值

PyObject* PyList_GetItem(PyObject *list, Py_ssize_t index);//返回列表中某些的值.

Py_ssize_t PyList_Size(PyObject* list);//返回列表中列表对象的长度, 像狼与len(list)

int PyList_Append(PyObject *list, PyObject *item);//append()
int PyList_Sort(PyObject *list);//sort()
int PyList_Reverse(PyObject *list);//reverse()

```

- 元组

```c++
PyObject* PyTuple_New(Py_ssize_t len);//PyTuple_New()函数返回所创建的元组, 其函数原型如下所示. len: 所创建元组的长度

int PyTuple_SetItem(PyObject *p, Py_ssize_t pos, PyObject *o);//当元组创建以后, 可以使用PyTuple_SetItem()函数项元组中添加项.pos:所添加项的位置索引, o: 所添加的值

PyObject* PyTuple_GetItem(PyObject *p, py_ssize_t pos);//可以使用python/c api中PyTuple_GetItem()函数来获取元组中某项的值, p表要进行操作的元组, pos:所添加项的位置索引, o: 所添加的值

Py_ssize_t PyTuple_Size(PyObject *p); //获取元组对象的指针, 并返回该元组的大小

int _PyTuple_Resize(PyObject *p, Py_size_t newsize);//当元组创建以后可以使用_PyTuple_Resize()函数重新调整元组的大小.
```

- 字典

```c++
PyObject* PyDict_New(); //返回所创建的字典

int PyDict_SetItem(PyObject *p, PyObject *key, PyObject *val);
int PyDict_SetItemString(PyObject *p, const char *key, PyObject *val);//当字典创建后, 使用上两个函数项字典中添加项, p：要进行操作的字典。key：添加项的关键字，
//PyDict_SetItem()为PyObject型, 对应PyDict_SetItemString()位char型, val:添加的值

PyObject* PyDict_GetItem(PyObject *p, PyObject *key);//PyObject型
PyObject* PyDict_GetItemString(PyObject *p, const char *key);//char型
//都是用来获取字典中某些的值

PyObject* PyDict_Items(PyObject *p);//items()
PyObject* PyDict_Keys(PyObject *p);//keys()
PyObject* PyDict_Values(PyObject *p);//values
```



### 返回值

Python的返回值是PyObject类型, 因此, 在python脚本返回到C/C++后, 需要结构Python数据位C的类型. 

```c++
int PyArg_Parse(PyObject *args, char *format,...);//根据format将args的值转化成c类型的值, format接受的类型和上述Py_BuildValue()的是一样
```

### 释放资源

Python使用应用计数机制对内存进行管理, 实现自动垃圾回收. 在C/C++中使用Python对象时, 应正确地处理引用计数, 否则容易导致内存泄漏. Python提供了`Py_CLEAR()`, `pY_DECRFF()`

当使用Python/C api中的函数创建列表, 元组, 字典后, 就在内存中生成了这些对象的应用计数. 在对其完成操作后应该使用`Py_CLEAR(), Py_DECREF()`等宏来销毁这些对象.

```c++
void Py_CLEAR(PyObject *o);
void Py_DECREF(PyObject *o);
//对于Py_CLEAR()其参数可以为NULL指针，此时，Py_CLEAR()不进行任何操作。而对于Py_DECREF()其参数不能为NULL指针，否则将导致错误。
```

### 实例

- python

```c++
#script/Py2Cpp.py
def add_num(a,b):
return a+b
```

- c++

```c++
#include <Python.h>
#include <iostream>

using namespace std;

int main()
{
    Py_Initialize();
    if(!Py_IsInitialized()){
        cout<<"python init fail"<<endl;
        return 0;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./script')");
    
    PyObject* moduleName = PyString_FromString("Py2Cpp");
    PyObject* pModule = PyImport_Import(moduleName);
    if(pModule == NULL){
        cout <<"module not found"<<endl;
        return 0;
    }
    
    PyObject* pFunc = PyObject_GetAttrString(pModule, 'add_num');
    if(!fFunc){
        cout<"not found function"<<endl;
        return 0;
    }
    
    PyObject* args = Py_BuildValue("(ii)",28,103);
    PyObject* pRet = PyObject_CallObject(pFunc, args);
    Py_DECREF(args);
    
    int res = 0;
    PyArg_Parse(pRet, "i",&res);
    Py_DECREF(pRet);
    cout<<res<<endl;
    
    Py_Finalize();
    return 0;
}
```

## question

- Replacing PyString_FromString method in python3

```c++
pValue = PyLong_FromLong(atoi(argv[i+3]));
pValue = PyString_FromString("A string instead of a number");
```

```error
main.cpp error: "PyString_FromString" was not declared in this scope
```

Answer

`PyUnicode_FromString()`

```C++
if (!(pValue=PyUnicode_FromString("A string instead of a number")))
    return NULL;
```

