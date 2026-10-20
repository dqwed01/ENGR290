# Design 2 — Rectangular Hovercraft with Single Vectored Thrust Fan

## Description

### Technical Overview

Design 2 is a rectangular, low-component-count hovercraft built around a single central lift fan and a single rear thrust fan mounted on a servo. The servo swings the thrust fan to steer, which is called thrust vectoring. The goal is to complete the course with as few counted components as possible while keeping enough lift margin to clear the 1, 2 and 3 mm obstacles. The outline is rectangular with a 1.5 : 1 aspect ratio. This gives good straight-line directional stability, fits inside the 50–55 cm lanes, and keeps the overall diagonal under the 48 cm competition limit.

| Parameter | Value |
|---|---|
| Deck (foam) | 360 × 240 mm |
| Overall incl. skirt | 380 × 260 mm |
| Overall diagonal | 460 mm (limit 480 mm) |
| Total mass | 0.760 kg |
| Counted components N<sub>c</sub> | 6 (IMU not counted) |

### Hovercraft Architecture and Subsystems

The chassis is a single foam deck with the origin at its geometric centre, with +x pointing toward the nose and +y toward the left side. The lift fan sits in a 120 mm hole slightly ahead of the centre. The thrust fan and servo are at the rear, and the controller and batteries are at the front. The rear propulsion assembly and the front electronics have nearly equal mass moments, so the centre of mass lands 3 mm from the centre of lift.

### Propulsion and Directional Control

- **Lift:** The Delta AFB1212SH is used for lift because it has the highest static pressure of the available fans (107.1 Pa at 12 V). Pressure is the governing specification for a cushion. The fan is driven at an effective 13.8 V, its rated maximum, through PWM from the 14.8 V battery. This increases its pressure capability to about 141.6 Pa.
- **Thrust:** The Sunon MEC0251V1 is mounted vertically on the servo horn at the rear. Thrust depends on airflow rather than pressure, and this fan offers the best airflow per gram (108.2 CFM, 162 g).
- **Steering:** The HS-311 servo rotates the thrust fan by up to δ = ±90°. At δ = 0° all thrust pushes the craft forward. At δ = ±90° all thrust produces turning torque about the centre of mass. The servo pivot is 65 mm from the rear edge, so the fan's swept circle (radius 61 mm) stays inside the deck outline.

### Skirt Aerodynamics and Stability

A bag-style polyethylene skirt runs around the deck perimeter and bulges about 10 mm beyond the deck edge. Air from the lift fan pressurizes the space under the deck. The lift force acts at the centroid of the cushion area, which is the deck centre, not at the lift-fan location. This is why the components are balanced around the deck centre. The flexible skirt also conforms to the floor obstacles as the craft passes over them.

### Autonomous Navigation Suite

| Sensor | Location | Purpose |
|---|---|---|
| HC-SR04 ultrasonic | Front edge, facing forward | Detects the end wall to trigger each U-turn |
| GP2Y0A21 IR (side) | Centreline, on the controller enclosure lid, facing sideways | Wall-following and detecting the exit opening |
| GP2Y0A21 IR (top) | Left of the lift fan, about 4 cm ahead of the COM, facing up | Detects the overhead bar for the stop |
| GY-521 (MPU-6050) IMU | Right of the lift fan | Heading hold and controlled turns to exact angles (not counted in N<sub>c</sub>) |

The side IR sensor is mounted on the centreline rather than at the deck edge. This way the measured wall distance is always between 130 and 420 mm, inside the sensor's valid 100–800 mm range. It avoids the reading inversion that IR sensors show below 10 cm. The top IR sensor is placed ahead of the centre of mass by roughly the braking distance, so the craft comes to rest with its centre under the bar.

### Power Distribution and Mass Balancing

Two Gens ace 2S 450 mAh LiPo packs are wired in series, giving 14.8 V nominal. They are placed on either side of the controller at y = ±85 mm, so they balance each other laterally. A voltage above 12 V is required by the controller's battery-monitoring input (ADC7 reads V<sub>batt</sub> − 12 V). This arrangement is to be confirmed with the lab technician.

---

## Components

