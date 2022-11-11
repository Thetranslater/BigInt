# BigInt
---
这是一个BigInterger库，使用visual studio 2022编译而成，目前是c++17标准。
在项目中使用`#include<include/BigInt.h>`将库声明包含进来，在`lib`文件夹中可以找到lib文件。

```
#include<include/BigInt.h>

int main(){
  BigInt ullInt{2183812ull};
  BigInt sInt{"-32142132132"s};
  BigInt fInt{3.1415926f};
  
  return 0;
}
```
你能将任何可以隐式转换到`douoble`的类型隐式转换到`BigInt`，`BigInt`允许缩窄转换。

`BigInt`支持`+`,`-`,`*`,`/`,`%`,`++`,`--`等运算符，**不**支持移位运算，支持流输入输出，支持hash，支持大小比较。

`BigInt`需要显式转换到基本数据类型，过大的数据会缩窄到最大值或最小值。

`BigInt`不提供某些方便的函数，类似的函数你可以在`util`中找到，比如`to_string`,`sign`

```
util::to_string(BigInt(11235813));
util::sign(BigInt(-1));
```
