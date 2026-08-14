"""Cliente PC del robot Tripteron (USB CDC).

Uso:
    py robot.py home
    py robot.py stop
    py robot.py move X Y Z

Flujo: abre el puerto, envia el comando, y registra la telemetria que llega
mientras el robot ejecuta hasta recibir la trama END. La telemetria se guarda
en un CSV nuevo por ejecucion.

Trama: [0xAA][ID][PAYLOAD]. Floats little-endian, sin LEN ni CRC.
"""

import os
import struct
import sys
import time
from datetime import datetime

import serial

LOG_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'logs')

PORT = 'COM6'
BAUDRATE = 115200
READ_TIMEOUT = 5.0  # seguridad: si no llega nada en este tiempo, corta

START = 0xAA

ID_HOME = ord('H')
ID_STOP = ord('S')
ID_MOVE = ord('M')
ID_TELEMETRY = ord('T')
ID_END = ord('E')

PAYLOAD_SIZE = {
    ID_TELEMETRY: 32,
    ID_END: 0,
}

CSV_HEADER = 't,posX,posY,posZ,velX,velY,velZ,execPos,execVel\n'


def build_command(args):
    cmd = args[0].lower()
    if cmd == 'home':
        return bytes([START, ID_HOME])
    if cmd == 'stop':
        return bytes([START, ID_STOP])
    if cmd == 'move':
        x, y, z = (float(v) for v in args[1:4])
        return bytes([START, ID_MOVE]) + struct.pack('<fff', x, y, z)
    raise ValueError(f'Comando desconocido: {cmd}')


def read_byte(ser):
    b = ser.read(1)
    return b[0] if b else None


def capture(ser):
    """Lee tramas hasta recibir END. Escribe la telemetria en un CSV."""
    os.makedirs(LOG_DIR, exist_ok=True)
    filename = os.path.join(LOG_DIR, 'telemetry_' + datetime.now().strftime('%Y%m%d_%H%M%S') + '.csv')
    f = None
    t0 = None
    samples = 0

    while True:
        # sincronizar al inicio de trama
        b = read_byte(ser)
        if b is None:
            print('Timeout sin datos.')
            break
        if b != START:
            continue

        mid = read_byte(ser)
        if mid is None:
            break

        if mid == ID_END:
            print('END recibido.')
            break

        if mid == ID_TELEMETRY:
            payload = ser.read(PAYLOAD_SIZE[ID_TELEMETRY])
            if len(payload) != PAYLOAD_SIZE[ID_TELEMETRY]:
                break
            v = struct.unpack('<8f', payload)
            now = time.perf_counter()
            if f is None:
                f = open(filename, 'w')
                f.write(CSV_HEADER)
                t0 = now
            f.write(','.join(f'{x:.6f}' for x in ((now - t0),) + v) + '\n')
            samples += 1

    if f:
        f.close()
        print(f'{samples} muestras guardadas en {filename}')
    else:
        print('Sin telemetria.')


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    frame = build_command(sys.argv[1:])
    with serial.Serial(PORT, BAUDRATE, timeout=READ_TIMEOUT) as ser:
        ser.write(frame)
        capture(ser)


if __name__ == '__main__':
    main()
