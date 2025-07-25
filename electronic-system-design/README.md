# Electronic Design
This section provides a detailed explanation of the project's electronic design, including key decisions, objectives, and development rationale. It aims to offer a broader perspective on the piano as a finished product, beyond just its functionality.

## The Piano
This project is a university-level design that combines embedded systems and electronic hardware development. It is built around the LCPI-PC-T113 development board, featuring the Allwinner T113s chip running embedded Debian.

The piano integrates both custom PCB and enclosure design, with additional documentation on the development process.

Its goal is to offer a low-cost, portable musical instrument that serves both as a basic piano and as an educational tool. It features two main modes:

- **Piano Mode** – Pressing a key plays the corresponding note through a speaker.
- **Tutor Mode** – LEDs guide the user through preloaded melodies, indicating which key to press next.

The system is designed for beginners, blending play and learning in a compact, accessible format.

## Inspiration
This piano is physically inspired by commercial MIDI controllers such as the **MiniLab 3** by Arturia, the **Akai MPK Mini**, and the **OP-1** by Teenage Engineering.

<img src="/electronic-system-design/images/minilab3.jpg" alt="MiniLab 3" width="400"/>
<img src="/electronic-system-design/images/op1.jpg" alt="OP-1" width="600"/>
<img src="/electronic-system-design/images/akai_mkp_mini.jpg" alt="Akai MPK Mini" width="400"/>

The goal is to build a portable device that gives users easy access to a piano wherever they are.

## Objectives
The goal of this project is to create an affordable, efficient, and scalable digital keyboard that works as a standalone device. It features an intuitive user interface.

### Main Objective
Design and build an affordable, energy-efficient, and user-friendly digital keyboard with an intuitive interface, capable of functioning independently and suitable for educational or experimental use.

### Specific Objectives
- **Ensure accessibility and affordability** by keeping the total cost of components below $300,000 COP, making the device viable for students and academic institutions.
- **Promote usability and learning** by including exercises or songs, reinforcing its role as a musical tutor and learning tool.
- **Guarantee energy efficiency and portability** by ensuring the device operates with a current draw of less than 1.5 A, allowing it to be powered by a standard 5V / 2A USB adapter or computer.

## Block Diagram

The proposed system configuration uses the LCPI-PC-T113 development board, featuring the Allwinner T113-S3 chip, as the main controller. A custom daughterboard (the PCB designed in this course) interfaces with it to connect the piano peripherals.

This separation of boards allows modularity: the main board can be reused in future projects, while the daughterboard is application-specific.

The system consists of five main modules:

- **Piano Inputs** – Controls 25 keys (from C4 to C6, covering 2 octaves) arranged in a 5×5 matrix. A *74HC595* shift register activates the columns, while a *74HC165* shift register reads the row states.
  - Shift registers reduce the number of required GPIOs and allow scalable input/output control.
  - ✅ *Pro*: Efficient use of GPIOs with minimal pin usage.
  - ❌ *Con*: Adds a small delay due to serial communication and requires precise clocking.

- **User Interface** – Includes an OLED display and three menu buttons.
  - A minimal interface was chosen to ensure simplicity and ease of navigation for all users.
  - ✅ *Pro*: Clear feedback and interaction with minimal components.
  - ❌ *Con*: Limited interactivity and no text input options.

- **Piano Outputs** – Manages 25 LEDs (one per key) using two *74HC595* shift registers in a 5×5 matrix. Audio output is routed from the board’s headphone jack to an 8-ohm speaker.
  - Using a matrix configuration minimizes wiring and enables efficient LED control in tutor mode.
  - ✅ *Pro*: Clear visual guidance during learning.
  - ❌ *Con*: Cannot light multiple arbitrary combinations without flickering unless advanced multiplexing is implemented.

- **Power** – Powered via a USB-C port providing 5V @ 2A, allowing compatibility with standard smartphone chargers.
  - USB-C is a modern standard and widely available.
  - ✅ *Pro*: Universal and accessible power source.
  - ❌ *Con*: Some connectors have mechanical instability depending on the cable used.

- **Serial Communication and Data** – Includes SD card storage and USB to TTL serial communication used during development and booting.
  - External storage and serial communication simplify testing and firmware updates.
  - ✅ *Pro*: Easy debugging and access to external data.
  - ❌ *Con*: Required workaround (jumpers) due to missing TX/RX pins in the schematic.

