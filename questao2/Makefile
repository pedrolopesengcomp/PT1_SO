CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDLIBS  = -lrt
PROGS   = processo_a processo_b processo_a2 processo_b2 limpa_filas

all: $(PROGS)

%: %.c comum.h
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

# remove filas que sobraram de execucoes anteriores
limpar: limpa_filas
	./limpa_filas

clean: limpar
	rm -f $(PROGS)

.PHONY: all limpar clean
