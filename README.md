# LoRaSensor

Rak4270 module incorporates an STM32L071KB. The object of this project is to utilise that stm32 microcontroller to directly interface with the onboard Semtech LoRa transceiver.

Ideally, I would like to have a group of devices acting as low cost ie temperature sensors which can communicate at low power, long range.

I would like to use SF7/BW256 to minimise time on air (power cost). This should be configurable.

# Objectives: 
* flash onboard stm32 using standard stm32cubeide project (ie, blinky)
* move to realistic ide (ie, vscode)
* interface with SX2162 library
* basic communication from sensor to base station via LoRa
* basic communication from base station to sensor via LoRa
* get temperature of stm32 / use as analog for room temp
* diminish power usage (if I can reach 10microamps I'll be happy!)
* create pcb, ideally with onboard inverted F antenna
* polish code / get fancy.
