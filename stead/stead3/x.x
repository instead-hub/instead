#!/bin/sh
for f in *.lua; do
	echo '".\symbian_gamedata\stead\stead3\\'$f'" -"!:\data\instead\stead\stead3\\'$f'"'
done

for f in ext/*.lua; do
	f=`echo $f | sed -e 's|/|\\\\\\\|g'`
	echo '".\symbian_gamedata\stead\stead3\'"$f"'" -"!:\data\instead\stead\stead3\'"$f"'"'
done
