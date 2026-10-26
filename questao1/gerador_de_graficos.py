import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

sns.set_theme(style="whitegrid")
plt.rcParams.update({"font.size": 11})

try:
  df = pd.read_csv("resultados.csv", header=None)
  df.columns = ["Plataforma", "Num_Threads", "Tempo"]
except FileNotFoundError:
  print("Erro: Arquivo 'resultados.csv' não encontrado.")
  exit(1)

estatisticas = (
    df.groupby(["Plataforma", "Num_Threads"])["Tempo"]
    .agg(Media="mean", Desvio_Padrao="std", Count="count")
    .reset_index()
)

estatisticas["CV_Percentual"] = (
    estatisticas["Desvio_Padrao"] / estatisticas["Media"]
) * 100
estatisticas["Desvio_Padrao"] = estatisticas["Desvio_Padrao"].fillna(0)
estatisticas["CV_Percentual"] = estatisticas["CV_Percentual"].fillna(0)

print("=== TABELA DE RESULTADOS ESTATÍSTICOS ===")
print(
    estatisticas[
        ["Plataforma", "Num_Threads", "Media", "Desvio_Padrao", "CV_Percentual"]
    ].to_string(index=False)
)
print("=========================================\n")

plataformas = estatisticas["Plataforma"].unique()

for plat in plataformas:
  df_plat = estatisticas[estatisticas["Plataforma"] == plat]

  plt.figure(figsize=(8, 5))
  plt.plot(
      df_plat["Num_Threads"],
      df_plat["Media"],
      marker="o",
      linewidth=2.5,
      markersize=8,
      color="#1f77b4",
      label=f"Tempo Médio ({plat})",
  )

  plt.title(
      f"Curva de Tempo Médio de Execução — {plat}",
      fontsize=14,
      fontweight="bold",
  )
  plt.xlabel("Número de Threads (N)", fontsize=12)
  plt.ylabel("Tempo de Execução (segundos)", fontsize=12)
  plt.xticks(df_plat["Num_Threads"])
  plt.grid(True, linestyle="--", alpha=0.7)
  plt.tight_layout()

  nome_arquivo = f"grafico_tempo_{plat.lower().replace(' ', '_')}.png"
  plt.savefig(nome_arquivo, dpi=300)
  plt.close()

plt.figure(figsize=(9, 6))
cores = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"]

for i, plat in enumerate(plataformas):
  df_plat = estatisticas[estatisticas["Plataforma"] == plat].sort_values(
      "Num_Threads"
  )
  tempo_t1 = df_plat.loc[df_plat["Num_Threads"] == 1, "Media"].values

  if len(tempo_t1) == 0:
    continue

  base_t1 = tempo_t1[0]
  df_plat["Normalizado"] = df_plat["Media"] / base_t1

  plt.plot(
      df_plat["Num_Threads"],
      df_plat["Normalizado"],
      marker="s",
      linewidth=2.5,
      markersize=7,
      label=f"{plat} (Normalizado)",
      color=cores[i % len(cores)],
  )

plt.axhline(
    y=1.0,
    color="gray",
    linestyle=":",
    linewidth=1.5,
    label="Linha de Referência (1 Thread = 1.0)",
)
plt.title(
    "Desempenho Relativo Normalizado entre Plataformas",
    fontsize=14,
    fontweight="bold",
)
plt.xlabel("Número de Threads (N)", fontsize=12)
plt.ylabel("Tempo Normalizado  [ T(N) / T(1) ]", fontsize=12)
plt.xticks(estatisticas["Num_Threads"].unique())
plt.legend(fontsize=11)
plt.grid(True, linestyle="--", alpha=0.7)
plt.tight_layout()

plt.savefig("grafico_consolidado_normalizado.png", dpi=300)
plt.close()

print("Processamento concluído com sucesso!")