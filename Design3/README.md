
# Design 3 — Rounded Hovercraft with Front Vectored Thrust Fan

## Description

### Technical Overview

Design 3 is an ENGR 290 hovercraft based on the SolidWorks isometric model and dimensioned assembly drawing.

Unlike Design 2, which uses a rectangular chassis and a rear-mounted thrust fan, Design 3 features a **rounded, near-oval chassis with a front-mounted vectored thrust fan**.

The design consists of:

- One central lift fan for generating the air cushion.
- One front-mounted thrust fan for propulsion and steering.
- One servo motor for thrust vectoring.
- Two angled infrared (IR) sensors for proximity detection.
- One front ultrasonic (US) sensor for distance measurement.
- One GY-521 IMU for heading stabilization.
- A rear controller enclosure.
- Two proposed Li-Po batteries for power distribution.
- A flexible skirt to maintain cushion pressure.

The objective is to achieve stable hovering, accurate autonomous navigation, controlled turning, and reliable obstacle detection while minimizing unnecessary components.

| Parameter | Design 3 |
|---|---|
| Chassis width | 320 mm |
| Chassis length | 388.6 mm |
| Shape | Rounded / near-oval |
| Lift fan | Delta AFB1212SH (provisional) |
| Thrust fan | Sunon MEC0251V1 (provisional) |
| Steering | HS-311 servo motor |
| IR sensors | 2 angled front sensors |
| Ultrasonic sensor | HC-SR04 |
| IMU | GY-521 (MPU-6050), proposed |
| Estimated mass | 0.769 kg |
| Estimated hover gap at 12 V | 2.17 mm |
| Estimated straight-line travel (2.35 m) | 4.11 s |

**Note:** Dimensions are interpreted from the SolidWorks drawing and must be checked against the original CAD units. Masses and performance results are preliminary simulation estimates.

### Hovercraft Architecture and Subsystems

The chassis is constructed from lightweight foam with a rounded perimeter and a flexible skirt underneath.

The coordinate system is defined as:

- **+x:** Forward direction, toward the front thrust fan.
- **+y:** Left side.
- **−x:** Rear direction.
- **−y:** Right side.

The lift fan is positioned close to the center of the deck to generate the air cushion.

The thrust fan and servo are installed toward the front, while the controller enclosure is located toward the rear.

Two batteries are proposed on opposite sides of the centerline to reduce lateral mass imbalance.

An IMU is proposed near the center of mass for orientation and angular-rate feedback.

#### Advantages of the Rounded Chassis

**1. Reduced corner interference**

Compared with a rectangular chassis, the rounded perimeter can reduce the risk of sharp corners contacting walls during tight turns.

**2. Improved maneuverability in narrow lanes**

The shape can have a smaller turning envelope than the diagonal of its rectangular bounding box.

**3. Reduced skirt snagging**

Curved corners may help the skirt slide past obstacles rather than catching on sharp edges.

**4. Symmetrical lateral geometry**

The nearly symmetric left-right shape helps simplify balancing and mechanical layout.

**Limitations:** The rounded skirt may be more difficult to manufacture and seal. Turning performance also depends on mass distribution, skirt friction, and the actual rotation center.

### Propulsion and Directional Control

#### Lift Fan — Delta AFB1212SH

The Delta AFB1212SH is provisionally selected because of its relatively high static pressure.

Reference specifications:

- Maximum static pressure: 107.1 Pa at 12 V.
- Maximum airflow: 113.1 CFM at 12 V.
- Estimated mass: 198 g.

Static pressure is particularly important because the fan must maintain sufficient pressure beneath the hovercraft to support its weight.

#### Thrust Fan — Sunon MEC0251V1

The Sunon MEC0251V1 is provisionally selected for horizontal propulsion.

Reference specifications:

- Maximum airflow: 108.2 CFM.
- Estimated mass: 162 g.
- Estimated thrust: 0.297 N.

The fan is vertically mounted so its airflow produces horizontal thrust.

#### Advantages of the Front Thrust Fan

**1. Steering moment arm**

The thrust fan is located ahead of the center of mass. When the servo changes its orientation, the lateral component of thrust generates a torque that rotates the hovercraft.

**2. Simplified steering system**

The servo redirects the fan thrust without requiring a separate rudder.

**3. Available rear space**

Positioning the thrust system at the front leaves space toward the rear for the controller enclosure.

**4. Accessible assembly**

The front-mounted fan and servo can be accessed for mounting, maintenance, and testing.

