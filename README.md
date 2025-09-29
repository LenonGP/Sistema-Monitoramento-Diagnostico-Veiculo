
# Vehicle Diagnostic and Monitoring System

This is an Arduino-based Vehicle Monitoring and Diagnostic System designed for educational purposes and compatible with the SimulIDE simulator. The system reads and processes data from simulated sensors for engine temperature, oil pressure, and RPM. It displays this real-time information on a 16x2 LCD screen and features an alert system with LEDs and a buzzer to warn against critical conditions like overheating or low pressure. For detailed analysis, all diagnostic data is also transmitted to a computer via the serial port.


## Functionalities

- Real-time Monitoring: Continuously tracks and processes three key vehicle metrics:

        Engine Temperature (simulated by a potentiometer).

        Oil Pressure (simulated by a potentiometer).

        Engine RPM (Revolutions Per Minute).

- Data Display: Presents clear, real-time information to the user on a 16x2 parallel LCD screen, showing temperature, pressure, and RPM simultaneously.

- Visual and Audible Alerts: The system actively checks for unsafe operating conditions:

        Activates a red LED and a buzzer if the engine temperature exceeds 90°C.

        Activates a yellow LED and a buzzer if the oil pressure drops below 20 PSI.

- Interrupt-Driven RPM Counter: Uses a hardware interrupt attached to a digital pin to accurately count engine revolutions without blocking the main program loop, ensuring responsive and reliable measurements.

- Serial Data Output: Transmits all collected data (temperature, pressure, and RPM) to a computer through the serial port, allowing for live monitoring, debugging, or data logging.

- Modular Code Structure: The program is organized into clear, single-purpose functions (e.g., **lerTemperatura()**, **verificarAlertas()**, **atualizarDisplay()**), making it easy to read, modify, and expand.

## Demonstration

https://www.youtube.com/watch?v=XiyVPtHBD1E

