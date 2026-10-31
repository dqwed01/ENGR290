
# Design 3 — Rounded-Front Hovercraft with Front Vectored Thrust

## Description

### Technical Overview

Design 3 is a foam-deck hovercraft featuring a rounded front, square rear corners, one central lift fan, and one front-mounted thrust fan controlled by a servo motor. Two Li-Po batteries are positioned symmetrically on either side of the lift fan to improve lateral stability.

The design also incorporates an ultrasonic sensor (US), three infrared sensors (IR), an inertial measurement unit (IMU), and a controller located toward the rear.

Unlike Design 2, which uses a rear-mounted thrust fan, Design 3 positions the propulsion system toward the front. This configuration changes the center of mass and steering dynamics and therefore requires an independent mathematical evaluation.

The objective is to develop a lightweight, stable hovercraft capable of navigating the ENGR 290 competition course while satisfying the dimensional, component, and obstacle-clearance requirements.

| Parameter | Original design | Recommended simulation |
|---|---:|---:|
| Foam deck | 330 × 350 mm | 310 × 330 mm |
| Skirt overhang | 8 mm | 8 mm |
| Overall dimensions | 346 × 366 mm | 326 × 346 mm |
| Bounding diagonal | 503.7 mm | 475.4 mm |
| Reference diagonal limit | 480 mm | 480 mm |
| Estimated mass | Not measured | 0.768 kg |
| Lift fans | 1 | 1 |
| Thrust fans | 1 | 1 |
| Steering servos | 1 | 1 |
| IR sensors | 3 proposed | 3 proposed |

**Note:** The 480 mm diagonal limit is based on Design 2's competition reference. Actual compliance must be verified against the official competition rules.

### Hovercraft Architecture and Subsystems

The chassis is manufactured from lightweight foam board with a rounded front and rectangular rear.

The coordinate system is defined with its origin at the geometric center of the deck:

- **+x:** Forward direction
- **+y:** Left side
- **−x:** Rear direction
- **−y:** Right side

The lift fan is positioned at the center to generate the air cushion.

The thrust fan and steering servo are positioned toward the front, while the controller is located at the rear.

Two batteries are placed symmetrically beside the lift fan to balance the lateral mass distribution.

The IMU is located between the lift fan and the controller, close to the centerline.

### Propulsion and Directional Control

**Lift fan — Delta AFB1212SH**

The Delta AFB1212SH is provisionally selected because of its relatively high static pressure.

Reference characteristics:

- Maximum static pressure: 107.1 Pa at 12 V
- Maximum airflow: 113.1 CFM at 12 V
- Estimated mass: 198 g

Static pressure is particularly important because the lift fan must maintain sufficient pressure beneath the hovercraft to support its weight.

**Thrust fan — Sunon MEC0251V1-000U-A99**

The thrust fan is selected for its airflow-to-mass characteristics.

Reference characteristics:

- Maximum airflow: 108.2 CFM
- Estimated mass: 162 g
- Function: Forward propulsion and steering

The fan exhaust is directed toward the rear to produce forward motion.

**Steering — HS-311 Servo**

The HS-311 servo rotates the thrust fan horizontally.

The preliminary simulation assumes:

\[
-30^\circ \leq \delta \leq 30^\circ
\]

At zero steering angle, the thrust acts along the forward axis.

At nonzero steering angles, the thrust is divided into longitudinal and lateral components, producing forward acceleration and rotational motion.

Because the thrust fan is positioned in front of the center of mass, the steering torque has a different sign from Design 2's rear-mounted configuration.

### Skirt Aerodynamics and Stability

A lightweight flexible skirt is attached around the chassis perimeter.

The skirt retains pressurized air beneath the deck, reducing friction with the floor.

The mathematical model assumes an 8 mm skirt overhang.

The lift fan generates a pressure difference sufficient to counteract the hovercraft's weight:

\[
F_L = P_c A_c
\]

For stable hovering:

\[
F_L \geq Mg
\]

The center of lift is approximated by the centroid of the cushion area.

