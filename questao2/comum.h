#ifndef COMUM_H
#define COMUM_H

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>

#define FILA_PEDIDOS     "/fila-tp1so-pedidos"
#define FILA_RESPOSTAS   "/fila-tp1so-respostas"   /* usada so na versao 1 */
#define PREFIXO_RESPOSTA "/fila-tp1so-resp-"       /* versao 2: + PID do A  */
#define MAX_MSGS 10

/* Mensagem A -> B */
typedef struct {
    int   n1, n2;
    pid_t pid_a;          /* quem pediu (usado na versao 2 para achar a fila de volta) */
} pedido_t;

/* Mensagem B -> A (devolve n1 e n2 para o A conferir se a resposta e dele) */
typedef struct {
    int   n1, n2, soma;
    pid_t pid_b;          /* quem respondeu */
} resposta_t;


static volatile sig_atomic_t rodando = 1;

static inline void trata_sinal(int sig) { (void)sig; rodando = 0; }

static inline void instala_sinais(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = trata_sinal;   
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

static inline mqd_t abre_fila(const char *nome, int modo, long tam) {
    struct mq_attr attr;
    memset(&attr, 0, sizeof attr);
    attr.mq_maxmsg  = MAX_MSGS;
    attr.mq_msgsize = tam;

    mqd_t q = mq_open(nome, modo | O_CREAT, 0666, &attr);
    if (q == (mqd_t)-1) { perror(nome); return q; }

    mq_getattr(q, &attr);
    if (attr.mq_msgsize != tam) {
        fprintf(stderr, "Fila %s ja existe com mq_msgsize=%ld (esperado %ld). "
                        "Rode 'make limpar'.\n", nome, attr.mq_msgsize, tam);
        mq_close(q);
        return (mqd_t)-1;
    }
    return q;
}

#endif
