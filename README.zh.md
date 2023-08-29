# Ascend Tensorpipe

#### 介绍
开源仓Tensorpipe基于华为Ascend pytorch/torch_npu的适配。

#### 安装教程
在linux shell中指定位置运行：
```
$ git clone --recursive https://gitee.com/ascend/Tensorpipe.git
$ cd Tensorpipe
$ mkdir build
$ cd build
$ cmake ../ -GNinja
$ ninja
```
即可开始编译

#### 使用说明
根据功能需求，在编译之前选择性启动./build/Options.cmake中Option features下的四个编译选项
```
# Optional features
option(TP_BUILD_BENCHMARK "Build benchmarks" OFF)
option(TP_BUILD_MISC "Build misc tools" OFF)
option(TP_BUILD_PYTHON "Build python bindings" OFF)
option(TP_BUILD_TESTING "Build tests" OFF)
```

#### 注意事项
倘若要将TP_BUILD_PYTHON设置为ON，在cmake时需要额外添加：**-DBUILD_SHARED_LIBS=ON**编译选项，不然生成的.a文件无法被pybind11获取。

倘若要将TP_BUILD_MISC设置为ON，需要在Linux系统中下载**llvm-12**，**clang-12**和**libclang-12-dev**。三个包的版本必须相同（12），且不能使用14及以后的版本。

倘若要将TP_BUILD_TESTING设置为ON，需要在cmake时添加: **-DTP_ENABLE_SHM=OFF -DTP_ENABLE_IBV=OFF -DTP_ENABLE_CMA=OFF**（目前暂不支持这三个功能的测试），并在编译完成后的build子目录中运行：**./tensorpipe/test/tensorpipe_test**