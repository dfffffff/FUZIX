export CROSS_AS=m6811-elf-as
export CROSS_LD= m6811-elf-ld
export CROSS_CC = m6811-elf-gcc
export CROSS_CCOPTS=-mshort -c -Wall -Os -I$(ROOT_DIR)/cpu-68hc11 -I$(ROOT_DIR)/platform-$(TARGET) -I$(ROOT_DIR)/include
export CROSS_CC_SEG1=
export CROSS_CC_SEG2=
export CROSS_CC_SEG3=
export CROSS_CC_SEGDISC=
export CROSS_CC_VIDEO=
export ASOPTS=
export ASMEXT = .s
export BINEXT = .o
export BITS=16
