/* Versao 2 - processo A: fila de pedidos compartilhada + fila de resposta PRIVADA
   (nome = PREFIXO_RESPOSTA + PID). Cada A so le respostas destinadas a ele. */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "comum.h"

int main(void) {
    pid_t eu = getpid();
    char nome_resp[64];
    snprintf(nome_resp, sizeof nome_resp, "%s%d", PREFIXO_RESPOSTA, eu);

    mqd_t pedidos   = abre_fila(FILA_PEDIDOS, O_WRONLY, sizeof(pedido_t));
    mqd_t respostas = abre_fila(nome_resp,    O_RDONLY, sizeof(resposta_t));
    if (pedidos == (mqd_t)-1 || respostas == (mqd_t)-1) return 1;

    instala_sinais();
    srandom(eu);
    long total = 0, erros = 0;

    printf("[A %d] iniciado, respondam em %s\n", eu, nome_resp);
    while (rodando) {
        pedido_t p = { random() % 100, random() % 100, eu };
        if (mq_send(pedidos, (const char *)&p, sizeof p, 0) == -1) {
            if (errno != EINTR) perror("mq_send");
            break;
        }

        resposta_t r;
        if (mq_receive(respostas, (char *)&r, sizeof r, NULL) == -1) {
            if (errno != EINTR) perror("mq_receive");
            break;
        }

        total++;
        if (r.n1 == p.n1 && r.n2 == p.n2) {
            printf("[A %d] %d + %d = %d  (B %d)\n", eu, p.n1, p.n2, r.soma, r.pid_b);
        } else {
            erros++;
            printf("[A %d] %d + %d = %d  <-- ERRADO! resposta era de %d + %d (B %d)\n",
                   eu, p.n1, p.n2, r.soma, r.n1, r.n2, r.pid_b);
        }
        fflush(stdout);
    }

    printf("[A %d] encerrando: %ld respostas, %ld erradas\n", eu, total, erros);
    mq_close(pedidos);
    mq_close(respostas);
    mq_unlink(nome_resp);            /* a fila de resposta e so deste A */
    return 0;
}
