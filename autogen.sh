aclocal -I m4
autoheader
automake --add-missing --copy
autoconf

./configure --prefix=/usr
