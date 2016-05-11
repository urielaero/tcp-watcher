cat << EOF > /tmp/alarmevent.call
Channel: SIP/uicab
Application: Festival
Data: El estado actual del sensor $1 es encendido 
CallerID: "sensor $1" <606$2>
EOF
mv /tmp/alarmevent.call /var/spool/asterisk/outgoing/
