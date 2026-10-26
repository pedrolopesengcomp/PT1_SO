/* Remove as filas compartilhadas (util se um processo morreu sem limpar) */
#include <stdio.h>
#include <mqueue.h>
#include "comum.h"

int main(void) {
    if (mq_unlink(FILA_PEDIDOS)   == 0) printf("removida %s\n", FILA_PEDIDOS);
    if (mq_unlink(FILA_RESPOSTAS) == 0) printf("removida %s\n", FILA_RESPOSTAS);
    return 0;
}
