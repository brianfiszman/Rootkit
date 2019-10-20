#!/usr/bin/bash

FILE=".ccls"
LINUX=$(uname -r)
CCLS="clang\n-isystem\n-Iinc\n-DMACRO\n-D__KERNEL__\n-DMODULE\n-I/lib/modules/$LINUX/build/include/\n-I/lib/modules/$LINUX/build/arch/x86/include/"
if [ -f $FILE ]; then
    echo "File exists."
else
    echo "Creating .ccls"
    echo "..."

    touch .ccls
    echo -e $CCLS >> $FILE
    echo "File created!"
fi

