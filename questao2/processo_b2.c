/* Versao 2 - processo B: responde na fila privada do A que fez o pedido */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "comum.h"

int main(void) {
    mqd_t pedidos = abre_fila(FILA_PEDIDOS, O_RDONLY, sizeof(pedido_t));
    if (pedidos == (mqd_t)-1) return 1;

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

        /* descobre a fila de resposta pelo PID que veio no pedido */
        char nome_resp[64];
        snprintf(nome_resp, sizeof nome_resp, "%s%d", PREFIXO_RESPOSTA, p.pid_a);
        mqd_t resp = mq_open(nome_resp, O_WRONLY);   /* sem O_CREAT: o A ja criou */
        if (resp == (mqd_t)-1) {
            fprintf(stderr, "[B %d] A %d nao existe mais, descartando\n", eu, p.pid_a);
            continue;
        }
        if (mq_send(resp, (const char *)&r, sizeof r, 0) == -1 && errno != EINTR)
            perror("mq_send");
        mq_close(resp);
    }

    printf("[B %d] encerrando\n", eu);
    mq_close(pedidos);
    return 0;
}
