#!/bin/sh

FILE=/proc/test_mod_file

cat ${FILE}

echo new foo > ${FILE}
cat ${FILE}

echo new bar > ${FILE}
cat ${FILE}

echo new baz > ${FILE}
cat ${FILE}

echo on 10 hoge > ${FILE}
cat ${FILE}

echo rm 10 > ${FILE}
cat ${FILE}

echo rep 0 meech > ${FILE}
cat ${FILE}

# ID      Data
# ID      Data
# 00000   foo
# ID      Data
# 00000   foo
# 00001   bar
# ID      Data
# 00000   foo
# 00001   bar
# 00002   baz
# ID      Data
# 00000   foo
# 00001   bar
# 00002   baz
# 00010   hoge
# ID      Data
# 00000   foo
# 00001   bar
# 00002   baz
# ID      Data
# 00000   meech
# 00001   bar
# 00002   baz