<img src="/electronic-system-design/images/block_diagram.png" alt="block diagram" width="600"/>

## Electrical Schematic

A total of 19 pins are used to connect the processor to the piano's peripherals. The image below shows the pinout configuration:

<img src="/electronic-system-design/images/pinout.png" alt="pinout" width="400"/>

This layout was chosen to optimize routing and prioritize low-latency signals, like the key matrix and OLED interface.

The following sections explain how each module works and how the components are connected. All images are taken from the project schematic file `piano_pcb.kicad_sch`, available at `./resources/pcb/piano_pcb`.

### Keys Matrix
A 5×5 matrix is used to detect key presses, allowing us to read 25 distinct notes. A *74HC595* shift register sequentially activates each column, and a *74HC165* reads the row values. The switches are configured with pull-down resistors: a pressed key outputs logic `1`, and an unpressed key outputs `0`.

- ✅ *Pro*: Scalable and compact solution for multiple key inputs.
- ❌ *Con*: Requires careful debouncing and timing control in software.

<img src="/electronic-system-design/images/sc_keys_matrix.png" alt="sc_keys_matrix" width="400"/>
<img src="/electronic-system-design/images/sc_keys_registers.png" alt="sc_keys_registers" width="600"/>

### LEDs Matrix
Another 5×5 matrix is used to control the LEDs, which are mainly used in tutor mode to highlight the next key to press. Two *74HC595* shift registers manage the LED activation. LEDs use a pull-down configuration: a LED lights up when its cathode is at logic `1` and its anode is at logic `0`.

Coupling capacitors were used in the four integrated circuits to ensure proper power connection.

- ✅ *Pro*: Clean and organized visual feedback system.
- ❌ *Con*: Without current control, simultaneous activation of many LEDs can overload the system.

<img src="/electronic-system-design/images/sc_keys_matrix.png" alt="sc_leds_matrix" width="400"/>
<img src="/electronic-system-design/images/sc_keys_registers.png" alt="sc_leds_registers" width="600"/>

### I2C Screen
Initially, a 240×240 SPI screen was proposed for the menu interface. However, due to technical issues and budget/time constraints, we opted for a 128×64 I2C OLED display using the SSD1306 controller.

- ✅ *Pro*: Easy to integrate, widely supported, and consumes less power.
- ❌ *Con*: Lower resolution limits UI complexity.

<img src="/electronic-system-design/images/sc_screen.png" alt="sc_screen" width="300"/>

### Menu Buttons
Three buttons (`Left`, `Enter`, and `Right`) allow the user to navigate the interface. All buttons use a pull-down configuration: pressing a button results in logic `1`, and releasing it results in logic `0`.

- ✅ *Pro*: Simple and intuitive user interaction.
- ❌ *Con*: Limited to basic menu navigation without multi-directional input.

Once the device is powered on and the application starts, the main menu is displayed with two options:

1. **Piano Mode** – Basic mode in which pressing a key plays the corresponding sound.
2. **Tutor Mode** – Guides the user through one of four built-in songs: `Estrellita`, `Cumpleaños Feliz`, `Los Pollitos`, and `Piratas del Caribe`.

<img src="/electronic-system-design/images/sc_buttons.png" alt="sc_buttons" width="600"/>

### Power Input
Power is supplied through a USB-C female connector, chosen for its compatibility with most modern chargers. However, this decision turned out to be suboptimal — in practice, the connection can be unstable depending on the cable or connector, causing accidental shutdowns with slight movement.

To protect the circuit, an SS14 diode prevents reverse current and voltage spikes. Two 5.1kΩ resistors to ground ensure the USB-C port is used strictly for power and limits voltage to 5V.

- ✅ *Pro*: Broad compatibility and compact form factor.
- ❌ *Con*: Susceptible to disconnection if cable or socket is loose.

<img src="/electronic-system-design/images/sc_power.png" alt="sc_power" width="400"/>

### Dev Board Integration
A custom symbol and footprint were created in KiCad to represent the LCPI-PC-T113 board and ensure proper alignment of connections.

