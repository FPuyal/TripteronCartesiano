"""Envio de comandos al robot Tripteron (USB CDC).

Uso:
    py command_sender.py H
    py command_sender.py R
    py command_sender.py M X Y Z [X Y Z ...]

Ejemplo:
    py command_sender.py M 100 100 20 200 120 40

Comando (TX): texto ASCII "ID" o "ID n1 n2 n3 ..." terminado en '\\0'.
Este script solo envia el comando; no gestiona la recepcion de telemetria.
"""

import sys

import serial

PORT = 'COM6'
BAUDRATE = 115200
WRITE_TIMEOUT = 5.0  # seguridad: si el puerto no acepta la escritura, corta

VALID_COMMAND_IDS = {'H', 'R', 'M'}


def build_command(args):
    """Construye el mensaje de texto que espera Robot::ParseCommand.

    args = sys.argv[1:], p.ej. ['M', '100', '100', '20', '200', '120', '40']
    -> b'M 100 100 20 200 120 40\\0'
    """
    cmd_id = args[0].upper()
    if cmd_id not in VALID_COMMAND_IDS:
        raise ValueError(f'Comando desconocido: {args[0]}')

    message = ' '.join([cmd_id] + list(args[1:]))
    return message.encode('ascii') + b'\0'


def send_command(command):
    with serial.Serial(PORT, BAUDRATE, write_timeout=WRITE_TIMEOUT) as ser:
        ser.write(command)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    command = build_command(sys.argv[1:])
    send_command(command)


if __name__ == '__main__':
    main()