**Limitations:** The front assembly shifts mass forward, can transmit vibration to nearby sensors, and requires verification of servo torque and mechanical clearance.

The steering-torque sign is opposite that of Design 2's rear-mounted fan for the same lateral thrust direction.

### Skirt Aerodynamics and Stability

The flexible skirt retains pressurized air beneath the deck, reducing direct contact between the hovercraft and the floor.

The cushion pressure must generate enough force to balance the hovercraft's weight.

The mathematical model assumes:

- A rounded chassis approximated by a superellipse.
- An estimated skirt overhang of 8 mm.
- A simplified linear fan pressure-flow relationship.
- A uniform leakage gap around the skirt.

The predicted hover gap is not necessarily equal to physical obstacle clearance.

---

### Autonomous Navigation Suite

| Sensor | Placement | Purpose |
|---|---|---|
| HC-SR04 ultrasonic | Front, below/in front of the thrust-fan support | Forward wall and obstacle detection |
| Left IR sensor | Front-left, angled | Side and forward proximity detection |
| Right IR sensor | Front-right, angled | Side and forward proximity detection |
| GY-521 IMU | Proposed near center of mass | Angular-rate feedback and heading stabilization |

#### Advantages of the IMU

The GY-521 contains an MPU-6050 accelerometer and gyroscope.

**1. Improved straight-line stability**

The gyroscope measures angular velocity. The controller can detect unwanted rotation and adjust steering to maintain the desired heading.

**2. More accurate turns**

The IMU allows the controller to estimate how much the hovercraft has rotated, making controlled 90° and 180° turns more practical.

**3. Faster disturbance detection**

The gyroscope can identify rotational disturbances before they cause a large heading error.

**4. Better sensor coordination**

The IMU complements the IR and ultrasonic sensors. Distance sensors measure nearby obstacles, while the gyroscope measures the hovercraft's rotation.

**5. Improved navigation control**

An IMU-assisted feedback controller can reduce heading error and turning overshoot.

**Recommended placement**

The IMU should be fixed rigidly near the center of mass, with its axes aligned to the chassis.

It should not be installed on the rotating thrust-fan assembly.

**Limitations:** Gyroscope measurements contain bias and noise. Integrated heading estimates drift, so calibration and correction using external references are necessary.

#### Advantages of the Ultrasonic Sensor Position

In the SolidWorks design, the ultrasonic sensor is positioned at the front, below or in front of the upright thrust-fan structure.

**1. Early obstacle detection**

The sensor can measure the distance to an obstacle before the main body reaches it.

**2. Compact integration**

Its location near the thrust-fan support can reduce the need for a separate tall mounting bracket.

**3. Forward-facing measurement**

The sensor is aligned with the intended direction of travel, making it useful for detecting end walls.

**4. Autonomous turning**

When the measured distance falls below a programmed threshold, the controller can initiate a turning maneuver.

**5. Reduced rear-space requirements**

The sensor occupies the forward sensing area, keeping the rear available for electronics.

**Limitations**

The ultrasonic sensor must have an unobstructed field of view.

Its beam must not be blocked by the fan housing or rotating blades.

The sensor should also be tested while both fans operate to check vibration and measurement reliability.

#### Advantages of the Angled IR Sensors

Two angled IR sensors are installed near the front of the hovercraft.

Their arrangement provides additional proximity information toward both sides.

This can help the hovercraft detect approaching obstacles and avoid wall contact during navigation.

However, the two angled sensors may not independently satisfy any competition requirements for upward bar detection or dedicated sidewall exit detection. Their actual functions must be validated.

### Power Distribution and Mass Balancing

The simulation assumes two 2S 450 mAh Li-Po batteries connected in series:

$$
V_{battery}=7.4+7.4=14.8\text{ V}
$$

The combined nominal capacity remains:

$$
C=0.45\text{ Ah}
$$

The proposed battery positions are symmetric laterally to reduce the center-of-mass offset along the y-axis.

The actual mounting positions must be confirmed using the SolidWorks assembly and measured component masses.

Battery and fan wiring must follow the approved ENGR 290 controller connections. Batteries must not be connected directly to the fans.

---

## Components

The following quantities, masses, and positions are preliminary inputs used for mathematical simulation.

