# 项目说明
## 总体目标
实现非线性流水线调度算法模拟

## 关键技术
- 流水线预约表
- 禁止表
- 初始冲突向量
- 冲突向量有向图生成及遍历
- 最小平均启动距离比较
- 代码简介
 
### 流水线预约表
由用户输入完成，在项目目录中可以使用 `input.txt` 文件将所用的用户输入记录在内，包括流水线预约表，测试程序时可自行修改内容

### 禁止表
根据预约表中每行预约占用位进行叠加，生成最终的禁止表

### 初始冲突向量
根据禁止表所指向的位置，对应成二进制0-1序列的数位开关，0为非禁止位，1为禁止位，序列从左到右对应禁止表从高到低位

### 冲突向量有向图生成及遍历
实现难点在于有向图DFS遍历，代码使用C语言递归实现，并将可用路径生成的闭环通过入栈的方式进行记录

### 最小平均启动距离比较
在遍历完所有闭环路径后，计算每条路径的平均启动距离，最小者为最优；如果最小平均启动距离有若干选择方案，那么选择吞吐率最高者为最优

### 代码简介
代码主体分为`nonlinear.h`和`nonlinear.c`，`inputs.txt`中预存了一份示例输入数据用于快速查看效果，编译器是*gcc 6.3.0*，系统环境是 *linux 4.14.50 armv7l*，硬件环境是树莓派2开发板

#### 编译
使用如下命令进行编译即可得到**a.out**文件：
>$ gcc nonlinear.c -lm -I/PathOfYourHeaderFile/

#### 执行
可以直接使用如下命令启动程序
>$ ./a.out

或者
>$ ./a.out < inputs.txt

借助重定向功能可以避免大量数据通过手工输入，方便观察结果

#### 输出
在输出内容的倒数第二行会提示`Find the minimum average ticks at site XX:...`，最后一行会给出形如`(ConflictVector1, IntervalTicks1) (ConflictVector2, IntervalTicks2) ...`的状态结点序列，括号左边数据为0-1状态序列向量对应的非负数，右边则为下次的启动距离

##### 示例
一个4段流水线，循环周期为7的预约表如下：
>1 0 0 0 0 0 1  
>0 1 0 0 0 1 0  
>0 0 1 0 1 0 0  
>0 0 0 1 0 0 0

得到的最优策略如下：
>(63, 1)  (42, 7)

即`{1，7}`为最优启动距离方案

#### 关键函数
`void DFS(StateNode* Stack, int* StackTop, const unsigned int InitVector, const unsigned int TickAmountSetting, unsigned int CurrentVector, StateNode PathsBuffer[][20], int* PathCount)`

##### 函数说明
**功能**：通过深度优先遍历有向状态图，找出所有闭环路径并保存在`PathsBuffer`中  
Stack：用来辅助深度优先遍历的栈空间  
StackTop：保存栈顶指针的位置，这个位置包含栈顶元素  
InitVector：最初生成的冲突向量  
TickAmountSetting：由用户输入的预约表循环周期数  
CurrentVector：包含了当前正要处理的冲突向量值  
PathsBuffer：如果遇到一条闭环，则存放在这个二维数组中
PathCount：记录数组中存放的闭环数量，也即下一条闭环路径应该存放的位置  
**返回值**：无

----
`int FindMinAvgIntervalTicks(StateNode PathBuffer[][20], int PathCount, const int TickAmountSetting)`

##### 函数说明
**功能**：找到平均启动距离最小的循环方案，其中包括吞吐率最小的比较，返回该方案在`PathsBuffer`中的位置  
PathBuffer：存放了所有启动循环方案的缓冲区  
PathCount：所有启动循环方案的数量  
TickAmountSetting：用户输入的初试启动循环周期  
**返回值**：最优循环方案在`PathsBuffer`中的位置

----
`int GenerateMinAvgIntervalsList(float* AvgTicksBuffer, int PathCount, int* Candidates)`

##### 函数说明
**功能**：找出平均启动距离最小的若干个启动循环方案（因为可能有重叠），并将其在`PathsBuffer`中所对应的行位置存放在`Candidates`数组中  
AvgTicksBuffer：存放每个启动循环的平均启动距离  
PathCount：启动循环方案的总个数  
Candidates：平均启动距离最小的若干循环所在位置都记录在这个数组中  
**返回值**：`Candidates`中的候选项数量