#!/bin/bash

# input
rm -f a.out *.JPG *.html
src=$1;

echo $1
echo $1 >> /dev/stderr

succcnt=0
if [[ "$src" =~ ".cpp" ]];
then
	g++ $src
	sleep 1
	sed 's/^/.\/a.out /' lab.in | bash  - &> test.out
elif [[ $src =~ ".c" ]];
then
	gcc $src 
	sleep 1
	sed 's/^/.\/a.out /' lab.in | bash  - &> test.out
elif [[ $src =~ ".java" ]];
then
	javac $src 
	sleep 1
	cname=$(ls *.class | sed 's/.class//')
	sed 's/^/java '$cname' /' lab.in | bash -x - &> test.out
elif [[ $src =~ ".py" ]];
then
	sed 's/^/python3 '$src' /' lab.in | bash -x - &> test.out
else
	echo "not supported" $src
	exit
fi

if [[ $? == 0 ]];
then
	let "succcnt=succcnt+1"
fi

grep "404" test.out
if [[ $? == 0 ]];
then
	let "succcnt=succcnt+2"
fi

grep "unknown host" test.out
if [[ $? == 0 ]];
then
	let "succcnt=succcnt+16"
fi


echo ""
echo "Binary File Compare"
cmp palladio.JPG palladio.JPG.org

if [[ $? == 0 ]];
then
	let "succcnt=succcnt+4"
fi

echo ""
echo "Text File Compare"

diff index.html index.html.org

if [[ $? == 0 ]];
then
	let "succcnt=succcnt+8"
fi



echo "Result: " $succcnt $src

sleep 1
rm -f a.out *.JPG *.html *.class test.out


