#!/bin/env sh

name='scb'

# not safe but meh
files=$(ls src/*.c)
progameFiles=$(ls src/program/*.c)


safety=" -D=NAME_CHECK "
rule=" -g -D PROG_NAME=\"$name\""

cc="x86_64-w64-mingw32-gcc"
compileLine="cc $rule -o $name -D=SETUP_EXTERN -Wall -Werror -Wextra  $files $progameFiles"

#

echo '|' $compileLine '|'

#

$compileLine