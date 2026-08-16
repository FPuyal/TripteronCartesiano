"""Monitor del robot Tripteron (USB CDC): envia comandos y recibe telemetria
desde un unico proceso, evitando el choque por el acceso exclusivo al puerto COM.

Un hilo en segundo plano escucha el puerto y vuelca cada mensaje (hasta el byte
de finalizacion 0xFF) a un fichero log-<fecha>.txt en pc/logs/. El hilo principal
lee comandos por teclado y los envia a la STM32.

Uso:
    py monitor.py

Comandos (se teclean en la consola):
    H                     -> Home
    R                     -> Reset
    M x y z [x y z ...]   -> Move (uno o varios segmentos)
    q                     -> salir

Ejemplo:
    M 100 100 20 200 120 40
"""

import datetime
import os
import sys
import threading
import time

import serial

PORT = 'COM6'
BAUDRATE = 115200
END_BYTE = 0xFF
READY_BYTE = b'R'
RETRY_DELAY_S = 0.5

VALID_COMMAND_IDS = {'H', 'R', 'M'}

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


def build_command(args):
    """Construye el mensaje de texto que espera Robot::ParseCommand.

    args = ['M', '100', '100', '20', '200', '120', '40']
    -> b'M 100 100 20 200 120 40\\0'
    """
    cmd_id = args[0].upper()
    if cmd_id not in VALID_COMMAND_IDS:
        raise ValueError(f'Comando desconocido: {args[0]}')
    message = ' '.join([cmd_id] + list(args[1:]))
    return message.encode('ascii') + b'\0'


def receiver_loop(ser, stop_event):
    """Escucha el puerto y vuelca cada mensaje (hasta 0xFF) a un log."""
    buffer = bytearray()
    while not stop_event.is_set():
        try:
            chunk = ser.read(ser.in_waiting or 1)
        except serial.SerialException:
            break
        if not chunk:
            continue
        for byte in chunk:
            if byte == END_BYTE:
                log_path = make_log_path()
                # Un frame por linea: salto antes de cada etiqueta [MC.
                data = bytes(buffer).replace(b'[MC', b'\r\n[MC').lstrip(b'\r\n')
                with open(log_path, 'wb') as f:
                    f.write(data)
                print(f'\n{len(buffer)} bytes -> {log_path}\n> ', end='', flush=True)
                buffer.clear()
            else:
                buffer.append(byte)


def main():
    print(f'Esperando {PORT}...')
    ser = open_port()
    print(f'Conectado a {PORT} @ {BAUDRATE}')

    # Handshake: descarta lo pendiente y avisa a la STM32 de que ya escuchamos.
    # La STM32 esta bloqueada esperando este primer byte para arrancar.
    ser.reset_input_buffer()
    ser.write(READY_BYTE)

    stop_event = threading.Event()
    listener = threading.Thread(target=receiver_loop, args=(ser, stop_event), daemon=True)
    listener.start()

    print('Comandos: H | R | M x y z [...] | q para salir')
    try:
        while True:
            line = input('> ').strip()
            if not line:
                continue
            if line.lower() == 'q':
                break
            try:
                command = build_command(line.split())
            except ValueError as exc:
                print(exc)
                continue
            ser.write(command)
    except (KeyboardInterrupt, EOFError):
        pass
    finally:
        stop_event.set()
        listener.join(timeout=1)
        ser.close()
        print('\nCerrado.')


if __name__ == '__main__':
    main()
