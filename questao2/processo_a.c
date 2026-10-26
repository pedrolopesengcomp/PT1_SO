/* Versao 1 - processo A: duas filas compartilhadas (pedidos e respostas) */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "comum.h"

int main(void) {
    mqd_t pedidos   = abre_fila(FILA_PEDIDOS,   O_WRONLY, sizeof(pedido_t));
    mqd_t respostas = abre_fila(FILA_RESPOSTAS, O_RDONLY, sizeof(resposta_t));
    if (pedidos == (mqd_t)-1 || respostas == (mqd_t)-1) return 1;

    instala_sinais();
    pid_t eu = getpid();
    srandom(eu);                     /* semente diferente para cada A */
    long total = 0, erros = 0;

    printf("[A %d] iniciado\n", eu);
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
    return 0;
}
