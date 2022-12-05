# 介绍

这是ipLee为BJTU的体系结构课写的Cache模拟器。

包含：

* cache_monitor项目：即Cache模拟器主体。
* access_list_gen项目：用来生成访问地址的项目。



# 下载

```shell
git clone https://github.com/ipChrisLee/cache_monitor_for_BJTU_CA.git --recursive
```

这个项目使用了一些外部库，所以需要`--recursive`。



# 使用Cache monitor

```shell
cd cache_monitor_for_BJTU_CA
cmake -S . -B build
cd build && make && cd ..
build/cache_monitor/cache_monitor --in demo/stride131.txt --csv_out demo/stride131_res.csv
build/cache_monitor/cache_monitor --in demo/stride132.txt --csv_out demo/stride132_res.csv
```

之后就可以通过demo/stride131_res.csv和demo/stride132_res.csv看两个结果。

目前在macOS测试过没问题，Linux下理论上应该没问题。

注意：

1. 第一步是进入项目根目录。
2. 第三步是在默认你的CMake生成makefile的情况下的，如果你的CMake生成的toolchain不同，改成对应的就行。



# 关于地址访问生成器

我们可以用valgrind-lackey来追踪一个真实运行的程序的每次访存，但是valgrind-lackey生成的文件的格式不是我们需要的，access_list_gen子项目就可以做到转换。

**在Linux系统下**，先安装[valgrind](https://valgrind.org)，之后在access_list_gen/sh_src/下执行`gen.sh`，即可生成矩阵乘（mat_mut.c）和数组加（array_add.c）两个程序的内存访问数据（valgrind-lackey模式的，格式信息请参考[lackey源码注释](https://sourceware.org/git/?p=valgrind.git;a=blob;f=lackey/lk_main.c;h=e19b39f16f3414aef3141eeef8e229e0b73c7071;hb=HEAD)），分别为access_list_gen/sh_src/mat_mut_memory_trace_lackey.txt和access_list_gen/sh_src/array_add_memory_trace_lackey.txt。

这些数据和Cache monitor输入需要的数据格式不同，所以需要access_list_gen再处理：

```shell
cd cache_monitor_for_BJTU_CA
cmake -S . -B build
cd build && make && cd ..
buld/access_list_gen --lackey_mtrace_file_path access_list_gen/sh_src/mat_mut_memory_trace_lackey.txt -o demo/mat_mut_mtrace.txt
buld/access_list_gen --lackey_mtrace_file_path access_list_gen/sh_src/array_add_memory_trace_lackey.txt -o demo/array_add_mtrace.txt
```

之后就可以使用

```shell
build/cache_monitor/cache_monitor --in demo/mat_mut_mtrace.txt --csv_out demo/mat_mut_res.csv
build/cache_monitor/cache_monitor --in demo/array_add_mtrace.txt --csv_out demo/mat_mut_res.csv
```

来生成模拟结果。

