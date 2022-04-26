#!/bin/bash
rm -f a.out *.JPG
for src in $* ;do
	echo $src
	bash labtest.sh $src 
	sleep 1
done