| # | Component | Model | Qty | Estimated mass | Position (x,y), mm |
|---|---|---|---:|---:|---|
| 1 | Lift fan | Delta AFB1212SH | 1 | 198 g | (0,0) |
| 2 | Front thrust fan | Sunon MEC0251V1 | 1 | 162 g | (+145,0) |
| 3 | Servo | HS-311 | 1 | 43 g | (+135,0) |
| 4 | Fan mount and guards | Custom | 1 set | 20 g | (+145,0) |
| 5 | Controller and enclosure | ENGR 290 | 1 | 150 g | (−120,0) |
| 6 | Li-Po batteries | 2S 450 mAh | 2 | 28 g each | (−25,±103) |
| 7 | Ultrasonic | HC-SR04 | 1 | 8.5 g | (+174,0) |
| 8 | Left IR | Model TBD | 1 | 5 g | (+116,+105) |
| 9 | Right IR | Model TBD | 1 | 5 g | (+116,−105) |
| 10 | IMU | GY-521 | 1 | 5 g | (−55,0) |
| 11 | Skirt | Polyethylene | 1 | 25 g | Distributed |
| 12 | Wiring and mounting | Miscellaneous | — | 30 g | Distributed |
| 13 | Foam deck | Estimated properties | 1 | Calculated | Distributed |

**Estimated total mass: approximately 0.769 kg.**

The exact competition component count must be determined using the official scoring rules.

---

## Mathematical Evaluation and Modeling

### Modeling Assumptions

The simulation assumes:

1. The hovercraft behaves approximately as a rigid body.
2. Most components are represented as concentrated masses.
3. The rounded chassis is approximated using a superellipse.
4. Fan pressure decreases approximately linearly with airflow.
5. Air density is constant at 1.204 kg/m³.
6. Aerodynamic drag and residual skirt resistance are approximated.
7. Thrust-vector steering is modeled using planar torque equations.
8. The 180° turning response is idealized.
9. Electrical power and sensor performance require experimental verification.

### 1. Mass Properties

The total hovercraft mass is:

$$
M=\sum_{i=1}^{n}m_i
$$

Using the preliminary component masses:

$$
\boxed{M\approx0.769\text{ kg}}
$$

The center of mass is:

$$
x_{COM}=\frac{\sum_i m_i x_i}{M}
$$

$$
y_{COM}=\frac{\sum_i m_i y_i}{M}
$$

The preliminary result places the center of mass approximately 19.7 mm forward of the geometric center.

$$
\boxed{x_{COM}\approx+19.7\text{ mm}}
$$

The yaw moment of inertia is:

$$
I_{zz}\approx\sum_i m_i\left[(x_i-x_{COM})^2+(y_i-y_{COM})^2\right]+I_{deck}
$$

The preliminary simulation gives:

$$
\boxed{I_{zz}\approx0.00859\text{ kg}\cdot\text{m}^2}
$$

### 2. Geometric Compliance

The interpreted chassis dimensions are:

$$
W=320\text{ mm}
$$

$$
L=388.6\text{ mm}
$$

Assuming an 8 mm skirt overhang:

$$
W_o=W+2s
$$

$$
L_o=L+2s
$$

The conservative rectangular bounding diagonal is:

$$
D_{box}=\sqrt{W_o^2+L_o^2}
$$

$$
\boxed{D_{box}\approx526\text{ mm}}
$$

Because the chassis is rounded, the rectangular bounding diagonal is not necessarily its actual turning diameter.

The simulation samples the modeled rounded boundary:

$$
D_{spin}=2\left(r_{max}+s\right)
$$

The preliminary result is:

$$
\boxed{D_{spin}\approx408\text{ mm}}
$$

For a 500 mm lane:

$$
c_{lane}=\frac{500-D_{spin}}{2}
$$

$$
\boxed{c_{lane}\approx46\text{ mm per side}}
$$

This assumes rotation about the geometric center and no additional component protrusions. The actual clearance must be measured.

### 3. Lift Performance

The hovercraft must satisfy:

$$
F_L=Mg
$$

The required cushion pressure is:

$$
P_c=\frac{Mg}{A_c}
$$

Using the modeled cushion area:

$$
\boxed{P_c\approx72.6\text{ Pa}}
$$

The fan airflow is approximated using:

$$
Q_{op}=Q_{free}\left(1-\frac{P_c}{P_{max}}\right)
$$

The leakage velocity is:

$$
v_{esc}=\sqrt{\frac{2P_c}{\rho}}
$$

The estimated hover gap is:

$$
h=\frac{Q_{op}}{C_dP_{skirt}v_{esc}}
$$

The simulation gives:

| Fan scenario | Estimated hover gap |
|---|---:|
| 12 V | 2.17 mm |
| 13.8 V sensitivity case | 3.77 mm |

The 13.8 V scenario is only a mathematical sensitivity analysis. It is not authorization to operate a fan above its rated voltage.

