# defines
EXECUTABLE=/home/tom/nectar/code/nectar-user/bin/nectar_board
TARGETIP=192.168.20.25
OLDPROGRAMPATH=/home/debian/session/nectar_board
# kill gdbserver on target
ssh debian@$TARGETIP pkill gdbserver
# remove old executable on target
#ssh debian@$TARGETIP rm $OLDPROGRAMPATH
# copy over new executable
#scp $EXECUTABLE debian@$TARGETIP:$OLDPROGRAMPATH
# start gdb on target (IS ONE LONG COMMAND)
ssh -n -f debian@$TARGETIP "sh -c 'cd ~/session; nohup gdbserver localhost:2345 nectar_board > /dev/null 2>&1 &'"