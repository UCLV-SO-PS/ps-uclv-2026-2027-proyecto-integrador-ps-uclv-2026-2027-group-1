# Proyecto Final 2026-2027 - Servidor Concurrente TCP de Calificaciones IMDb

**Asignatura:** Programación de Sistemas  
**Curso Escolar:** 2026–2027  

## Descripción

Servidor concurrente TCP en C bajo arquitectura cliente-servidor que escucha en el puerto 6666. Proporciona información sobre las calificaciones IMDb de los episodios de la serie "A Knight of the Seven Kingdoms".

## Integrantes

-Alex David Castañedo Canto
-Frank Javier Garcia Rodriguez
-Frank Roberto Marin Rodriguez

## Archivos del proyecto

- `Makefile` - Archivo de automatización de compilación
- `libmonitor.a` - Biblioteca estática con base de datos y utilidades
- `server.c` / `server` - Servidor concurrente TCP
- `client.c` / `client` - Cliente de prueba
- `monitor.h` - Cabecera con estructuras y funciones
- `bd.c` - Implementación de la base de datos
- `utils.c` - Funciones de formato
- `README.md` - Documentación
- `screenshot/` - Capturas de pantalla del funcionamiento

## Base de Datos

| Código | Episodio/Título | Puntuación IMDB | Votos Registrados |
|--------|------------------|-----------------|-------------------|
| S01E01 | The Hedge Knight | 9.2 | 45,120 |
| S01E02 | Hard Salt Beef  | 8.8 | 38,900 |
| S01E03 | The Squire      | 9.5 | 52,400 |
| S01E04 | Seven           | 9.1 | 41,200 |
| S01E05 | In the Name of the Mother | 9.7 | 68,300 |
| S01E06 | The Morrow      | 9.4 | 49,800 |

## Compilación

Para compilar todo el proyecto:

```bash
make all
```

Esto generará:
- `libmonitor.a` (biblioteca estática)
- `server` (ejecutable del servidor)
- `client` (ejecutable del cliente)

Para limpiar archivos objeto, biblioteca y ejecutables:

```bash
make clean
```

## Ejecución

### 1. Iniciar el servidor

```bash
./server
```

El servidor escuchará en el puerto 6666. Cuando un cliente se conecte, el proceso padre imprimirá:
```
IP <dirección_ip> PID <pid_hijo>
```

### 2. Ejecutar el cliente

En otra terminal:

```bash
./client S01E01
```

Reemplazar `S01E01` por el código del episodio deseado (exactamente 6 caracteres).

### 3. Prueba automática (--test-run)

```bash
./server --test-run
```

Inicializa la base de datos, valida el socket TCP y finaliza con código 0 si es exitoso.

## Decisiones de Diseño

- **Concurrencia:** Se utiliza `fork()` para crear un proceso hijo por cada conexión aceptada. El proceso padre continúa aceptando nuevas conexiones.
- **Comunicación IPC:** Se utiliza una tubería sin nombre (`pipe()`) creada antes de `fork()` para que el proceso hijo envíe la dirección IP del cliente al proceso padre.
- **Manejo de señales:**
  - `SIGINT` (CTRL+C): Ignorado mediante `sigaction()` para que el servidor no termine con esta señal.
  - `SIGCHLD`: Capturado con `sigaction()` y los hijos finalizados son cosechados con `waitpid(-1, NULL, WNOHANG)` para evitar procesos zombie.
- **Biblioteca estática:** La base de datos y las utilidades de búsqueda/formato se empaquetan en `libmonitor.a` usando `ar rcs`.
- **Sockets TCP:** Comunicación cliente-servidor mediante sockets TCP en el puerto 6666.

## Ejemplos de uso

```bash
# Terminal 1
./server

# Terminal 2
./client S01E05
```

Salida esperada:
```
Código: S01E05
Episodio/Título: In the Name of the Mother
Puntuación IMDB: 9.7
Votos Registrados: 68,300
```
