# Development of an Ethernet-Based Vehicle Monitoring System Using Linux Socket Programming

## 1. Project Overview

The **Ethernet-Based Vehicle Monitoring System** is a client-server application developed using C programming and Linux socket programming. The system demonstrates communication between two Electronic Control Units (ECUs), namely the Vehicle ECU and the Dashboard ECU, using TCP and UDP transport protocols.

The Vehicle ECU acts as a transmitter that generates simulated vehicle telemetry data, including vehicle speed, engine RPM, and steering angle. The Dashboard ECU acts as a receiver that listens for incoming data and displays the received vehicle parameters in the terminal.

The project demonstrates the practical implementation of network communication, socket programming, data transmission, and packet analysis in a Linux environment.

## 2. Objectives

The main objectives of this project are:

- To develop a Vehicle ECU application that generates simulated vehicle telemetry.
- To develop a Dashboard ECU application that receives and displays vehicle data.
- To implement communication using both TCP and UDP socket protocols.
- To understand the differences between connection-oriented and connectionless communication.
- To capture and analyse network packets using Wireshark.
- To evaluate the behaviour of TCP and UDP during vehicle telemetry transmission.

## 3. System Architecture

The system consists of two independent C applications.

### 3.1 Vehicle ECU (Sender)

The Vehicle ECU generates simulated vehicle parameters and transmits them to the Dashboard ECU over a network socket.

The telemetry data includes:

| Parameter | Description |
|---|---|
| `seq` | Sequential message number |
| `speed_kmh` | Vehicle speed in kilometres per hour |
| `engine_rpm` | Engine rotational speed in revolutions per minute |
| `steering_deg` | Steering wheel angle in degrees |

The generated data is formatted as JSON text and transmitted periodically.

### 3.2 Dashboard ECU (Receiver)

The Dashboard ECU listens on a specified IP address and port. It receives telemetry messages from the Vehicle ECU and displays the received information in the terminal.

The receiver supports both TCP and UDP communication.

### 3.3 Communication Flow

1. The Vehicle ECU generates simulated vehicle data.
2. The data is formatted into a JSON message.
3. The selected transport protocol is used to transmit the message.
4. The Dashboard ECU receives the transmitted message.
5. The received telemetry is displayed in the terminal.
6. Wireshark is used to capture and analyse the network packets.

## 4. Technologies and Tools Used

| Technology | Purpose |
|---|---|
| C Programming | Application development |
| Linux Socket API | Network communication |
| Ubuntu Linux | Development and execution environment |
| GCC Compiler | Compilation of C source files |
| TCP | Reliable, connection-oriented communication |
| UDP | Connectionless communication |
| Wireshark | Network packet capture and analysis |

## 5. Project Directory Structure

```text
Vehicle_Monitoring_System/
│
├── src/
│   ├── vehicle_ecu.c
│   └── dashboard_ecu.c
│
├── docs/
│   ├── Design_Document.pdf
│   ├── Validation_Report.pdf
│   └── TCP_UDP_Comparison.pdf
│
├── screenshots/
│   └── Execution and Wireshark screenshots
│
├── wireshark/
│   ├── udp_capture.pcapng
│   └── tcp_capture.pcapng
│
├── Makefile
└── README.md
```

## 6. Implementation Details

### 6.1 UDP Communication

UDP is a connectionless transport protocol. The sender transmits individual datagrams to the receiver without establishing a connection beforehand.

In this project, the Vehicle ECU uses a UDP socket to transmit telemetry messages to the Dashboard ECU. The receiver listens on the configured port and processes incoming datagrams.

UDP is suitable for applications requiring low communication overhead and frequent transmission of sensor information.

### 6.2 TCP Communication

TCP is a connection-oriented transport protocol that provides reliable and ordered delivery of data.

In this project, the Dashboard ECU creates a TCP listening socket and waits for a connection from the Vehicle ECU. Once a connection is established, the sender transmits telemetry messages through the connected socket.

TCP is suitable for applications where reliable delivery and correct ordering of transmitted information are important.

## 7. Compilation and Execution

### 7.1 Prerequisites

Ensure that the following tools are installed on Ubuntu:

- GCC compiler
- Make utility (if using the Makefile)
- Wireshark

Install the required tools using:

```bash
sudo apt update
sudo apt install build-essential wireshark -y
```

### 7.2 Compile the Applications

Navigate to the project directory:

```bash
cd Vehicle_Monitoring_System
```

Compile using the Makefile:

```bash
make
```

Alternatively, compile the applications manually:

