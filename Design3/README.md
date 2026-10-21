# Design 3 — Rounded-Front Hovercraft with Front Vectored Thrust

## Description

### Technical Overview
Design 3 is a foam-deck hovercraft with a **rounded nose, square rear corners**, one central lift fan, and **one front-mounted thrust fan controlled by a steering servo**. Two identical Li-Po batteries sit symmetrically on the left and right of the lift fan. A controller at the rear and an IMU behind the lift fan complete the main layout. The concept drawing includes **three front IR sensors** and one front ultrasonic (US) sensor.

The objective is to obtain adequate cushion lift, forward propulsion, obstacle sensing, and repeatable turns within the ENGR 290 competition's dimensional and component constraints. In contrast to Design 2, which has a **rear-mounted** vectored thrust fan, Design 3 mounts propulsion **ahead of its center of mass**. Its steering dynamics and weight distribution must therefore be checked independently.

| Parameter | Original drawing | Simulation baseline |
|---|---:|---:|
| Foam deck, width × length | 330 × 350 mm | **310 × 330 mm** |
| Estimated skirt overhang (each side) | 8 mm | 8 mm |
| Bounding dimensions with skirt | 346 × 366 mm | **326 × 346 mm** |
| Bounding-box diagonal | 503.7 mm | **475.4 mm** |
| Reference diagonal limit (from Design 2 README) | 480 mm | 480 mm |
| Geometric check against that limit | **Fail** | **Pass, ~4.6 mm margin** |
| Mass | Not measured | **~0.768 kg, model estimate** |
| IR sensor count | 3 illustrated | 3 modeled; availability unconfirmed |

**Important:** The 480 mm limit and the reference course geometry are carried over from Design 2's documentation. Confirm the official competition rules and whether any attachments protrude beyond the defined envelope. A rectangular bounding diagonal is conservative for the rounded nose but does not guarantee compliance if a fan, bracket, wire, or skirt extends farther.

### Hovercraft Architecture and Subsystems
A body-fixed coordinate system is used: **+x points toward the rounded front**, **+y points left**, and the origin is at the deck's geometric center. In the MATLAB layout, the lift fan is centered at (0, 0) mm; the propulsion fan and servo pivot are near (+115, 0) mm. The left and right batteries are at (0, +113) and (0, −113) mm. The rear controller is at (−120, 0) mm and the IMU at (−92, 0) mm. The three IR sensors and US module face or occupy the front of the craft as illustrated, although physical clearances and sensing directions remain to be finalized.

### Propulsion and Directional Control
- **Lift:** One Delta **AFB1212SH**, provisionally selected for relatively high static pressure. The model uses **107.1 Pa maximum static pressure and 113.1 CFM free-air flow at 12 V**, based on Design 2's reference parameters. The actual operating point depends on pressure losses, the skirt, and the fan curve.
- **Thrust:** One Sunon **MEC0251V1-000U-A99**, provisionally selected for its airflow-to-mass characteristics. Its exhaust must point **toward the rear** to move the craft toward its rounded nose.
- **Steering:** One **HS-311** servo rotates the thrust direction in the horizontal plane. The initial MATLAB simulation limits steering to **±30°** as a placeholder, not a verified mechanical limit. Because the fan is mounted in front of the center of mass, its lateral thrust generates a yaw moment from the **forward lever arm**. A secure bracket must keep the assembly clear of the deck, other sensors, and its own safety guard across the full rotation.

### Skirt Aerodynamics and Stability
The assumed lightweight flexible skirt retains air under the deck and extends about 8 mm beyond the foam on each side. The model approximates the rounded deck as a rectangular cushion for area and perimeter calculations, so its pressure and hover-gap estimates need correction after the actual shape and skirt are built. The lift fan's position is **not** automatically the center of lift; with a symmetric cushion, the lift resultant is approximately at the cushion's geometric centroid. The forward fan and rear controller create a longitudinal mass balance that must be measured.

### Autonomous Navigation Suite