The estimated leakage gap does not guarantee passage over the competition obstacles.

### 4. Thrust and Thrust Vectoring

The thrust generated by the propulsion fan is approximated using momentum theory:

$$
F_T=k\rho\frac{Q_T^2}{A_{jet}}
$$

The preliminary estimate is:

$$
\boxed{F_T\approx0.297\text{ N}}
$$

The thrust components depend on steering angle:

$$
F_x=F_T\cos\delta
$$

$$
F_y=F_T\sin\delta
$$

The forward acceleration, neglecting resistance, is:

$$
a_x=\frac{F_x}{M}
$$

The yaw torque is:

$$
\tau_z=(x_T-x_{COM})F_y-(y_T-y_{COM})F_x
$$

The front position creates a steering moment about the center of mass when the thrust vector is deflected.

### 5. Linear Dynamics

Newton's second law gives:

$$
M\frac{dv}{dt}=F_T-F_{drag}-F_{skirt}
$$

Aerodynamic drag is modeled as:

$$
F_{drag}=\frac12\rho C_DA_fv|v|
$$

Therefore:

$$
\frac{dv}{dt}=\frac{F_T-\frac12\rho C_DA_fv|v|-F_{skirt}}{M}
$$

The displacement is obtained from:

$$
\frac{dx}{dt}=v
$$

Using numerical integration, the preliminary result is:

$$
\boxed{t_{2.35m}\approx4.11\text{ s}}
$$

This estimate assumes the modeled thrust and drag characteristics and does not include wall avoidance or braking.

### 6. Rotational Dynamics

The yaw response is described by:

$$
I_{zz}\frac{d\omega}{dt}=\tau_z-b_\omega\omega
$$

The heading evolves according to:

$$
\frac{d\psi}{dt}=\omega
$$

An IMU-assisted heading controller could use:

$$
e_\psi=\psi_{target}-\hat{\psi}
$$

$$
\delta_{cmd}=K_pe_\psi-K_d\omega_{gyro}
$$

This allows the controller to reduce unwanted rotation and improve turning accuracy.

The MATLAB turning-response graph is an idealized 180° maneuver. It does not yet simulate experimentally calibrated IMU feedback.

### 7. Servo Check

The required servo torque is estimated from:

$$
\tau_{servo,req}\approx I_{assembly}\alpha_{servo}+\tau_{friction}+\tau_{aero}
$$

The servo must generate sufficient torque to rotate the mounted thrust fan while overcoming mechanical and aerodynamic loads.

The fan's rotational clearance and servo torque margin must be verified physically before using large steering deflections.

### 8. Power Budget

The total electrical power is:

$$
P_{total}=P_{lift}+P_{thrust}+P_{control}
$$

Using the preliminary assumptions:

$$
P_{total}=6.36+5.50+4.00
$$

$$
\boxed{P_{total}=15.86\text{ W}}
$$

For an assumed 14.8 V supply:

$$
I_{battery}=\frac{P_{total}}{V_{battery}}
$$

$$
I_{battery}=\frac{15.86}{14.8}
$$

$$
\boxed{I_{battery}\approx1.07\text{ A}}
$$

Estimated battery runtime is:

$$
t_{run}=60\frac{0.8V_{battery}C_{Ah}}{P_{total}}
$$

$$
t_{run}=60\frac{0.8(14.8)(0.45)}{15.86}
$$

$$
\boxed{t_{run}\approx20.2\text{ min}}
$$

The result assumes 80% usable battery energy and excludes additional electrical losses and peak loads.

### 9. Course Time and Competition Score

The reference competition scoring expression is:

$$
S=\frac{d}{N_ct}B
$$

where:

- d is the reference travel distance.
- Nc is the counted component total.
- t is course completion time.
- B is the applicable bonus multiplier.

The MATLAB score simulation compares five hypothetical sensor configurations under two finishing scenarios.

The results are for sensitivity analysis only, not verified competition scores.

### Summary of Results

| Quantity | Estimated value |
|---|---:|
| Chassis width | 320 mm |
| Chassis length | 388.6 mm |
| Total mass | 0.769 kg |
| Center-of-mass offset | +19.7 mm |
| Moment of inertia | 0.00859 kg·m² |
| Cushion pressure | 72.6 Pa |
| Hover gap at 12 V | 2.17 mm |
| Hover gap at 13.8 V scenario | 3.77 mm |
| Static thrust | 0.297 N |
| Time for 2.35 m | 4.11 s |
| Modeled spin diameter | 408 mm |
| Power consumption | 15.86 W |
| Battery current | 1.07 A |
| Estimated runtime | 20.2 min |
| Competition score | Not validated |

