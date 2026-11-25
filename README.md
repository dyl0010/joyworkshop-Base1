# SDL3 CMake Hello World

一个离线友好的 SDL3 + CMake + C++ hello-world 窗口项目。

## 目录

```text
.
├── CMakeLists.txt
├── SDL3HelloWorld.sublime-project
├── build.ps1
├── external/SDL3/README.md
└── src/main.cpp
```

## SDL3 离线依赖放置方式

当前环境不能从 GitHub clone，所以项目不会自动下载 SDL3。

推荐方式：手动下载 SDL3 development package，然后解压到：

```text
external/SDL3
```

该目录下需要能找到 `SDL3Config.cmake`，常见位置包括：

```text
external/SDL3/cmake/SDL3Config.cmake
external/SDL3/lib/cmake/SDL3/SDL3Config.cmake
```

也可以构建时指定 SDL3 路径：

```powershell
./build.ps1 -SDL3Root C:/path/to/SDL3
```

## 构建并运行

```powershell
./build.ps1
```

常用参数：

```powershell
./build.ps1 -NoRun
./build.ps1 -Clean
./build.ps1 -Config Release
./build.ps1 -SDL3Root C:/path/to/SDL3
```

## Sublime Text

1. 用 Sublime 打开 `SDL3HelloWorld.sublime-project`。
2. 选择 `Tools > Build System > Build and Run SDL3`。
3. 按 `Ctrl+B` 构建并运行。

窗口打开后按 `Esc` 或关闭窗口退出。
