#!/bin/sh

export PATH=/home/freddie/Compiler_PowerPC/usr/bin:/home/freddie/Compiler_PowerPC/bin:$PATH
export PATH=/opt/eldk42/usr/bin:/opt/eldk42/bin:$PATH
mkimage -A ppc -O linux -T script -C none -a 0 -e 0 -n "set envs but address script" -d ./set_env.script ./set_env.uscr
cp ./set_env.uscr ../Release_version