Maintaining the center of mass close to the center of lift helps reduce unwanted pitching and improves stability.

### Autonomous Navigation Suite

| Sensor | Location | Function |
|---|---|---|
| HC-SR04 ultrasonic | Front, above thrust-fan support | Forward obstacle detection |
| IR sensor 1 | Front-left | Proximity detection |
| IR sensor 2 | Front-center | Forward proximity detection |
| IR sensor 3 | Front-right | Proximity detection |
| GY-521 MPU-6050 | Between lift fan and controller | Heading, angular velocity and acceleration |

The ultrasonic sensor measures the distance to obstacles ahead of the craft.

The IR sensors provide shorter-range proximity measurements.

The IMU measures angular velocity and acceleration, allowing the controller to estimate orientation and correct unwanted rotation.

**Important:** The illustrated three-IR configuration is provisional. The available number of sensors and the required side-facing or upward-facing sensing for competition tasks must be confirmed.

### Power Distribution and Mass Balancing

The preliminary model assumes two 2S 450 mAh Li-Po batteries connected in series:

\[
V_{battery}=7.4+7.4=14.8\text{ V}
\]

The batteries are placed symmetrically at:

\[
(x_1,y_1)=(0,+113)\text{ mm}
\]

\[
(x_2,y_2)=(0,-113)\text{ mm}
\]

This arrangement reduces the lateral center-of-mass offset.

Fan supply voltages and battery connections must be verified with the laboratory technician. The batteries must not be connected directly to the fans.

---

## Components

The following table contains preliminary simulation values. Component masses and positions should be verified during assembly.

| # | Component | Model | Qty | Mass (g) | Position (x,y) mm | Purpose |
|---|---|---|---:|---:|---|---|
| 1 | Lift fan | Delta AFB1212SH | 1 | 198 | (0,0) | Air cushion |
| 2 | Thrust fan | Sunon MEC0251V1 | 1 | 162 | (+115,0) | Propulsion |
| 3 | Servo motor | HS-311 | 1 | 43 | (+115,0) | Steering |
| 4 | Fan bracket and guards | Custom | 1 set | 20 est. | (+115,0) | Mounting and protection |
| 5 | Batteries | 2S 450 mAh Li-Po | 2 | 28 each | (0,±113) | Power |
| 6 | Controller | ENGR 290 controller | 1 | 150 est. | (−120,0) | Processing and control |
| 7 | Ultrasonic sensor | HC-SR04 | 1 | 8.5 | (+145,0) | Distance measurement |
| 8 | IR sensors | Model TBD | 3 proposed | 5 each | Front | Proximity sensing |
| 9 | IMU | GY-521 MPU-6050 | 1 | 5 | (−92,0) | Orientation |
| 10 | Skirt | Flexible polyethylene | 1 | 20 | Perimeter | Cushion |
| 11 | Wiring and mounting | Miscellaneous | — | 30 | Distributed | Assembly |
| 12 | Foam deck | Lightweight foam | 1 | 60.1 | (0,0) | Chassis |
| | **Estimated total** | | | **767.6 g** | | |

The number of counted competition components must be evaluated using the official scoring rules.

---

## Mathematical Evaluation and Modeling

### Modeling Assumptions

The following assumptions are used:

1. The hovercraft is modeled as a rigid body.
2. Components are approximated as concentrated masses.
3. The deck is approximated as rectangular for the first-order cushion model.
4. The lift fan follows an approximately linear pressure-flow relationship.
5. Air density is assumed constant at 1.204 kg/m³.
6. Aerodynamic drag is included in the straight-line simulation.
7. Residual skirt drag is approximated.
8. Steering is modeled as vectored thrust.
9. The MATLAB simulation uses estimated parameters that require experimental calibration.

### 1. Mass Properties

The total hovercraft mass is:

\[
M=\sum_{i=1}^{n}m_i
\]

Using the preliminary component masses:

\[
\boxed{M\approx0.768\text{ kg}}
\]

The center of mass is calculated using:

\[
x_{COM}=\frac{\sum m_i x_i}{M}
\]