| Sensor | Planned position | Intended purpose | Limitation |
|---|---|---|---|
| HC-SR04 US | Front, centered, above/in front of the thrust-fan support | Forward obstacle/end-wall distance | Avoid obstruction, airflow vibration, and fan interference |
| IR front-left | Rounded front, left side | Nearby wall/obstacle detection | Verify range and alignment |
| IR front-center | Rounded front, centered | Forward proximity detection | Possible sensing overlap with US |
| IR front-right | Rounded front, right side | Nearby wall/obstacle detection | Verify range and alignment |
| GY-521 / MPU-6050 IMU | On centerline, behind lift fan | Yaw rate, tilt, heading estimation | Gyro heading drifts; requires calibration |

**Competition-task caveat:** Design 2's README uses a **side-facing IR** for wall following/exit detection and an **upward-facing IR** for overhead-bar detection. The three forward-looking IR sensors in Design 3's sketch do **not** automatically satisfy those functions. Before freezing the sensor plan, verify allowed sensor quantities and reorient one sensor upward and/or one sideways if required by the course. The initial materials screenshot listed only **one** IR sensor.

### Power Distribution and Mass Balancing
The simulation assumes **two 2S 450 mAh packs in series** (14.8 V nominal) and models fans at **12 V**, but this is **not an approved wiring diagram**. Confirm with the course technician how the chosen controller distributes voltage and PWM, the battery connectors, fan ratings, and the approved series connection. Route power **through the controller** and never connect the batteries directly to a fan. Use protective fan covers and inspect Li-Po packs before operation.

---

## Components

Values below are the **current `hovercraft3.m` simulation inputs**, not a finalized bill of materials. Coordinate values are from the deck center; **mass estimates must be replaced with measured values**.

| # | Component | Model or assumption | Qty | Model mass (g) | Position (x, y) mm | Purpose |
|---|---|---|---:|---:|---|---|
| 1 | Lift fan | Delta AFB1212SH | 1 | 198 | (0, 0) | Cushion pressure |
| 2 | Thrust fan | Sunon MEC0251V1-000U-A99 | 1 | 162 | (+115, 0) | Forward and vectored thrust |
| 3 | Steering servo | HS-311 | 1 | 43 | (+115, 0) | Fan-direction control |
| 4 | Thrust bracket and guards | Custom | 1 set | 20 est. | (+115, 0) | Safe fan mounting |
| 5 | Batteries | 2S 450 mAh Li-Po | 2 | 28 each est. | (0, ±113) | Energy, symmetric loading |
| 6 | Controller and enclosure | ENGR 290 controller | 1 | 150 est. | (−120, 0) | Motor/sensor control |
| 7 | Ultrasonic sensor | HC-SR04 | 1 | 8.5 est. | (+145, 0) | Front distance |
| 8 | IR sensors | Model to be confirmed | 3 planned | 5 each est. | (+151, 0), (+118, ±110) | Proximity |
| 9 | IMU | GY-521 (MPU-6050) | 1 | 5 est. | (−92, 0) | Attitude and yaw |
| 10 | Skirt | Flexible lightweight material | 1 | 20 est. | Perimeter | Air cushion |
| 11 | Wiring/glue/mounting | Miscellaneous | — | 30 est. | Distributed | Assembly |
| 12 | Foam deck (net of fan hole) | 20 mm foam, 33 kg/m³ assumed | 1 | ~60.1 est. | (0, 0) | Structure |
| | **Total model estimate** | | | **~767.6 g** | | Not weighed |

**Component-score note:** Do not transfer Design 2's reported `N_c = 6` to Design 3. The competition's counting method and allowance for three IR sensors must be checked before claiming a score. No part choice or quantity is guaranteed available until confirmed with the laboratory.

---

## Mathematical Evaluation and Modeling

### Modeling Assumptions
- The foam deck is approximated as a rectangular plate with a circular fan cutout. Rounded corners are not subtracted from the mass or cushion area.
- Components are represented as concentrated masses plus approximate individual rectangular moments of inertia.
- The hovercraft behaves as a rigid planar body; vertical bounce, skirt deformation, and wall collisions are not simulated.
- The lift fan is represented by a **linear pressure–flow curve**; it is a first estimate, not measured fan performance.
- Vectored thrust is estimated from a momentum-flow approximation with an assumed installation factor.
- Straight-line motion includes quadratic aerodynamic drag and a small residual skirt drag; the 2D steering test uses linear damping.
- All estimated values must be calibrated with physical tests.

