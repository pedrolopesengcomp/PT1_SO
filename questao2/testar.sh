#!/bin/bash
# Uso: ./testar.sh <versao 1|2> <num_A> <num_B> [segundos]
V=$1; NA=$2; NB=$3; T=${4:-10}
SUF=""; [ "$V" = "2" ] && SUF="2"
./limpa_filas > /dev/null
echo "=== Versao $V: $NA processo(s) A, $NB processo(s) B, ${T}s ==="
for i in $(seq $NB); do ./processo_b$SUF & done
sleep 0.3
for i in $(seq $NA); do ./processo_a$SUF & done
sleep $T
pkill -TERM -x processo_a$SUF
sleep 1.5
pkill -TERM -x processo_b$SUF
wait 2>/dev/null
./limpa_filas > /dev/null