```bash
gcc -Wall -Wextra -std=c11 -O2 src/vehicle_ecu.c -o vehicle_ecu
gcc -Wall -Wextra -std=c11 -O2 src/dashboard_ecu.c -o dashboard_ecu
```

### 7.3 Execute Using UDP

Open the first terminal and start the Dashboard ECU:

```bash
./dashboard_ecu --protocol udp --bind 127.0.0.1 --port 5001
```

Open a second terminal and start the Vehicle ECU:

```bash
./vehicle_ecu --protocol udp --host 127.0.0.1 --port 5001
```

The Vehicle ECU periodically generates and transmits telemetry messages. The Dashboard ECU displays the received messages.

### 7.4 Execute Using TCP

Open the first terminal and start the Dashboard ECU:

```bash
./dashboard_ecu --protocol tcp --bind 127.0.0.1 --port 5000
```

Open a second terminal and start the Vehicle ECU:

```bash
./vehicle_ecu --protocol tcp --host 127.0.0.1 --port 5000 --interval 1
```

The TCP connection is established between the two applications, after which telemetry messages are transmitted and displayed.

Press `Ctrl+C` to stop either application.

## 8. Wireshark Packet Analysis

Wireshark was used to capture and analyse the packets exchanged between the Vehicle ECU and Dashboard ECU.

Packet capture was performed using the loopback network interface (`lo`) because both applications were executed on the same Ubuntu machine.

The following display filters were used:

**UDP packet filter:**

```text
udp.port == 5001
```

**TCP packet filter:**

```text
tcp.port == 5000
```

### 8.1 UDP Packet Observations

The UDP capture shows datagrams transmitted from the Vehicle ECU to the Dashboard ECU through port 5001.

The captured packets contain vehicle telemetry data, including speed, engine RPM, steering angle, and sequence number.

### 8.2 TCP Packet Observations

The TCP capture shows communication between the Vehicle ECU and Dashboard ECU through port 5000.

The capture contains TCP data segments and acknowledgement packets, demonstrating the reliable, connection-oriented nature of TCP communication.

The packet captures are included in the `wireshark` directory.

## 9. Results and Observations

The project was successfully executed in the Ubuntu Linux environment.

The following results were observed:

- The Vehicle ECU successfully generated simulated vehicle telemetry.
- The Dashboard ECU successfully received and displayed telemetry messages.
- UDP communication was demonstrated using port 5001.
- TCP communication was demonstrated using port 5000.
- Wireshark successfully captured UDP and TCP traffic.
- The captured packets showed the transmitted JSON telemetry information.
- Both communication modes were demonstrated using separate terminal sessions.

The screenshots and packet capture files included in this repository provide supporting evidence of the execution and network analysis.

## 10. TCP and UDP Comparison

| Feature | TCP | UDP |
|---|---|---|
| Connection | Connection-oriented | Connectionless |
| Reliability | Reliable delivery | No delivery guarantee |
| Ordering | Preserves byte-stream order | No ordering guarantee |
| Acknowledgement | Supported | Not provided by UDP itself |
| Overhead | Higher | Lower |
| Communication | Stream-based | Datagram-based |
| Project Port | 5000 | 5001 |

TCP provides reliable and ordered communication, whereas UDP offers lower-overhead datagram transmission. The appropriate protocol depends on the reliability and latency requirements of the vehicle monitoring application.

## 11. Limitations

- Vehicle parameters are simulated and are not obtained from actual vehicle sensors.
- Both ECUs were executed on the same Ubuntu machine.
- Communication used the localhost address (`127.0.0.1`) and loopback interface.
- The project demonstrates application-level telemetry communication rather than communication with a physical vehicle network.
- UDP does not inherently guarantee successful message delivery.

## 12. Conclusion

The project successfully demonstrates the development of an Ethernet-based vehicle monitoring application using Linux socket programming in C.

Two independent applications, the Vehicle ECU and Dashboard ECU, were developed to demonstrate vehicle telemetry transmission and reception using TCP and UDP protocols.

The implementation provided practical experience with socket creation, network communication, data transmission, and packet analysis using Wireshark.

The results demonstrate the differences between TCP and UDP and highlight their respective advantages for vehicle monitoring applications. The project can be extended in the future by integrating real vehicle sensors, implementing graphical dashboards, and enabling communication between separate physical devices over an Ethernet network.

## 13. Author and Project Information

**Project Title:** Development of an Ethernet-Based Vehicle Monitoring System Using Linux Socket Programming

**Programming Language:** C

**Operating System:** Ubuntu Linux

**Compiler:** GCC

**Network Analysis Tool:** Wireshark

**Communication Protocols:** TCP and UDP