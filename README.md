# 🐭 NoNameZz MicroMouse - STM32 Maze Solving Robot
[![MCU: STM32F103](https://img.shields.io/badge/MCU-STM32F103C8T6-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103.html)
[![Sensor: VL53L0X](https://img.shields.io/badge/Sensor-VL53L0X%20ToF-green.svg)](https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html)
[![Algorithm: Flood Fill](https://img.shields.io/badge/Algorithm-Flood%20Fill-orange.svg)]()

## 📌 Giới thiệu (Overview)
Dự án này là mã nguồn điều khiển robot tự hành giải mê cung (Micromouse) được phát triển vào năm 2 đại học. Robot có khả năng tự động khám phá mê cung, lập bản đồ và tìm ra đường đi ngắn nhất đến đích. 


## 🛠 Phần cứng (Hardware)
* **Vi điều khiển chính:** STM32F103C8T6 (ARM Cortex-M3).
* **Cảm biến khoảng cách:** Cụm cảm biến Time-of-Flight (ToF) **VL53L0X** đo khoảng cách chính xác cao, ít bị nhiễu bởi màu sắc tường.
* **Động cơ:** Động cơ DC JGB37 có Encoder để đo lường quãng đường (Odometry) và kiểm soát tốc độ.

## 🚀 Tính năng nổi bật (Technical Highlights)
- **Sensor Fusion:** Sử dụng cụm 6 cảm biến Laser ToF **VL53L0X** giao tiếp qua I2C.
- **Motion Control:** Điều khiển động cơ DC qua PWM (TIM1) kết hợp phản hồi từ Encoder (TIM2, TIM3) để đạt độ chính xác cao trong các pha di chuyển.
- **Pathfinding & Mapping:** - Giai đoạn khám phá: Sử dụng cơ chế Stack-based để quản lý bản đồ và tự động quay lui khi gặp đường cụt.
  - Giai đoạn chạy tốc độ: Triển khai thuật toán tìm đường ngắn nhất (BFS-based).
- **Trajectory Optimization:** Thuật toán tự động tính toán và chuyển đổi các khúc cua vuông góc thành **đường chạy chéo 45 độ**, giúp tối ưu hóa thời gian hoàn thành mê cung.

## 📂 Cấu trúc mã nguồn
- `main.c`: Khởi tạo hệ thống, cấu hình ngoại vi và vòng lặp điều khiển chính.
- `fill_maze.c`: Logic khám phá mê cung và quản lý trạng thái tường.
- `path_optimization.c`: Tính toán đường đi ngắn nhất và tối ưu hóa quỹ đạo chuyển động.

## ⚙️ Cấu trúc thư mục chính (Key Directories)
- `Core/Src/`: Chứa logic điều khiển chính (Main loop, Flood Fill, Path Optimization).
- `Drivers/VL53L0X/`: Driver giao tiếp I2C với cảm biến khoảng cách ToF do tự tinh chỉnh.
- `*.ioc`: File cấu hình phần cứng của STM32CubeIDE.
