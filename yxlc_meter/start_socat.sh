kill -9 $(pgrep -f "socat")
sleep 3
nohup socat TCP-LISTEN:60137,reuseaddr,fork TCP:192.168.8.137:22 &
nohup socat TCP-LISTEN:68137,reuseaddr,fork TCP:192.168.8.137:8000 &
nohup socat TCP-LISTEN:60138,reuseaddr,fork TCP:192.168.8.138:22 &
nohup socat TCP-LISTEN:68138,reuseaddr,fork TCP:192.168.8.138:8000 &