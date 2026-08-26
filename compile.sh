#!/bin/env sh

name='scb'

# not safe but meh
files=$(ls src/*.c)
progameFiles=$(ls src/program/*.c)


safety=" "
rule=" -g -D PROG_NAME=\"$name\" $safety "

cc="x86_64-w64-mingw32-gcc"
compileLine="$cc $rule -D=SETUP_EXTERN -Wall -Werror -Wextra  $files $progameFiles -o $name"

#

echo '|' $compileLine '|'

#

$compileLine