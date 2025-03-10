# xv6-homework

> 说明：本作业所有资源均来自 [MIT 6.828 课程](https://pdos.csail.mit.edu/6.828/2018/)，版本为 2018 年课程材料。
> xv6 为 [xv6第11版](https://pdos.csail.mit.edu/6.828/2018/xv6.html)

## Gitee相关镜像仓库：
- [xv6第11版源代码](https://gitee.com/tjucs/xv6-public)
- [6.828-qemu](https://gitee.com/tjucs/6.828-qemu)

## 作业列表
- [Homework: Threads and synchronize](thread.md)
- [Homework: xv6 system calls](syscall.md)
- [Homework: xv6 CPU alarm](alarm.md)
- [Homework: User-level threads](uthread.md)
- [Homework: xv6 lazy page allocation](alloc.md)
- [Homework: mmap()](mmap.md)
- [Homework: bigger files for xv6](bigfile.c)
- [Homework: xv6 log](log.c)

## xv6 运行环境设置
- 首先，你需要有一台Linux，无论是物理机还是虚拟机上的都可以，虚拟机可以使用 VMware Workstation 或者 VirtualBox 或者 Windows 自带的 WSL。以下 Linux 版本以 Ubuntu 24.04 为例，其他版本大同小异，但是可能会有一些新问题需要解决。

- 安装必要的工具：gcc, make, qemu
```
sudo apt update
sudo apt install gcc
sudo apt install make
sudo apt install qemu-system-x86
```