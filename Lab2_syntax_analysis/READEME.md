### 1. 本程序中使用 ~ 符号替代 $\epsilon$

### 2. 输入样例

```c++
// 递归调用分析程序
1
num + num * ( num - num ) / num

```

```c++
// LL(1) 分析程序
2
E E' T T' F
E
+ - * / ( ) num ~
E -> T E'          
E' -> + T E' | - T E' | ~ 
T -> F T'          
T' -> * F T' | / F T' | ~ 
F -> ( E ) | num        
#
num + num * ( num - num ) / num

```

```c++
// LR(1) 分析程序
3
E T F
E
+ - * / ( ) num
E -> E + T | E – T | T
T -> T * F | T / F | F
F -> ( E ) | num
#
num + num * ( num - num ) / num

```

