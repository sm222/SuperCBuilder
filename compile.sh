#!/bin/sh

name='base'

files=$(ls src/*.c)


safety=" -D NAME_CHECK "
rule=" -g -D PROG_NAME=\"$name\" $safety "

compileLine="cc $rule -Wall -Werror -Wextra  $files  -o $name"

#

echo '|' $compileLine '|'

#

$compileLine