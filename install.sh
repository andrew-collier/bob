#!/bin/zsh

[ ! $(which zsh) ] && echo "Missing dependency \"zsh\"" && exit 1
[ ! $(which less) ] && echo "Missing dependency \"less\"" && exit 1

make all
install -Dm 755 "./bill" "/usr/local/bin/kjv"
install -Dm 755 "./bob" "/usr/local/bin/kjv"
install -Dm 444 "./bob.1" "/usr/local/man/man1/kjv.1"
mandb -q