### 1. Mass Properties

The estimated total mass and center of mass are

$$M=\sum_i m_i, \qquad x_{COM}=\frac{\sum_i m_i x_i}{M},\qquad y_{COM}=\frac{\sum_i m_i y_i}{M}.$$

For the nominal 310 × 330 mm model, the current inputs give approximately **M = 0.768 kg**, **xCOM = +13.8 mm**, and **yCOM ≈ 0 mm**. Thus, the mass center is offset **toward the front**, rather than exactly over the cushion center.

$$I_{zz}=\sum_i \left[I_{self,i}+m_i\left((x_i-x_{COM})^2+(y_i-y_{COM})^2\right)\right].$$

### 2. Geometric Compliance

For a conservative rectangular envelope including skirt overhang $s$:

$$W_o=W+2s,\qquad L_o=L+2s,\qquad D=\sqrt{W_o^2+L_o^2}.$$

For the baseline: $D=\sqrt{326^2+346^2}=\mathbf{475.4\ mm}$. For the original drawing plus the same overhang: $D=\sqrt{346^2+366^2}=\mathbf{503.7\ mm}$.

### 3. Lift and Cushion

$$A_c\approx WL,\qquad p_c=\frac{Mg}{A_c}.$$

For the 310 × 330 mm deck, the rectangular-area approximation gives $A_c=0.1023\ \text{m}^2$ and $p_c\approx73.6\ \text{Pa}$.

$$Q_{op}=Q_{free}\max\!\left(0,1-\frac{p_c}{p_{max}}\right),$$

$$v_{leak}=\sqrt{\frac{2p_c}{\rho}},\qquad h=\frac{Q_{op}}{C_d P_{skirt}v_{leak}}.$$

The resulting $h$ is an **idealized uniform leakage gap**, **not a guaranteed height above a 3 mm obstacle**. It does not account for compressed skirt geometry or fan inlet losses.

### 4. Thrust and Thrust Vectoring

$$A_j=\frac{\pi}{4}(d_{tip}^2-d_{hub}^2),\qquad F_T\approx k\rho\frac{Q_T^2}{A_j}.$$

For servo deflection $\delta$:

$$F_x=F_T\cos\delta,\qquad F_y=F_T\sin\delta.$$

Because Design 3's propulsion fan is **in front of** the mass center:

$$\tau_z=(x_T-x_{COM})F_y-(y_T-y_{COM})F_x.$$

The **sign of yaw torque** is different from a rear-mounted fan using the same deflection convention. Control directions must be checked on the real hardware.

### 5. Straight-Line Motion

$$M\frac{dv}{dt}=F_T-\tfrac12\rho C_D A_f v|v|-F_{skirt},\qquad \frac{dx}{dt}=v.$$

The code integrates these equations numerically to estimate the time and speed over the **2.35 m reference distance** used by Design 2. This is not a measured track result.

### 6. Rotational and Heading Dynamics

$$I_{zz}\frac{d\omega}{dt}=\tau_z-b_\omega\omega,\qquad\frac{d\psi}{dt}=\omega.$$

The illustrative controller applies a bounded steering command using heading error, integral correction, and yaw-rate damping:

$$\delta=\operatorname{sat}_{\pm30^\circ}\left(K_p e_\psi+K_i\int e_\psi\,dt-K_d\omega\right).$$

In the sample simulation, the commanded heading steps from **0° to 90° at t = 3 s**. This is a steering demonstration, **not a complete lane-following/U-turn algorithm**.

### 7. Electrical Power and Runtime

$$P_{tot}=P_{lift}+P_{thrust}+P_{control},\qquad I_{pack}\approx\frac{P_{tot}}{V_{pack}}.$$

$$t_{run}\approx\frac{0.8V_{pack}C_{Ah}}{P_{tot}}.$$

