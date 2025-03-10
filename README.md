# xv6-homework

> 说明：本实验所有资源均来自 [MIT 6.828 课程](https://pdos.csail.mit.edu/6.828/2018/)，版本为 2018 年课程材料。
> xv6 为 [xv6第11版](https://pdos.csail.mit.edu/6.828/2018/xv6.html)

## Gitee相关镜像地址
- [xv6第11版源代码仓库](https://gitee.com/tjucs/xv6-public)
- [xv6第11版源代码PDF带行号版](../../raw/master/xv6-rev11.pdf)
- [xv6第11版教材PDF](../../raw/master/book-rev11.pdf)
- [6.828-qemu](https://gitee.com/tjucs/6.828-qemu)

## 实验列表
- [Homework: Threads and synchronize](thread.md)
- [Homework: xv6 system calls](syscall.md)
- [Homework: xv6 CPU alarm](alarm.md)
- [Homework: User-level threads](uthread.md)
- [Homework: xv6 lazy page allocation](alloc.md)
- [Homework: mmap()](mmap.md)
- [Homework: bigger files for xv6](bigfile.md)
- [Homework: xv6 log](log.md)

## xv6 运行环境设置
- 首先，你需要有一台Linux，无论是物理机还是虚拟机上的都可以。
- 虚拟机可以使用 VMware Workstation 或者 VirtualBox 或者 Windows 自带的 WSL。
- 以下步骤在 Ubuntu 22.04、Ubuntu 24.04 上试验可行，其他版本大同小异，但是可能会有一些新问题需要解决。

- 安装必要的工具：git, gcc, make, qemu
```
sudo apt install git
sudo apt install gcc
sudo apt install make
sudo apt install qemu-system-x86
```
- 以上过程中如果提示找不到软件包，可以使用下列命令，更新软件包列表，检查可用的软件包更新。
```
sudo apt update
```

- 下载 xv6 源代码。注意，我们的实验使用的是xv6-x86，不是xv6-riscv，对应 MIT 6.828 课程官网是 2018 年及以前版本，不是 2019 年及以后的版本。
```
git clone https://gitee.com/tjucs/xv6-public
cd xv6-public
make
```

- 注意：在 Ubuntu 24.04 中，如果编译报错，则修改 xv6-public 目录下的 Makefile，删除其中的 -Werror ，这样就能编译通过了。最后运行以下命令，启动xv6：

```
make qemu-nox
```
- 注意进了qemu以后怎么退出：先按 ctrl-a 松开再按 x