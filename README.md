# dfs-bfs-maze-solver  
solve maze problem by dfs and bfs method  
基于DFS和BFS算法的迷宫求解过程 

A text file that stores a matrix of `m*n` size, with 1 representing the wall and 0 representing the passable path, finding the path from the upper left corner to the lower right corner   
Based on DFS and BFS, you can flexibly select the 'main' function, which is enabled by default  
一个文本文件，存放mxn大小的矩阵，以1代表墙壁，0代表可通行的路，求从左上角到右下角的路径 
基于DFS和BFS，可以在`main`函数灵活选择，默认都启用 

### 说明 
- 编写平台为Windows11和Clion 2023.2.2，由于一些未知原因，Clion改变编码方式后出现问题，故此处在`CMakeLists.txt`文件定义了宏`ON_CLION_CONSOLE`，若发现Clion控制台出现了无法读入重定向输入的情况，请照此编写cmake
- 部分地方有待优化
- 迷宫起始位置和终点位置在代码写死（认为第二行第二列和倒数第二行倒数第二列为起始位置和终点位置），可以自行修改，且操作较为简单，不再赘述
### Note
- The writing platform is Windows 11 and Clion 2023.2.2, due to some unknown reasons, Clion changed the encoding method after the problem, so here in the 'CMakeLists.txt' file defines the macro 'ON_CLION_CONSOLE', if you find that the Cleion console cannot read the redirected input, please write cmake accordingly
- Some places need to be optimized
- The maze start position and end position are dead in the code (think that the second column of the second row and the penultimate row are the start position and end position), which can be modified by themselves, and the operation is relatively simple, so I will not repeat it
### 特点
- 着重于输入文件格式的检测，严格判断换行方式
- 结果输出直观
### Features
- Focus on the detection of input file formats, strict judgment of line breaks
- Intuitive output of results  

== 若有其它问题，请提交issue ==  
== If you have any other questions, please submit issue ==
