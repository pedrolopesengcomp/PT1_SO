/* Versao 1 - processo B: le da fila de pedidos e responde na fila de respostas */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "comum.h"

int main(void) {
    mqd_t pedidos   = abre_fila(FILA_PEDIDOS,   O_RDONLY, sizeof(pedido_t));
    mqd_t respostas = abre_fila(FILA_RESPOSTAS, O_WRONLY, sizeof(resposta_t));
    if (pedidos == (mqd_t)-1 || respostas == (mqd_t)-1) return 1;

    instala_sinais();
    pid_t eu = getpid();
    printf("[B %d] aguardando pedidos\n", eu);

    while (rodando) {
        pedido_t p;
        if (mq_receive(pedidos, (char *)&p, sizeof p, NULL) == -1) {
            if (errno != EINTR) perror("mq_receive");
            break;
        }

        resposta_t r = { p.n1, p.n2, p.n1 + p.n2, eu };
        printf("[B %d] recebeu %d e %d de A %d\n", eu, p.n1, p.n2, p.pid_a);
        fflush(stdout);

        sleep(1);

        if (mq_send(respostas, (const char *)&r, sizeof r, 0) == -1) {
            if (errno != EINTR) perror("mq_send");
            break;
        }
    }

    printf("[B %d] encerrando\n", eu);
    mq_close(pedidos);
    mq_close(respostas);
    return 0;
}