\[
y_{COM}=\frac{\sum m_i y_i}{M}
\]

For Design 3:

\[
\boxed{x_{COM}\approx+13.8\text{ mm}}
\]

\[
\boxed{y_{COM}\approx0\text{ mm}}
\]

The center of mass lies slightly ahead of the geometric center because of the front thrust fan and servo assembly.

The yaw moment of inertia is:

\[
I_{zz}=\sum_i\left[I_{self,i}+m_i\left((x_i-x_{COM})^2+(y_i-y_{COM})^2\right)\right]
\]

The preliminary model gives:

\[
\boxed{I_{zz}\approx0.00858\text{ kg}\cdot\text{m}^2}
\]

### 2. Geometric Compliance

The recommended chassis dimensions are:

\[
W=310\text{ mm},\qquad L=330\text{ mm}
\]

Assuming 8 mm skirt overhang on each side:

\[
W_o=W+2(8)=326\text{ mm}
\]

\[
L_o=L+2(8)=346\text{ mm}
\]

The conservative diagonal is:

\[
D=\sqrt{W_o^2+L_o^2}
\]

\[
D=\sqrt{326^2+346^2}
\]

\[
\boxed{D\approx475.4\text{ mm}}
\]

Under the reference 480 mm requirement:

\[
475.4<480\text{ mm}
\]

The estimated dimensional margin is:

\[
480-475.4=4.6\text{ mm}
\]

The original 330 × 350 mm design, with the same skirt overhang, has a diagonal of approximately 503.7 mm and would exceed this reference limit.

### 3. Lift

For hovering equilibrium:

\[
F_L=Mg
\]

The required cushion pressure is:

\[
P_c=\frac{Mg}{A_c}
\]

Using the rectangular cushion approximation:

\[
A_c=0.310(0.330)=0.1023\text{ m}^2
\]

\[
P_c=\frac{0.768(9.81)}{0.1023}
\]

\[
\boxed{P_c\approx73.6\text{ Pa}}
\]

The fan is modeled using:

\[
Q_{op}=Q_{max}\left(1-\frac{P_c}{P_{max}}\right)
\]

The leakage velocity is:

\[
v_{esc}=\sqrt{\frac{2P_c}{\rho}}
\]

The estimated hover gap is:

\[
h=\frac{Q_{op}}{C_d P_{skirt}v_{esc}}
\]

The simplified 12 V model predicts:

\[
\boxed{h\approx1.87\text{ mm}}
\]

This is an idealized leakage-gap estimate, not a verified physical hover height.

### 4. Thrust and Thrust Vectoring

The effective fan outlet area is:

\[
A_j=\frac{\pi}{4}(d_{tip}^2-d_{hub}^2)
\]

The estimated thrust is:

\[
F_T=k\rho\frac{Q_T^2}{A_j}
\]

Using the reference thrust-fan parameters:

\[
\boxed{F_T\approx0.297\text{ N}}
\]

For servo angle \(\delta\):

\[
F_x=F_T\cos\delta
\]

\[
F_y=F_T\sin\delta
\]

The longitudinal acceleration is:

\[
a_x=\frac{F_x-F_{drag}}{M}
\]

The yaw torque is:

\[
\tau_z=(x_T-x_{COM})F_y-(y_T-y_{COM})F_x
\]

The front-mounted thrust fan produces a yaw moment about the center of mass as its steering angle changes.

### 5. Linear Dynamics

Newton's second law gives:

\[
M\frac{dv}{dt}=F_T-F_{drag}-F_{skirt}
\]

Aerodynamic drag is approximated by:

\[
F_{drag}=\frac12\rho C_D A_f v|v|
\]

The governing differential equations are:

\[
\frac{dx}{dt}=v
\]

\[
\frac{dv}{dt}=\frac{F_T-\frac12\rho C_D A_fv|v|-F_{skirt}}{M}
\]

The MATLAB script integrates these equations numerically to estimate velocity, displacement, and travel time.

A reference straight-line distance of 2.35 m is used for comparison with Design 2.

