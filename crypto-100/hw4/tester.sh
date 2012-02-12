#!/bin/bash
n=$1
[ "$n" == "" ] && n=1
echo -n "" > chk1
echo -n "" > chk_file
for i in $(eval echo {1..$n})
do
	ctx=$(tr -dc "a-zA-Z0-9~!@#$%^&*-_\"'" < /dev/urandom | head -c $RANDOM | cat -)
	echo $ctx >> chk_file
	echo -n $ctx | sha1sum >> chk1
done
cat chk_file | ./a.out > chk2
diff chk1 chk2