The schematic shows the necessary connections for:
- **Shift register control** – `SER`, `CLK`, `LATCH`
- **Menu buttons** – input GPIOs for `Left`, `Enter`, and `Right`
- **OLED display** – `SDA`, `SCL` for I2C communication

- ✅ *Pro*: Accurate mapping of dev board pins in PCB layout.
- ❌ *Con*: Omission of serial TX/RX was a critical error, requiring manual correction with jumpers.

<img src="/electronic-system-design/images/sc_devboard.png" alt="sc_devboard" width="400"/>

The serial communication was originally assigned to pins `PE2` and `PE3`, as shown in the pinout image above. However, these connections were not included in the schematic — a design oversight. As a workaround, jumpers were added in the physical prototype. This is one of the areas identified for improvement in future revisions.

## Materials
The piano consists of low-cost electronic components, including standard switches, LEDs, and diodes. The two most expensive parts of the build are the LCPI-PC-T113 development board and the custom-designed daughterboard (PCB). These will be detailed further in the cost breakdown.

Below is a list of the main components used:

- 25 × Tactile switches (1 per key)
- 25 × Red LEDs (1 per key for tutor feedback)
- 25 × 220 Ω resistors (for LED current limiting)
- 50 × 1N4148 diodes (for matrix input protection)
- 8 × 10 kΩ resistors (for pull-down configurations)
- 3 × 4-leg tactile menu switches (Left, Enter, Right)
- 4 × 8×2 pin sockets (for shift register and board connections)
- 3 × 74HC595 shift registers (2 for LED matrix, 1 for key matrix column activation)
- 1 × 74HC165 shift register (for key matrix row reading)
- 4 × 100 nF ceramic capacitors (for decoupling)
- 1 × SS14 Schottky diode (for reverse polarity protection)
- 2 × 5.1 kΩ resistors (for USB-C power configuration)
- 1 × USB-C female port (for 5V power input)
- 2 × Female header grids (to interface with dev board)

All components were selected for availability, cost-efficiency, and compatibility with hand soldering and prototyping.

## PCB Design

KiCad and LayoutEditor were the main tools used to design the PCB. The complete KiCad project is available at `./resources/pcb/piano_pcb`.

Returning to our five main modules, this section covers the design process for each one.

---

### Piano Inputs

