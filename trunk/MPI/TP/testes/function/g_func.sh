P=1

while [ $P -le 28 ]
do
	./fgeneral $P
	P=$((P+1))
done
