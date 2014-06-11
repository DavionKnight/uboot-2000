#!/bin/sh

#export PATH=/home/zhangqi/opt/eldk53/powerpc/sysroots/i686-eldk-linux/usr/bin:/home/zhangqi/opt/eldk53/powerpc/sysroots/i686-eldk-linux/usr/bin/powerpc-linux:$PATH

#export PATH=/home/zhangqi/opt/eldk42/usr/bin:$PATH
source /opt/fsl/1.2/environment-setup-ppce500v2-fsl-linux-gnuspe
export ARCH=powerpc

#export CROSS_COMPILE=ppc_8xx-


#export LIBC=/home/zhangqi/opt/eldk53/powerpc/sysroots/i686-eldk-linux/lib


#export PATH=$PATH:/opt/freescale/usr/local/gcc-4.1.78-eglibc-2.5.78-1/powerpc-e300c3-linux-gnu/bin
#export CROSS_COMPILE=powerpc-e300c3-linux-gnu-

#export PATH=/home/sxl/mpc/mpc_install/usr/bin:/home/sxl/mpc/mpc_install/bin:/usr/include:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
#export CROSS_COMPILE=/opt/eldk4.2_ppc/usr/bin/ppc_85xxDP-
export CROSS_COMPILE=powerpc-fsl-linux-gnuspe-

case "$1" in
    distclean)
        make distclean
        ;;
    clean)
        make clean
        ;;
    cpu-p1020)
        #make P1020RDB-PC_NAND
        make CPU_P1020_NAND
        ;;
    all)
        make all
        ;;
    *)
        echo  $"Usage: $0 {clean |distclean |cpu-p1020| all}"
        exit 1
esac
exit $?

cp u-boot.bin ~/tftpboot/u-bootram.bin
#cp u-boot-nand.bin ~/tftpboot/
#cp ./u-boot.bin  /home/sxl/tftpboot/lmxe_u-boot.bin 
#cp ./u-boot-nand.bin /home/liuyk/tftpboot/

