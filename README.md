# Automated Storage and Retrieval System (ASRS)

This repo covers the development of a low-cost **Automated Storage and Retrieval System (ASRS)**, using stepper motors, ESP32, Arduino Mega 2560 and some other electronic components. The system is controlled with a FIFO (First-In First-Out) Algorithm and RFID authetication. It also integrates a load cell to save the weight of the material, and storing it alongside the RFID Card.

<div align="center">
    <img src="https://imgur.com/B0eKkpb.png" alt="script" width="300">
</div>


## Repository Structure

- **/CAD**: STL and STEP files.
- **/Electronics**: Electrical schematic and diagrams.
- **/Software**: Source code for Arduino and ESP32.
- **/Pictures**: Images for analysis. 
- **/Docs**: Relevant documents.


## Mechanical Structure
The structure is primarily build from wood with some 3D-printed parts for the motors and axis. The Y-Axis use a lead screw for vertical movement, while the X-Axis and Z-Axis use timing belts for horizontal movement.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Mechanics/ASRS_MechanicalComponents.png" alt="script" width="250">
</div>

### **Mechanical Components**  

| **Component**              | **Quantity** | **Description**                                              |  
|----------------------------|-------------|--------------------------------------------------------------|  
| Lead screw (8mm, 50cm)     | 1           | Used for vertical movement on the Y-axis.                   |  
| Coupler                    | 1           | Connects the lead screw to the stepper motor.               |  
| Linear shafts (10mm, 50cm) | 4           | Provides smooth linear motion for the system.               |  
| Linear shafts (10mm, 20cm) | 2           | Supports movement in smaller sections of the system.        |  
| Linear shaft supports      | 8           | Holds the linear shafts in place.                           |  
| Linear bearing blocks      | 6           | Enables smooth sliding motion along the shafts.             |  
| GT2 timing belts           | 2           | Used for horizontal movement on the X and Z axes.           |  
| GT2 pulleys with bearing   | 2           | Compatible with the GT2 timing belts, includes a bearing.   |  
| 3D-printed supports        | x           | Used for mounting motors and mechanical components.         |  

---

### Operation

1. The ESP32 receives authentication via RFID and enables system control.

2. The user controls the system through Contact Buttons.

3. The ESP32 processes the FIFO algorithm and sends, via serial communication, the compartment to be handled by the Arduino Mega.

4. The Arduino Mega interprets the commands and activates the stepper motors to move along the X, Y, and Z axes.

5. Sensors ensure precision and safety in movements.

6. The system records the position, material weight, and the user's RFID tag.

### Block Diagram
The Block Diagram provides a visual representation of the data and control flow within the ASRS (Automated Storage and Retrieval System), organizing the main components and their interactions. It is divided into several essential sections for system operation, such as Inputs, Outputs, Actuators, Power Supply, and Microcontrollers.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Functional_Data_Block.png" alt="script" width="700">
</div>
   
### Electronic Circuit
This is the detailed electrical schematic of the ASRS system, developed in KiCad. It presents all electrical connections, including components such as ESP32, Arduino Mega, motor drivers, sensors, power supply, and other electronic modules used in the project.

[<img src="https://i.imgur.com/bHJxbk0.png" alt="Esquema" width="800">](https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Schematic_Colours.pdf)
    
This diagram provides a visual representation of the connections between the main ASRS components. It facilitates assembly and understanding of the connections between microcontrollers, motors, sensors, and auxiliary modules, making it ideal for quick reference and practical circuit assembly.

<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Circuit_Connections.png" alt="script" width="1000">

### Electronic Components

That table provides a summary of the electronic components used in the development of the project. It lists input and output devices, microcontrollers, and auxiliary components.

| Component                                   | Quantity |
|---------------------------------------------|----------|
| ESP32                                       | 1        |
| Arduino Mega 2560                           | 1        |
| CNC Shield V3                               | 1        |
| A4988 Driver                                | 3        |
| RFID RC522                                  | 1        |
| LCD 16x2 with I²C Module                    | 1        |
| 4x3 Keypad                                  | 1        |
| Weight Module with HX711                    | 1        |
| 10k Linear Potentiometer                    | 3        |
| Limit Sensors                               | 3        |
| 19mm Metal Contact Button with LED          | 2        |
| 22mm Metal Contact Button with LED          | 1        |
| NC Emergency Button                         | 1        |
| 24V Relay                                   | 1        |
| 5V Voltage Regulator LM805                  | 1        |
| 12V Voltage Regulator LM812                 | 1        |
| 12V 0.14A Fan                               | 1        |
| Buzzer                                      | 1        |
| 1µF Capacitor                               | 2        |
| 100µF Capacitor                             | 2        |
| LM358                                       | 1        |
| 1N4001 Diode                                | 2        |
| 10kΩ Resistor                               | 2        |
| 55Ω Resistor                                | 1        |
| 470Ω Resistor                               | 1        |

### Programming and Flowcharts

#### Queue Algorithm (FIFO)

The Queue algorithm was implemented to control the movement of materials within the ASRS system. The Queue follows the FIFO (First In, First Out) approach, where the first item to enter will be the first to exit. This type of structure is ideal for our system as it ensures that materials are stored and retrieved in an organized manner, preventing any compartment from being overloaded.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Algorithm_Queue.png" alt="script" width="700">
</div>

### Enqueue Algortithm

The Enqueue operation is responsible for adding a new material to the system, registering its weight, and assigning it to an available compartment. The process follows these steps:

    Check Availability – The system verifies if there is space in the queue.
    Read the Material's Weight – A weight sensor captures the material’s weight.
    Store in Queue – The material is assigned to the next available compartment.
    Update the Tail Pointer – The queue pointer is updated for the next insertion.
    Save to EEPROM – Data is stored for persistence.
    
<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Enqueue.png" alt="script" width="500">
</div>

### Dequeue Algorithm

The Dequeue operation handles the retrieval of a material from storage. It follows these steps:

    Check if the Queue is Not Empty – Ensures that there are materials to be retrieved.
    Retrieve the Material – The system selects the first stored material.
    Move the Storage Mechanism – The ASRS moves to the corresponding compartment.
    Update the Head Pointer – The queue pointer is updated to reflect the removal.
    Save to EEPROM – The updated queue state is stored for future reference.
    
<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Dequeue.png" alt="script" width="500">
</div>

#### Flowcharts

The flowchart below illustrates the program flow on the ESP32, with the FIFO algorithm being used to determine the order of operation.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Flowchart_ESP32.png" alt="script" width="300">
</div>

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Flowchart_Enqueue.png" alt="script" width="250">

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Flowchart_Dequeue.png" alt="script" width="250">

---