| # | Component | Model | Qty | Mass (g) | Position (x, y) mm | Counts in N<sub>c</sub> | Description |
|---|---|---|---|---|---|---|---|
| 1 | Lift fan | Delta AFB1212SH | 1 | 198 | (10, 0) | Yes | Highest static pressure (107.1 Pa @ 12 V); generates the air cushion |
| 2 | Thrust fan | Sunon MEC0251V1-000U-A99 | 1 | 162 | (−115, 0) | Yes | Highest flow per gram (108.2 CFM); forward thrust and vectoring |
| 3 | Servo | HiTec HS-311 | 1 | 43 | (−115, 0) | Yes | Rotates the thrust fan ±90° for steering |
| 4 | Ultrasonic sensor | HC-SR04 | 1 | 8.5 | (168, 0) | Yes | Front wall detection, 2–400 cm range |
| 5 | IR sensor (side) | Sharp GP2Y0A21YK0F | 1 | 5 | (115, 0) | Yes | Wall-following and exit detection, 10–80 cm range |
| 6 | IR sensor (top) | Sharp GP2Y0A21YK0F | 1 | 5 | (45, 90) | Yes | Overhead bar detection, narrow beam |
| 7 | IMU | GY-521 (MPU-6050) | 1 | 5 | (45, −90) | No | Yaw, angular rate, acceleration |
| 8 | Controller | ENGR 290 board + enclosure | 1 | 150 (est.) | (115, 0) | No | Arduino Nano based control board |
| 9 | Battery | Gens ace 2S 450 mAh | 2 | 28 each | (115, ±85) | No | Two packs in series, 14.8 V nominal |
| 10 | Deck | Foam board, 20 mm | 1 | 57 − 7.5 (hole) | (0, 0) | No | Structural platform |
| 11 | Skirt | Polyethylene bag skirt | 1 | 20 | perimeter | No | Contains the air cushion |
| 12 | Fan safety nets | — | 2 | 8 each | at each fan | No | Mandatory fan protection |
| 13 | Fan–servo bracket | — | 1 | 12 | (−115, 0) | No | Mounts the thrust fan on the servo horn |
| 14 | Wiring, glue, tape | — | — | 30 (est.) | (0, 0) | No | Miscellaneous |
| | **Total** | | | **760** | | **6** | |

---

## Mathematical Evaluation and Modeling

### Modeling Assumptions

- Each component is treated as a point mass at its centre. The deck, skirt and fans also include their own inertia about their centre.
- The craft is a rigid body.
- Friction is zero while hovering. Aerodynamic drag is included in the linear simulation.
- Fan behaviour scales with voltage: airflow Q ∝ V, pressure p ∝ V², power P ∝ V³.
- The fan curve is linear: p = p<sub>max</sub>(1 − Q/Q<sub>max</sub>).
- Turns start and end at rest. The craft accelerates for half the turn and decelerates for the other half.
- Values marked "est." are to be replaced with measurements.

### 1. Mass Properties

$$M=\sum m_i = 0.760\ \text{kg}$$

$$x_{COM}=\frac{\sum m_i x_i}{M}=3.0\ \text{mm},\qquad y_{COM}=\frac{\sum m_i y_i}{M}=0\ \text{mm}$$

$$e=\sqrt{x_{COM}^2+y_{COM}^2}=3.0\ \text{mm}$$

$$I_{zz}=\sum\left[I_{self,i}+m_i\left((x_i-x_{COM})^2+(y_i-y_{COM})^2\right)\right]=0.00889\ \text{kg·m}^2$$

### 2. Geometric Compliance

$$D=\sqrt{L^2+W^2}=\sqrt{380^2+260^2}=460.4\ \text{mm}\ \le\ 480\ \text{mm}$$

$$r_{sweep}=\sqrt{\left(\tfrac{25}{2}\right)^2+60^2}=61.3\ \text{mm}$$

$$c_{spin}=\frac{500-460.4}{2}=19.8\ \text{mm}$$

### 3. Lift

$$p_c=\frac{Mg}{A_c}=\frac{0.760(9.81)}{0.0864}=86.3\ \text{Pa}$$

$$p_{max}=107.1\left(\frac{13.8}{12}\right)^2=141.6\ \text{Pa},\qquad Q_{max}=113.1\left(\frac{13.8}{12}\right)=130.1\ \text{CFM}$$

$$Q=Q_{max}\left(1-\frac{p_c}{p_{max}}\right)=130.1\left(1-\frac{86.3}{141.6}\right)=50.8\ \text{CFM}=0.0240\ \text{m}^3/\text{s}$$

$$v_{esc}=\sqrt{\frac{2p_c}{\rho}}=\sqrt{\frac{2(86.3)}{1.204}}=11.97\ \text{m/s}$$

$$h=\frac{Q}{C_d\,P\,v_{esc}}=\frac{0.0240}{0.6(1.28)(11.97)}=2.61\ \text{mm}$$

$$h_{12V}=1.13\ \text{mm}$$

