Control de movimiento de un robot paralelo cartesiano de 3 DoF tipo tripteron.
Componentes:
- STM32F407 Discovery
- TMC2209 v1.3 BigTreeTech
- Nema 17 17HS4401-22B Two Trees

Es necesario poner a VDD el pin DIR del encoder para establecer una medición del angulo creciente en sentido contrario a las agujas del reloj.

## TODO
- [ ] Implementar comunicación UART entre STM y PC.
- [ ] Configurar TMC2209.
- [ ] Refactorizar el generador de trayectorias para que genere sólo perfiles trapezoidales.
- [ ] Cinemática.
- [ ] Realimentacion PID.
- [ ] Recepción de consignas.
- [ ] Envio de datos de STM a PC.
