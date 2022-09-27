#!/bin/zsh

[ ! $(which zsh) ] && echo "Missing dependency \"zsh\"" && exit 1
[ ! $(which less) ] && echo "Missing dependency \"less\"" && exit 1
[ ! $(which dos2unix) ] && echo "Missing dependency \"dos2unix\"" && exit 1

make all

install -Dm 755 "./bill" "/usr/local/bin/bill"
install -Dm 755 "./bob" "/usr/local/bin/bob"
install -Dm 444 "./bob.1" "/usr/local/man/man1/bob.1"
mandb -q
