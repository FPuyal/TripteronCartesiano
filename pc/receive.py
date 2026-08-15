"""Recepcion de datos del robot Tripteron (USB CDC).

Escucha el puerto serie y acumula bytes hasta recibir el byte de
finalizacion 0xFF. En ese momento vuelca lo recibido (sin el byte de
finalizacion) a un fichero log-<fecha>.txt en esta misma carpeta y
sigue escuchando para el siguiente mensaje.

Uso:
    py receive.py
"""

import datetime
import os
import sys
import time

import serial

PORT = 'COM6'
BAUDRATE = 115200
END_BYTE = 0xFF
READY_BYTE = b'R'
RETRY_DELAY_S = 0.5

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LOGS_DIR = os.path.join(SCRIPT_DIR, 'logs')


def make_log_path():
    os.makedirs(LOGS_DIR, exist_ok=True)
    timestamp = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
    return os.path.join(LOGS_DIR, f'log-{timestamp}.txt')


def open_port():
    """Reintenta abrir el puerto hasta que exista (p.ej. hasta que se
    conecte/flashee la STM32 y aparezca el COM)."""
    while True:
        try:
            return serial.Serial(PORT, BAUDRATE, timeout=1)
        except serial.SerialException:
            time.sleep(RETRY_DELAY_S)


def main():
    buffer = bytearray()

    print(f'Esperando {PORT}...')
    with open_port() as ser:
        print(f'Conectado a {PORT} @ {BAUDRATE}')
        # Handshake: descarta lo que hubiera y avisa a la STM32 de que ya
        # estamos listos para leer. La STM32 envia en cuanto reciba este byte.
        ser.reset_input_buffer()
        ser.write(READY_BYTE)
        try:
            while True:
                chunk = ser.read(ser.in_waiting or 1)
                if not chunk:
                    continue

                for byte in chunk:
                    if byte == END_BYTE:
                        log_path = make_log_path()
                        with open(log_path, 'wb') as f:
                            f.write(bytes(buffer))
                        print(f'{len(buffer)} bytes -> {log_path}')
                        buffer.clear()
                    else:
                        buffer.append(byte)
        except KeyboardInterrupt:
            print('\nInterrumpido.')
            sys.exit(0)


if __name__ == '__main__':
    main()
