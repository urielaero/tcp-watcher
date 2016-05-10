cat << EOF > /tmp/alarmevent.call
Channel: SIP/uicab
Application: Festival
Data: El estado actual del sensor $1 es encendido
EOF
mv /tmp/alarmevent.call /var/spool/asterisk/outgoing/
