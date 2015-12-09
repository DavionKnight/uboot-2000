#!/bin/sh
export ARCH=powerpc


export PATH=/opt/eldk42/usr/bin:$PATH

export CROSS_COMPILE=ppc_85xxDP-


case "$1" in
    distclean)
        make distclean
        ;;
    clean)
        make clean
        ;;
    cpu-p1020)
        #make P1020RDB-PC_NAND
        make distclean
        make clean
        make CPU_P1020_NAND
        cp u-boot-nand.bin /tftpboot/u-boot-2000.bin
	cp u-boot.bin /mnt/hgfs/D/documents/P1020/p1020-firmware/u-boot-2000ram.bin 
        ;;
    all)
        make all
        cp u-boot.bin /tftpboot/u-boot-2000-ram.bin
        ;;
    *)
        echo  $"Usage: $0 {clean |distclean |cpu-p1020| all}"
        exit 1
esac
exit $?

#cp u-boot.bin ~/tftpboot/u-bootram.bin
#cp u-boot-nand.bin ~/tftpboot/
#cp ./u-boot.bin  /home/sxl/tftpboot/lmxe_u-boot.bin 
#cp ./u-boot-nand.bin /home/liuyk/tftpboot/