The 80% capacity factor, constant power demand, 14.8 V pack and 450 mAh capacity are assumptions. Measured current, actual fan duty cycle and safe low-voltage shutdown govern practical runtime.

### 8. Competition Performance and Score

Unlike Design 2's README, **this model does not yet predict a full competition time or final score**. It lacks a validated obstacle-clearance model, programmed wall-following, bar detection, path planning, turn completion logic, and a confirmed counted-component total. Once the rules and measurements are available, a scoring section can be added with explicit assumptions.

### Summary of Preliminary Results

| Quantity | Baseline design status |
|---|---|
| Deck | 310 × 330 mm proposed |
| With modeled skirt | 326 × 346 mm |
| Conservative diagonal | 475.4 mm |
| Estimated mass | 0.768 kg |
| Estimated center of mass | (+13.8, 0) mm |
| Required ideal cushion pressure | 73.6 Pa |
| Lift, thrust, travel time, yaw response | Computed by `hovercraft3.m`; must be validated |
| Steering range | ±30° assumed |
| Battery arrangement | Two 2S packs in series, **subject to technician approval** |
| Competition score | Not yet determined |

---

## MATLAB Simulation

### File
`MATLAB/hovercraft3.m` (or `hovercraft3.m` if the file is stored beside this README).

### Instructions
1. Open MATLAB **R2016b or newer**.
2. Open `hovercraft3.m` and run it. No additional toolboxes are required.
3. Keep `useRecommended = true` for the reduced deck; set it to `false` to analyze the original **330 × 350 mm** drawing.
4. Check the Command Window for **geometry, mass/COM, lift, thrust, straight-run and battery** calculations.
5. Inspect the **six generated figures**: top view; lift fan and hover-gap sensitivity; thrust and steering acceleration; straight run; 2D heading response; geometry study.
6. Replace all assumed masses, pressure–flow properties, mounting coordinates, skirt coefficients, thrust efficiency, damping and power consumption with measurements before reporting verified results.

**Known model limitations:** Figure 1 uses a simplified rounded rectangle rather than the exact front-only rounded geometry. Components may overlap geometrically in this initial plot (especially the centrally located US/IR and the front thrust assembly). The thrust fan's rotated swept envelope, skirt behavior, sensor visibility, Li-Po electrical compatibility, and full course navigation remain to be checked physically.

## Design Advantages and Risks

| Advantages | Trade-offs / verification needed |
|---|---|
| Central lift fan offers a simple cushion layout | Rounded corners and skirt leaks affect actual cushion area |
| Symmetric batteries minimize sideways COM error | Heavy forward thrust unit pulls COM toward the nose |
| Front thrust vectoring enables yaw control in principle | Different torque direction than rear steering; potential nose swing and bracket interference |
| Wide front sensor field | Three IR sensors may not be available or permitted; insufficient upward sensing for a bar |
| Reduced deck can fit the reference diagonal | Only a few millimeters of assumed margin; attachments must be included |
| Single lift + single thrust fan minimizes propulsion complexity | Actual lift margin and thrust force must be measured |

## Next Validation Tasks

- Confirm the **official dimensional limits**, course tasks, and component-count rules.
- Confirm the **number and models of IR sensors** issued to the team and decide which face forward, sideways, and upward.
- Weigh every component and measure **center of mass** with the two packs installed.
- Measure the selected **lift fan pressure/flow**, **thrust force**, hover clearance, and power draw at permitted supply settings.
- Verify fan protection, controller-side battery wiring, and mechanical sweep of the **front servo-driven propulsion unit**.
- Measure 90°/180° turning behavior; tune the MATLAB damping and heading controller before implementing real navigation.

## References

- [ENGR290 Design 2 README](https://github.com/dqwed01/ENGR290/blob/Design_R/Design2/README.md) — organization and preliminary reference values.
- `290_tech_intro_F2026` — course technical support document, available fans, batteries, servos, sensors, controller connections, and safety restrictions.
- Design 3 top/front blueprint provided by the team.

*Status: preliminary engineering model for PDR development; not a tested final design.*