---

## MATLAB Simulation

### MATLAB Source Code

The simulation code is located in:

[hovercraft3.m](MATLAB/hovercraft3.m)

The MATLAB simulation generates the same six figure categories as Design 2.

### Fig 1 — Top View Layout

![Fig 1 - Top view layout](Figures/Fig%201%20-%20Top%20view%20layout.png)

This figure shows the rounded chassis, lift fan, thrust fan, ultrasonic sensor, two angled IR sensors, controller, proposed IMU and battery positions, and estimated center of mass.

### Fig 2 — Lift

![Fig 2 - Lift](Figures/Fig%202%20-%20Lift.png)

The figure compares lift-fan pressure and airflow at the reference operating conditions.

It also illustrates how the estimated hover gap changes with total mass.

### Fig 3 — Thrust Vectoring

![Fig 3 - Thrust vectoring](Figures/Fig%203%20-%20Thrust%20vectoring.png)

This figure presents angular acceleration and forward acceleration as functions of servo deflection.

The analysis uses the front-mounted fan's position relative to the center of mass.

### Fig 4 — Time Responses

![Fig 4 - Time responses](Figures/Fig%204%20-%20Time%20responses.png)

This figure includes four simulations:

1. Straight-line displacement over time.
2. Forward velocity over time.
3. Idealized 180° turning response.
4. Yaw-rate response.

### Fig 5 — Size Trade

![Fig 5 - Size trade](Figures/Fig%205%20-%20Size%20trade.png)

This figure evaluates how changing the chassis dimensions affects predicted hover gap and the turning envelope.

Reference competition constraints are shown for comparison.

### Fig 6 — Score

![Fig 6 - Score](Figures/Fig%206%20-%20Score.png)

This figure compares five hypothetical sensor configurations (A–E) for two finishing scenarios.

The score calculations depend on assumed times, bonuses, and counted components, so they are not final competition predictions.

### Running the MATLAB Simulation

1. Open MATLAB.
2. Navigate to `Design3/MATLAB`.
3. Open `hovercraft3.m`.
4. Run the script.
5. Review the Command Window calculations.
6. Inspect the six generated figures.
7. Replace assumed parameters with actual measured values.
8. Upload the exported PNG figures into `Design3/Figures`.

---

## Advantages, Limitations and Recommendations

| Design feature | Advantages | Limitations |
|---|---|---|
| Rounded chassis | Reduces corner interference risk during tight turns | Actual turning clearance depends on skirt and protrusions |
| Central lift fan | Simple air cushion generation | Requires sufficient pressure and airflow |
| Front thrust fan | Steering torque without a separate rudder | Forward mass shift and vibration |
| Front ultrasonic sensor | Earlier detection of forward obstacles | Possible obstruction and vibration |
| Two angled IR sensors | Improves front-side proximity awareness | May not cover upward-bar or side-exit tasks |
| IMU | Heading stabilization and controlled turns | Drift, calibration and vibration sensitivity |
| Symmetric batteries | Improves lateral mass balance | Final positions need verification |
| Rear controller enclosure | Protects electronics and simplifies wiring organization | Added weight and cooling requirements |

### Recommended Improvements

**1. Validate lift performance**

Test actual hover clearance and the ability to cross obstacles of 1, 2 and 3 mm.

**2. Verify center of mass**

Use SolidWorks mass properties and physical measurements to locate the center of mass.

**3. Optimize the IMU placement**

Mount the IMU rigidly near the center of mass, away from excessive vibration.

**4. Test ultrasonic reliability**

Check whether the sensor receives stable readings when the thrust and lift fans operate.

**5. Validate turning performance**

Measure 90° and 180° turns and compare them with the MATLAB predictions.

**6. Confirm competition compliance**

Verify component allocation, physical dimensions, measurement rules, and sensor requirements.

**7. Update the MATLAB simulation**

Replace provisional masses, fan data, aerodynamic coefficients, and control parameters with experimental measurements.

---

## References

- [ENGR 290 Design 2 README](https://github.com/dqwed01/ENGR290/blob/Design_R/Design2/README.md)
- ENGR 290 Technical Support — Fall 2026
- Design 3 SolidWorks Isometric Model
- Design 3 SolidWorks Assembly Drawing
- [Design 3 MATLAB Simulation](MATLAB/hovercraft3.m)

*Status: Preliminary Design Review (PDR). All calculated performance results are estimates requiring experimental validation.*
