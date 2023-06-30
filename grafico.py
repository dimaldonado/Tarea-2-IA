import pandas as pd
import matplotlib.pyplot as plt

# Leer el archivo en un dataframe de pandas
df = pd.read_csv('Episodios.txt', header=None, names=['Campo1', 'Campo2'])

# Graficar los datos
plt.plot(df['Campo1'], df['Campo2'])
plt.xlabel('Episodio')
plt.ylabel('Recompensa acumulada')
plt.title('Gráfico de Aprendizaje')
plt.show()