### 4. Thrust and Thrust Vectoring

$$A=\frac{\pi}{4}\left(d_{tip}^2-d_{hub}^2\right)=\frac{\pi}{4}\left(0.116^2-0.045^2\right)=0.00898\ \text{m}^2$$

$$v_{jet}=\frac{Q}{A}=\frac{0.05107}{0.00898}=5.69\ \text{m/s}$$

$$F=k\,\rho\,Q\,v_{jet}=0.85(1.204)(0.05107)(5.69)=0.297\ \text{N}$$

$$F_x=F\cos\delta,\qquad \tau=F\sin\delta\cdot r$$

$$\tau_{max}=F\,r=0.297(0.118)=0.0351\ \text{N·m}$$

### 5. Linear Dynamics

$$M\frac{dv}{dt}=F-\tfrac12\rho C_D A_f v^2$$

$$a=\frac{F}{M}=\frac{0.297}{0.760}=0.391\ \text{m/s}^2$$

$$t=\sqrt{\frac{2x}{a}}=\sqrt{\frac{2(2.35)}{0.391}}=3.47\ \text{s}$$

$$t_{drag}=3.50\ \text{s},\qquad v_{2.35}=1.32\ \text{m/s}$$

$$d_{brake}=v\,t_d+\frac{v^2}{2\mu g}=0.5(0.3)+\frac{0.5^2}{2(0.35)(9.81)}=0.186\ \text{m}$$

### 6. Rotational Dynamics

$$\alpha=\frac{\tau_{max}}{I_{zz}}=\frac{0.0351}{0.00889}=3.94\ \text{rad/s}^2$$

$$t_\theta=2\sqrt{\frac{\theta}{\alpha}}$$

$$t_{90^\circ}=2\sqrt{\frac{\pi/2}{3.94}}=1.26\ \text{s},\qquad t_{180^\circ}=2\sqrt{\frac{\pi}{3.94}}=1.78\ \text{s}$$

$$\tau_{lift}=\frac{\eta P}{\omega}=\frac{0.5(9.67)}{409.7}=0.0118\ \text{N·m}$$

$$\delta_{trim}=\sin^{-1}\!\left(\frac{\tau_{lift}}{F\,r}\right)=\sin^{-1}\!\left(\frac{0.0118}{0.0351}\right)=19.7^\circ$$

### 7. Servo Check

$$I_{fan}=\frac{m\left(w^2+d^2\right)}{12}=2.28\times10^{-4}\ \text{kg·m}^2$$

$$\tau_{req}=I_{fan}\,\alpha_s=2.28\times10^{-4}(110)=0.025\ \text{N·m}\ \ll\ \tau_{stall}=0.294\ \text{N·m}$$

### 8. Power Budget

$$P=P_{lift}+P_{thrust}+P_{5V}=9.67+5.50+5.46=20.63\ \text{W}$$

$$I=\frac{P}{V}=\frac{20.63}{14.8}=1.39\ \text{A}=3.1C\ \ll\ 30C$$

$$t_{run}=\frac{0.8\,V\,C}{P}=\frac{0.8(14.8)(0.45)}{20.63}=0.258\ \text{h}=15.5\ \text{min}$$

### 9. Course Time and Score

$$t_{lane}=3.42\ \text{s},\qquad t_{U}=t_{spin}+2t_{90^\circ}+2\sqrt{\frac{w_{lane}}{a}}=5.24\ \text{s}$$

$$t_{course}=k\left(n\,t_{lane}+(n-1)\,t_U\right)+t_{bar}+t_{exit}=32.7\ \text{s}$$

$$S=\frac{d}{N_c\,t}\times B=\frac{7.05}{6(32.7)}(1.15)(1.25)=0.052$$

### Summary of Results

| Quantity | Value |
|---|---|
| Total mass | 0.760 kg |
| Centre of mass | (3.0, 0) mm |
| COM ↔ centre of lift | 3.0 mm |
| Yaw moment of inertia | 0.00889 kg·m² |
| Cushion pressure | 86.3 Pa |
| Hover gap (12 V / 13.8 V) | 1.13 / 2.61 mm |
| Static thrust | 0.297 N |
| Max linear acceleration | 0.391 m/s² |
| Time to 2.35 m | 3.47 s (3.50 s with drag) |
| Max angular acceleration | 3.94 rad/s² |
| Time to 90° / 180° | 1.26 / 1.78 s |
| Total power / current | 20.63 W / 1.39 A |
| Run time | 15.5 min |
| Estimated score | 0.052 |