The key matrix relies on mechanical keyboard switches and diodes. [This video](https://www.youtube.com/watch?v=8WXpGTIbxlQ) provided valuable guidance on mechanical keyboard design and was used as a reference. Its symbol and footprint library was also integrated and is available in `./resources/pcb/piano_pcb/Resources`.

Each note has a dedicated LED, so the layout prioritizes compactness while replicating a traditional piano key arrangement. Diodes are placed to the left of each key to prevent ghosting, and LEDs are positioned above or below the switch based on the note's vertical position.

<img src="/electronic-system-design/resources/pcb/images/kicad_1.png" alt="kicad_1" width="400"/>
<img src="/electronic-system-design/resources/pcb/images/kicad_2.png" alt="kicad_2" width="400"/>

Footprint used for the key switches:

<img src="/electronic-system-design/resources/pcb/images/sw_footprint.png" alt="sw_footprint" width="300"/>

### Piano Outputs

The LEDs are a core feature for the tutor mode. Each LED has a dedicated 220 Ω current-limiting resistor and a 1N4148 diode to avoid unwanted activation due to matrix effects (ghosting).

<img src="/electronic-system-design/resources/pcb/images/led_1.png" alt="led_1" width="300"/>
<img src="/electronic-system-design/resources/pcb/images/led_2.png" alt="led_2" width="300"/>
<img src="/electronic-system-design/resources/pcb/images/led_3.jpg" alt="led_3" width="300"/>

### User Interface

The menu buttons and display are located on the top edge of the piano for visibility and accessibility. The OLED screen symbol and footprint were manually created based on an initial SPI version, though later adapted to a 128×64 I2C display (SSD1306). The chosen I2C pins remained the same for mechanical compatibility.

<img src="/electronic-system-design/resources/pcb/images/menu_1.png" alt="menu_1" width="400"/>
<img src="/electronic-system-design/resources/pcb/images/menu_2.png" alt="menu_2" width="400"/>

### Power Input

This section presented the most soldering challenges due to two surface-mount components: the USB-C female connector and the SS14 Schottky diode. These were the only compatible models available in the local market and required careful placement.

<img src="/electronic-system-design/resources/pcb/images/power_1.png" alt="power_1" width="300"/>
<img src="/electronic-system-design/resources/pcb/images/power_2.png" alt="power_2" width="300"/>
<img src="/electronic-system-design/resources/pcb/images/power_3.png" alt="power_3" width="300"/>

### Devboard Interface

A custom footprint was created to place female headers in a "shield-like" fashion, aligning with the LCPI-PC-T113 board’s pinout. This allows reliable and modular interfacing between the devboard and the daughterboard.

<img src="/electronic-system-design/resources/pcb/images/dev_1.png" alt="dev_1" width="400"/>

---

### Construction

The PCB routing was done using the `FreeRouting` tool. All traces were set to 0.5 mm width, which is sufficient given the low current requirements of each signal. Both top and bottom layers were used, with several vias placed throughout the board—particularly around the key and LED matrices—due to the high density of connections.

The PCB was manufactured by **JLCPCB**, and the production files are available at `./resources/pcb/piano_pcb/production`.

Below are some images of the PCB layout and the assembled board:

<img src="/electronic-system-design/resources/pcb/images/top_pcb_1.png" alt="top_pcb_1" width="600"/>
<img src="/electronic-system-design/resources/pcb/images/top_pcb_2.png" alt="top_pcb_2" width="600"/>
<img src="/electronic-system-design/resources/pcb/images/bot_pcb_1.png" alt="bot_pcb_1" width="600"/>
<img src="/electronic-system-design/resources/pcb/images/bot_pcb_2.png" alt="bot_pcb_2" width="600"/>

Final soldered board:

<img src="/electronic-system-design/resources/pcb/images/final_pcb.png" alt="final_pcb" width="600"/>

## Case Design

The case was designed using **Adobe Illustrator**, vectorizing the pieces for laser cutting. The main PCB measures **314 mm × 150.5 mm**, and additional clearance was considered for the height of the devboard underneath (~15 mm), as well as the switches and screen above (~17.5 mm).

The design consists of two main parts:

1. **The Case** – Made of MDF and laser cut to encase the piano. It protects the electronics and gives structural support.

<img src="/electronic-system-design/resources/case/images/mdf.png" alt="mdf" width="500"/>

2. **The Keycaps** – A piece of laser-cut acrylic simulates the shape of piano keys. The transparency allows LED light to shine through during tutor mode, guiding the user visually.

<img src="/electronic-system-design/resources/case/images/acr.png" alt="acr" width="500"/>

The Adobe Illustrator and PDF files used in the process are available at `./resources/case/files`.

### Case Construction

Once all pieces were laser cut, assembly was done manually. The result is a lightweight, solid enclosure with custom-fit keycaps:

<img src="/electronic-system-design/resources/case/images/case_1.png" alt="case_1" width="500"/>
<img src="/electronic-system-design/resources/case/images/case_2.png" alt="case_2" width="400"/>
<img src="/electronic-system-design/resources/case/images/case_3.png" alt="case_3" width="300"/>

## User Menu

As mentioned earlier, three buttons (`Left`, `Enter`, and `Right`) are used to navigate the user interface. All buttons follow a pull-down configuration: pressing a button sends logic `1`, and releasing it returns to logic `0`.

Upon powering on the device and loading the main application, the user is presented with the main menu featuring two options:

1. **Piano Mode** – A classic mode where each key press immediately plays a corresponding note.
2. **Tutor Mode** – A guided mode where the user can select one of four built-in songs: `Estrellita`, `Cumpleaños Feliz`, `Los Pollitos`, and `Piratas del Caribe`.

Below is a simplified flow diagram of the navigation logic:

<img src="/electronic-system-design/resources/screen/images/flow_diagram.png" alt="flow_diagram" width="500"/>

Some photos of the interface in action:

<img src="/electronic-system-design/resources/screen/images/oled_1.png" alt="oled_1" width="300"/>
<img src="/electronic-system-design/resources/screen/images/oled_2.png" alt="oled_2" width="300"/>


## Prototype


# Benchmarking / Target specifications

# prices of the project

# improvements