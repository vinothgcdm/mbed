#!/bin/bash

MAGIC_BYTS=0x55CC55CC
APP_START_ADDR=0x00020000

SRC_FILE=$1
DEST_FILE=out.bin
SRC_FILE_SIZE=$(stat -c%s "$SRC_FILE")
HEADER_SIZE=256

printf "0: %.8x" $MAGIC_BYTS | xxd -r -g0 > $DEST_FILE
printf "0: %.8x" $APP_START_ADDR | xxd -r -g0 >> $DEST_FILE
printf "0: %.8x" `expr $SRC_FILE_SIZE + $HEADER_SIZE` | xxd -r -g0 >> $DEST_FILE

dd if=$SRC_FILE of=$DEST_FILE bs=1 seek=$HEADER_SIZE

