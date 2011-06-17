NMSGS=100
INTERVAL=10
MINDELAY=10
MAXDELAY=1000

REPETITIONS=20
WP=0

echo ===============================
echo Comparado o delivery otimista com e sem descarte de mensagens

while [ $WP -le $MAXDELAY ]
do
  echo ===============================
  echo Executando com wp = $WP
  REP=1
  while [ $REP -le $REPETITIONS ]
  do
    echo $REP
    ./mc $NMSGS $INTERVAL $MINDELAY $MAXDELAY $WP 0 >> mcnd.txt
    ./mc $NMSGS $INTERVAL $MINDELAY $MAXDELAY $WP 1 >> mcd.txt
    REP=$((REP+1))
  done
  echo Fim da execução com wp = $WP
  echo ===============================
  WP=$((WP+100))
  if [ $WP -le $MAXDELAY ]; then
    echo .
  fi
done