### 6. Rotational Dynamics

Rotational motion is governed by:

\[
I_{zz}\frac{d\omega}{dt}=\tau_z-b_\omega\omega
\]

where:

- \(I_{zz}\) is yaw moment of inertia.
- \(\omega\) is angular velocity.
- \(\tau_z\) is steering torque.
- \(b_\omega\) is rotational damping.

Heading is calculated using:

\[
\frac{d\psi}{dt}=\omega
\]

A simplified heading controller is:

\[
\delta=K_p e_\psi+K_i\int e_\psi\,dt-K_d\omega
\]

The steering command is limited to ±30° in the preliminary model.

MATLAB simulates a heading command changing from 0° to 90° at \(t=3\) seconds.

### 7. Servo Check

The steering servo rotates the thrust fan and mounting assembly.

Required servo torque is approximated by:

\[
\tau_{servo}=I_{assembly}\alpha_{servo}+\tau_{bearing}+\tau_{aero}
\]

The servo must provide sufficient torque to rotate the fan assembly while overcoming mechanical and aerodynamic resistance.

The HS-311 servo is the initial candidate, subject to torque verification.

The steering angle must also be limited to avoid mechanical interference with nearby components.

### 8. Power Budget

Total power consumption is:

\[
P_{total}=P_{lift}+P_{thrust}+P_{control}
\]

Battery current is estimated by:

\[
I_{battery}=\frac{P_{total}}{V_{battery}}
\]

Battery runtime is approximated by:

\[
t_{run}=\frac{0.8V_{battery}C_{Ah}}{P_{total}}
\]

For the preliminary battery arrangement:

\[
V_{battery}=14.8\text{ V}
\]

\[
C=0.45\text{ Ah}
\]

The usable capacity factor of 0.8 is an assumption.

Actual runtime must be determined from measured current and the permitted operating voltage.

### 9. Course Time and Score

The reference score relationship used in Design 2 is:

\[
S=\frac{d}{N_c t}B
\]

For Design 2's example score scenario:

\[
S=\frac{7.05}{N_c t}(1.15)(1.25)
\]

where:

- \(d\): Reference travel distance.
- \(N_c\): Number of counted components.
- \(t\): Course completion time.
- \(B\): Combined scoring multiplier.

The Design 3 simulation examines score sensitivity for different completion times and component counts.

A final numerical competition score cannot yet be calculated reliably because the component count, complete course navigation, turning times, and obstacle performance remain unverified.

### Summary of Preliminary Results

| Quantity | Design 3 |
|---|---:|
| Recommended deck | 310 × 330 mm |
| Overall with skirt | 326 × 346 mm |
| Conservative diagonal | 475.4 mm |
| Total estimated mass | 0.768 kg |
| Center of mass | (+13.8, 0) mm |
| Yaw moment of inertia | 0.00858 kg·m² |
| Cushion pressure | 73.6 Pa |
| Estimated hover gap at 12 V | 1.87 mm |
| Estimated static thrust | 0.297 N |
| Steering range | ±30° assumed |
| Battery voltage | 14.8 V assumed |
| Competition score | Not yet determined |

---

## Design Advantages and Limitations

| Advantages | Limitations |
|---|---|
| Central lift fan provides simple cushion generation | Actual cushion performance depends on skirt geometry |
| Symmetric batteries improve lateral mass balance | Front thrust assembly shifts center of mass forward |
| Front vectored thrust permits directional control | Steering sign differs from a rear-mounted fan |
| Rounded front may reduce corner interference | Reduced cushion area must be accounted for |
| IMU supports heading stabilization | Sensor drift and calibration errors |
| Multiple IR sensors provide wider proximity coverage | Sensor quantity and availability must be confirmed |
| Reduced dimensions improve geometric compliance | Limited dimensional margin |
| Two-fan architecture limits propulsion complexity | Fan thrust and lift must be tested experimentally |

---
*Status: Preliminary design and mathematical simulation for the ENGR 290 Preliminary Design Review (PDR). Results require experimental validation.*
