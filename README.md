# ✈️ Airport Runway Management System

## 📌 Overview

The **Airport Runway Management System** is a console-based application developed in **C** that simulates the scheduling of aircraft landings using a **Priority Queue**. The system efficiently assigns runway access by considering multiple factors such as **emergency status**, **fuel level**, and **arrival time**, ensuring that critical aircraft receive priority.

This project demonstrates the practical application of **Data Structures**, **Algorithms**, **Dynamic Memory Allocation**, and **File Handling** in solving a real-world scheduling problem.

---

## ✨ Features

* ✈️ Add incoming aircraft to the landing queue
* 🚨 Emergency aircraft receive the highest landing priority
* ⛽ Fuel-level based landing priority
* ⏰ Arrival-time based scheduling
* 📋 Display the current runway waiting queue
* ❌ Cancel scheduled aircraft
* 💾 Save runway queue to a file
* 📄 Generate runway operation reports
* 🔄 Dynamic Priority Queue implementation using memory allocation

---

## 🛠️ Tech Stack

* **Language:** C
* **Concepts Used:**

  * Priority Queue
  * Heap-based Scheduling
  * Dynamic Memory Allocation
  * File Handling
  * Structures
  * Functions
* **Development Environment:**

  * Visual Studio Code
  * GCC Compiler (MinGW)

---

## 🧠 Data Structures Used

### Priority Queue

The system maintains aircraft in a priority queue where landing priority is determined by:

1. Emergency Status
2. Fuel Level
3. Arrival Time

This ensures that aircraft requiring immediate attention are processed before others.

---

## ⚙️ Project Workflow

```text
Aircraft Arrives
        │
        ▼
Enter Flight Details
        │
        ▼
Calculate Landing Priority
        │
        ▼
Insert into Priority Queue
        │
        ▼
Display Queue / Assign Runway
        │
        ▼
Generate Reports & Save Data
```

---

## 📸 Screenshots

> Screenshots will be added soon.

* Main Menu
* Aircraft Queue
* Emergency Landing
* Generated Report

---

## 🚀 How to Run

### Clone the Repository

```bash
git clone https://github.com/Satheessk/Airport_runway_Management.git
```

### Navigate to the Project

```bash
cd Airport_runway_Management
```

### Compile

```bash
gcc Airport_runway_management.c -o airport
```

### Run

Windows

```bash
airport.exe
```

Linux / macOS

```bash
./airport
```

---

## 📊 Key Concepts Demonstrated

* Priority Queue
* Heap Operations
* Dynamic Memory Allocation
* File Handling
* Simulation of Real-world Scheduling
* Structured Programming in C

---

## 📈 Time Complexity

| Operation                        | Complexity |
| -------------------------------- | ---------- |
| Insert Aircraft                  | O(log n)   |
| Remove Highest Priority Aircraft | O(log n)   |
| Display Queue                    | O(n)       |
| Search Aircraft                  | O(n)       |
| Save / Load Data                 | O(n)       |

---

## 📂 Project Structure

```text
Airport_runway_Management/
│
├── Airport_runway_management.c
├── README.md
├── .gitignore
└── screenshots/
```

---

## 🔮 Future Enhancements

* Multi-runway scheduling
* Flight search functionality
* Landing statistics dashboard
* Delay management
* Graphical User Interface
* Database integration
* Live flight data integration

---

## 🎯 Learning Outcomes

Through this project, I gained practical experience in:

* Designing efficient scheduling algorithms
* Implementing Priority Queues in C
* Dynamic Memory Management
* File Operations
* Problem Solving using Data Structures
* Building real-world console applications

---

## 👨‍💻 Author

**Satheeskumar G**

* GitHub: https://github.com/Satheessk

---

## ⭐ Support

If you found this project useful, consider giving it a **⭐ Star** on GitHub.
