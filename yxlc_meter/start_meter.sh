kill -9 $(pgrep -f "meter")
sleep 3
./meter &
tail -f /var/log/syslog