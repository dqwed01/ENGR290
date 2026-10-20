%% =====================================================================
%  ENGR 290 - Autonomous Hovercraft : "Design R" (rectangular, single
%  lift fan + single rear vectored thrust fan)
%  Analytical model for the PDR
%
%  Sections
%    0  User options
%    1  Physical constants + fan database (datasheet values)
%    2  Geometry (deck, skirt, track)
%    3  Component list (mass + position)  -> top-view layout
%    4  Mass properties : total mass, centre of mass, yaw inertia Izz
%    5  Geometric compliance checks (48 cm rule, fan sweep, turn room)
%    6  Lift : cushion pressure, fan operating point, hover gap
%    7  Thrust : momentum theory, thrust vectoring, yaw authority,
%       lift-fan reaction torque and required trim angle
%    8  Linear dynamics : 2.35 m straight with aero drag, braking
%    9  Yaw dynamics : bang-bang 90 deg and 180 deg turns
%   10  Servo torque check
%   11  Power budget, current, run time
%   12  Course-time estimate and competition score for sensor options
%   13  Size trade study (why 360 x 240 mm)
%   14  Figures
%
%  Coordinate system (all layout numbers in mm):
%    origin = geometric centre of the deck, +x = forward (nose),
%    +y = left side,  z = up.  Yaw is about z.
%
%  Works in MATLAB R2016b+ and GNU Octave 6+ (no toolboxes needed).
%  Every number marked (EST) is an estimate - replace it with a value you
%  measure (kitchen scale, ruler, tachometer) as soon as you can.
% =====================================================================
clear; close all; clc;

%% 0. USER OPTIONS ------------------------------------------------------
thrustFanChoice = 'MEC';   % 'MEC' = Sunon MEC0251V1 (recommended, light)
                           % 'CFM' = CUI CFM-A238V-131-480 (more thrust, heavier)
V_lift   = 13.8;           % [V] effective (PWM-averaged) lift-fan voltage
V_thrust = 12.0;           % [V] effective thrust-fan voltage
bonusMode = 'multiply';    % 'multiply' -> x1.15*1.25 ; 'add' -> x1.40
nLanes    = 3;             % straight segments (rules text says 3, but
                           % mentions a 4th - confirm with the instructor)

%% 1. CONSTANTS + FAN DATABASE -----------------------------------------
g    = 9.81;               % [m/s^2]
rho  = 1.204;              % [kg/m^3] air at 20 C
CFM2SI = 4.719474e-4;      % 1 CFM = 4.719e-4 m^3/s
inH2O  = 249.09;           % 1 inch H2O = 249.09 Pa
okTxt  = {'FAIL','OK'};
tf     = @(c) okTxt{double(logical(c))+1};   % prints OK / FAIL in checks

% Datasheet values at 12 V (DigiKey / manufacturer datasheets)
fan.AFB = struct('name','Delta AFB1212SH',  'Q',113.1,'p',107.1,'P',6.36,'I',0.530,'m',198,'rpm',3400,'depth',25.4);
fan.MEC = struct('name','Sunon MEC0251V1',  'Q',108.2,'p',0.28*inH2O,'P',5.5,'I',0.455,'m',162,'rpm',3100,'depth',25.0);
fan.CFM = struct('name','CUI CFM-A238V-131','Q',138.0,'p',0.36*inH2O,'P',11.5,'I',0.958,'m',218,'rpm',3100,'depth',38.0);

liftFan   = fan.AFB;               % highest static pressure -> lift
thrustFan = fan.(thrustFanChoice); % highest flow per gram   -> thrust

% Fan affinity laws (speed ~ voltage): Q ~ V, p ~ V^2, P ~ V^3
scaleFan = @(f,V) struct('Q',f.Q*(V/12),'p',f.p*(V/12)^2,'P',f.P*(V/12)^3, ...
                         'rpm',f.rpm*(V/12),'I',f.I*(V/12)^2);
liftOp   = scaleFan(liftFan,  V_lift);
thrustOp = scaleFan(thrustFan,V_thrust);

%% 2. GEOMETRY ---------------------------------------------------------
L_deck   = 360;  W_deck = 240;     % [mm] foam deck (aspect 1.5 : 1)
t_deck   = 20;                     % [mm] foam thickness (EST - your sheet)
rho_foam = 33;                     % [kg/m^3] XPS/EPS board (EST - weigh it)
skirtOut = 10;                     % [mm] bag-skirt bulge beyond deck edge
L_all = L_deck + 2*skirtOut;  W_all = W_deck + 2*skirtOut;   % overall
laneW = [500 550];                 % [mm] track width range
laneL = 2350;                      % [mm] straight segment length
wallH = 150;                       % [mm] wall height

% Thrust-fan pivot (servo axis) - far enough forward that the fan's
% swept circle stays inside the deck outline
x_pivot  = -115;  y_pivot = 0;     % [mm]
x_lift   =   10;  y_lift  = 0;     % [mm] lift-fan centre (fan in a deck hole)

%% 3. COMPONENT LIST ---------------------------------------------------
% {name, mass[g], x[mm], y[mm], size_x[mm], size_y[mm], shape, counts toward Nc?}
% shape: 'rect' / 'circ' drawn & given own inertia, 'plate' deck,
%        'hole' (negative mass), 'frame' skirt, 'pt' point mass (not drawn)
m_deckFull = rho_foam*(L_deck*W_deck*t_deck)*1e-9*1e3;       % [g]
m_hole     = rho_foam*(pi*60^2*t_deck)*1e-9*1e3;              % [g] 120 mm hole
C = {
 'Lift fan AFB1212SH',            liftFan.m,   x_lift,  y_lift, 120, 120, 'circ', 1
 'Lift-fan safety net',           8,           x_lift,  y_lift,   0,   0, 'pt',   0
 ['Thrust fan ' thrustFan.name],  thrustFan.m, x_pivot, y_pivot, thrustFan.depth, 120, 'rect', 1
 'Thrust-fan safety net',         8,           x_pivot, y_pivot,  0,   0, 'pt',   0
 'Servo HS-311 (under fan)',      43,          x_pivot, y_pivot, 20,  40, 'rect', 1
 'Fan-servo bracket',             12,          x_pivot, y_pivot,  0,   0, 'pt',   0
 'Controller + enclosure (EST)',  150,         115,      0,      77, 107, 'rect', 0
 'Battery A Gens ace 450 2S',     28,          115,     85,      55,  31, 'rect', 0
 'Battery B Gens ace 450 2S',     28,          115,    -85,      55,  31, 'rect', 0
 'US front HC-SR04',              8.5,         168,      0,      20,  45, 'rect', 1
 'IR side GP2Y0A21 (exit/wall)',  5,           115,      0,      13,  30, 'rect', 1
 'IR top GP2Y0A21 (bar)',         5,           45,      90,      13,  30, 'rect', 1
 'IMU GY-521 (free)',             5,           45,     -90,      21,  16, 'rect', 0
 'Foam deck',                     m_deckFull,  0,        0,  L_deck, W_deck, 'plate', 0
 'Lift-fan hole',                -m_hole,      x_lift,  y_lift, 120, 120, 'hole', 0
 'Bag skirt (PE film)',           20,          0,        0,   L_all,  W_all, 'frame', 0
 'Wiring, glue, tape, misc (EST)',30,          0,        0,       0,   0, 'pt',   0
};
nC   = size(C,1);
name = C(:,1);  m_g = cell2mat(C(:,2));
x_mm = cell2mat(C(:,3));  y_mm = cell2mat(C(:,4));
sx   = cell2mat(C(:,5));  sy   = cell2mat(C(:,6));
shp  = C(:,7);            isNc = cell2mat(C(:,8));

%% 4. MASS PROPERTIES --------------------------------------------------
m  = m_g/1000;  x = x_mm/1000;  y = y_mm/1000;  a = sx/1000;  b = sy/1000;
M  = sum(m);
xC = sum(m.*x)/M;   yC = sum(m.*y)/M;           % centre of mass [m]

Iself = zeros(nC,1);                             % each part about its own centroid
for k = 1:nC
  switch shp{k}
    case {'rect','plate'}, Iself(k) = m(k)*(a(k)^2 + b(k)^2)/12;
    case {'circ','hole'},  Iself(k) = m(k)*(a(k)/2)^2/2;
    case 'frame'           % thin rectangular ring of sides a x b
      Iself(k) = m(k)/(a(k)+b(k))*((a(k)^3+b(k)^3)/12 + a(k)*b(k)*(a(k)+b(k))/4);
    otherwise,             Iself(k) = 0;
  end
end
Izz = sum(Iself + m.*((x-xC).^2 + (y-yC).^2));   % parallel-axis theorem

% Lift acts at the centroid of the cushion area (= deck centre, 0,0),
% NOT at the lift-fan location: the plenum spreads the pressure.
e_CP = hypot(xC-0, yC-0);                        % COM <-> centre of pressure

fprintf('================ DESIGN R : MASS PROPERTIES ================\n');
fprintf('%-34s %8s %8s %8s\n','Component','m [g]','x [mm]','y [mm]');
for k = 1:nC, fprintf('%-34s %8.1f %8.1f %8.1f\n',name{k},m_g(k),x_mm(k),y_mm(k)); end
fprintf('Total mass               M   = %.3f kg\n',M);
fprintf('Centre of mass           COM = (%.1f, %.1f) mm\n',xC*1e3,yC*1e3);
fprintf('COM to centre of pressure e  = %.1f mm  (target < 10 mm)\n',e_CP*1e3);
fprintf('Yaw inertia about COM    Izz = %.5f kg m^2\n',Izz);
fprintf('Counted components       Nc  = %d (IMU not counted)\n',sum(isNc));

%% 5. GEOMETRIC CHECKS -------------------------------------------------
diagAll   = hypot(L_all,W_all);                  % largest horizontal dim.
r_sweep   = hypot(thrustFan.depth/2,60);          % fan corner radius about pivot
rearClear = (x_pivot - r_sweep) + L_deck/2;       % >0 -> fan stays on the deck
liftGap   = (x_lift-60) - (x_pivot + r_sweep);    % plan-view gap fan sweep / lift inlet
spinClear = (laneW(1) - diagAll)/2;               % room per side to spin in a 50 cm lane
fprintf('\n================ GEOMETRY CHECKS ================\n');
fprintf('Overall %d x %d mm, diagonal %.1f mm  (limit 480) -> %s\n',L_all,W_all,diagAll,tf(diagAll<=480));
fprintf('Thrust fan swept radius %.1f mm, rear clearance %.1f mm -> %s\n',r_sweep,rearClear,tf(rearClear>=0));
fprintf('Gap between fan sweep and lift-fan inlet: %.1f mm\n',liftGap);
fprintf('Clearance per side when spinning in a 50 cm lane: %.1f mm\n',spinClear);
fprintf('Side IR reading range: %.0f mm (touching) .. %.0f mm  (sensor valid 100..800)\n', ...
        W_all/2, laneW(2)-W_all/2);

%% 6. LIFT -------------------------------------------------------------
A_c   = (L_deck*W_deck)*1e-6;          % [m^2] cushion area (conservative: deck only)
Per   = 2*(L_all+W_all)*1e-3;          % [m] skirt perimeter (air escapes here)
Cd    = 0.6;                           % discharge coefficient of the skirt gap
p_c   = M*g/A_c;                       % [Pa] required cushion pressure
% Fan curve, linear approximation p = pmax(1 - Q/Qmax)  (conservative)
Qop   = @(op,p) max(op.Q*CFM2SI*(1 - p/op.p),0);   % [m^3/s] flow at pressure p
v_esc = sqrt(2*p_c/rho);                            % escape velocity under skirt
h_gap = @(op) Qop(op,p_c)/(Cd*Per*v_esc);           % [m] hover gap
lift12 = scaleFan(liftFan,12);
fprintf('\n================ LIFT ================\n');
fprintf('Cushion area %.4f m^2, required cushion pressure p_c = %.1f Pa\n',A_c,p_c);
fprintf('Lift fan @12.0 V: p_max = %.1f Pa (p_c/p_max = %.0f%%), Q = %.1f CFM, gap = %.2f mm\n', ...
        lift12.p,100*p_c/lift12.p,Qop(lift12,p_c)/CFM2SI,1e3*h_gap(lift12));
fprintf('Lift fan @%.1f V: p_max = %.1f Pa (p_c/p_max = %.0f%%), Q = %.1f CFM, gap = %.2f mm\n', ...
        V_lift,liftOp.p,100*p_c/liftOp.p,Qop(liftOp,p_c)/CFM2SI,1e3*h_gap(liftOp));
M_max = 0.75*liftOp.p*A_c/g;   % keep p_c below ~75% of p_max
fprintf('Mass budget to keep p_c < 75%% of p_max at %.1f V: %.3f kg\n',V_lift,M_max);

%% 7. THRUST + VECTORING ----------------------------------------------
d_tip = 0.116;  d_hub = 0.045;        % [m] blade tip / hub diameters (EST, measure)
A_ann = pi/4*(d_tip^2 - d_hub^2);     % [m^2] annulus that actually moves air
k_th  = 0.85;                         % jet non-uniformity / installation losses
Q_t   = thrustOp.Q*CFM2SI;            % [m^3/s] free-air flow
v_jet = Q_t/A_ann;                    % [m/s]
F     = k_th*rho*Q_t*v_jet;           % [N]  momentum theory  F = mdot * v
r_arm = hypot(x_pivot/1000 - xC, y_pivot/1000 - yC);   % [m] pivot -> COM
delta = linspace(-90,90,181);         % [deg] servo deflection (0 = straight back)
Fx    = F*cosd(delta);                % forward thrust
tauZ  = F*sind(delta)*r_arm;          % yaw torque
alphaZ= tauZ/Izz;                     % yaw acceleration
% Lift-fan reaction torque (motor torque on the air reacts on the hull)
eta_m   = 0.5;                                      % motor efficiency (EST)
omega_l = liftOp.rpm*2*pi/60;
tau_lf  = eta_m*liftOp.P/omega_l;                   % [N m] upper bound
delta_trim = asind(min(tau_lf/(F*r_arm),1));
fprintf('\n================ THRUST ================\n');
fprintf('Thrust fan: %s @ %.1f V, Q = %.1f CFM, jet %.2f m/s\n',thrustFan.name,V_thrust,thrustOp.Q,v_jet);
fprintf('Static thrust F = %.3f N (= %.1f gf)  -> MEASURE on a scale\n',F,F/g*1000);
fprintf('Thrust-to-weight = %.3f\n',F/(M*g));
fprintf('Lever arm pivot->COM r = %.1f mm, max yaw torque = %.4f N m\n',r_arm*1e3,F*r_arm);
fprintf('Max yaw acceleration (delta = 90 deg) = %.2f rad/s^2\n',F*r_arm/Izz);
fprintf('Lift-fan reaction torque (upper bound) = %.4f N m -> trim angle %.1f deg\n',tau_lf,delta_trim);

%% 8. LINEAR DYNAMICS --------------------------------------------------
CdA   = 1.05*(W_all*1e-3*0.10);        % frontal area ~ width x 10 cm, box Cd ~1.05
mu_k  = 0.35;                          % skirt-on-floor friction when lift is cut (EST)
dt = 1e-3;  t = 0;  v = 0;  s = 0;  T = 0;  V = 0;  S = 0;
while s < laneL/1000 && t < 30
  aNow = (F - 0.5*rho*CdA*v^2)/M;      % frictionless cushion + aero drag
  v = v + aNow*dt;  s = s + v*dt;  t = t + dt;
  T(end+1) = t;  V(end+1) = v;  S(end+1) = s; %#ok<AGROW>
end
t_235_drag = t;   v_235 = v;
a0 = F/M;  t_235_ideal = sqrt(2*laneL/1000/a0);
t_decay = 0.3;                         % s for the cushion to collapse after lift off
d_brake = @(v) v*t_decay + v.^2/(2*mu_k*g);
fprintf('\n================ LINEAR DYNAMICS ================\n');
fprintf('a0 = F/M = %.3f m/s^2\n',a0);
fprintf('2.35 m from rest: %.2f s ideal, %.2f s with drag, exit speed %.2f m/s\n',t_235_ideal,t_235_drag,v_235);
fprintf('Braking by cutting lift from %.2f m/s: %.2f m   | from 0.5 m/s: %.3f m\n',v_235,d_brake(v_235),d_brake(0.5));

%% 9. YAW DYNAMICS (bang-bang: accelerate half, counter-steer half) ----
alphaMax = F*r_arm/Izz;
t_turn = @(th) 2*sqrt(th/alphaMax);          % th in rad, starts & ends at rest
t90 = t_turn(pi/2);  t180 = t_turn(pi);
tt  = linspace(0,t180,300);
thB = zeros(size(tt));  wB = zeros(size(tt));
for k = 1:numel(tt)
  if tt(k) <= t180/2
    wB(k) = alphaMax*tt(k);           thB(k) = 0.5*alphaMax*tt(k)^2;
  else
    tr = tt(k)-t180/2;  w1 = alphaMax*t180/2;
    wB(k) = w1 - alphaMax*tr;         thB(k) = pi/2 + w1*tr - 0.5*alphaMax*tr^2;
  end
end
fprintf('\n================ YAW DYNAMICS ================\n');
fprintf('90 deg turn (rest to rest): %.2f s   | 180 deg: %.2f s\n',t90,t180);
fprintf('Peak yaw rate in a 180 deg turn: %.1f rad/s  (IMU gyro +-250 deg/s = %.1f rad/s)\n', ...
        max(wB),250*pi/180);

%% 10. SERVO CHECK -----------------------------------------------------
m_rot  = (thrustFan.m + 8 + 12)/1000;               % fan + net + bracket on the horn
I_fan  = m_rot*((0.120)^2 + (thrustFan.depth/1000)^2)/12;  % about vertical axis
w_srv  = (60*pi/180)/0.19;                           % HS-311: 0.19 s/60 deg @4.8 V
a_srv  = w_srv/0.05;                                 % reach full speed in 50 ms
tau_srv_need = I_fan*a_srv;
I_rotor = 0.5*0.040*0.05^2;                          % rotor ~40 g, r ~ 50 mm (EST)
tau_gyro = I_rotor*(thrustOp.rpm*2*pi/60)*w_srv;     % on bearings / servo spline
tau_srv_stall = 3.0*0.0981;                          % 3.0 kg cm @ 4.8 V
fprintf('\n================ SERVO HS-311 ================\n');
fprintf('Yaw inertia of fan assembly about servo axis: %.2e kg m^2\n',I_fan);
fprintf('Torque needed: %.3f N m vs stall %.3f N m  -> margin x%.0f\n', ...
        tau_srv_need,tau_srv_stall,tau_srv_stall/tau_srv_need);
fprintf('Gyroscopic side moment while slewing: %.3f N m (bearing load, not servo torque)\n',tau_gyro);

%% 11. POWER BUDGET -------------------------------------------------------
V_batt = 14.8;  C_Ah = 0.450;        % two 2S 450 mAh packs IN SERIES (4S equivalent)
P_lift = liftOp.P;  P_thr = thrustOp.P;
I5 = 0.25 + 0.040 + 0.015 + 2*0.030 + 0.004;   % servo avg + Nano + US + 2 IR + IMU [A]
P_5V = I5*V_batt;                    % worst case: linear 5 V regulator
P_tot = P_lift + P_thr + P_5V;
I_tot = P_tot/V_batt;
E_use = 0.8*V_batt*C_Ah;             % use 80 % of capacity (never over-discharge)
fprintf('\n================ POWER ================\n');
fprintf('Lift %.2f W + thrust %.2f W + 5 V rail %.2f W = %.2f W\n',P_lift,P_thr,P_5V,P_tot);
fprintf('Battery current %.2f A = %.1f C (pack limit 30 C = %.1f A)\n',I_tot,I_tot/C_Ah,30*C_Ah);
fprintf('Run time at 80%% DoD: %.1f min\n',60*E_use/P_tot);
fprintf('Lift-fan PWM duty for %.1f V: %.0f%% (full 16.8 V) .. %.0f%% (nominal 14.8 V)\n', ...
        V_lift,100*V_lift/16.8,100*min(V_lift/14.8,1));

%% 12. COURSE TIME + SCORE -------------------------------------------
v_c     = 0.8;               % [m/s] cruise speed the controller holds (tuning knob)
k_ctrl  = 1.3;               % real-world overhead (corrections, settling) EST
t_spin  = 0.4;               % lift-fan spin-up after a lift-cut stop [s]
t_dwell = 2.0;               % time held under the bar [s]
D_lane  = (laneL - mean(laneW))/1000;       % straight run before the turn zone
t1 = v_c/a0;  d1 = v_c^2/(2*a0);  d3 = d_brake(v_c);
t_lane = t1 + max(D_lane-d1-d3,0)/v_c + t_decay + v_c/(mu_k*g);
t_shift = 2*sqrt((mean(laneW)/1000)/a0);    % bang-bang lateral move one lane
t_U   = t_spin + 2*t90 + t_shift;
t_bar = t_dwell + t_spin;
t_exit = t90 + 2*sqrt(0.40/a0);
t_bump = 0.6;                                % per wall contact if no front sensor
t_base = k_ctrl*(nLanes*t_lane + (nLanes-1)*t_U);
d_full = nLanes*laneL/1000;
if strcmp(bonusMode,'multiply'), bBoth = 1.15*1.25; else, bBoth = 1.40; end
cfg = { % label, Nc, bonus, extra time, does it take the exit?
 'A: US front only',                4, 1.00, 0,                    0
 'B: US + IR top (bar)',            5, 1.15, t_bar,                0
 'C: US + IR side (exit)',          5, 1.25, t_exit,               1
 'D: US + IR top + IR side (build)',6, bBoth, t_bar+t_exit,        1
 'E: IMU bump-turn + 2 IR (lean)',  5, bBoth, t_bar+t_exit+nLanes*t_bump, 1
};
nCfg = size(cfg,1);  score = zeros(nCfg,2);  tC = zeros(nCfg,1);
fprintf('\n================ COURSE TIME + SCORE ================\n');
fprintf('Per lane %.2f s, per U-turn %.2f s, base course %.1f s (overhead x%.1f)\n',t_lane,t_U,t_base,k_ctrl);
fprintf('%-34s %3s %6s %7s %10s %10s\n','Configuration','Nc','bonus','t [s]','score S1','score S2');
for k = 1:nCfg
  tC(k) = t_base + cfg{k,4};
  score(k,1) = d_full/(cfg{k,2}*tC(k))*cfg{k,3};              % S1: finish without exit OK
  if cfg{k,5}, tS2 = tC(k); else, tS2 = 120; end               % S2: exit = finish line
  score(k,2) = d_full/(cfg{k,2}*tS2)*cfg{k,3};
  fprintf('%-34s %3d %6.3f %7.1f %10.4f %10.4f\n',cfg{k,1},cfg{k,2},cfg{k,3},tC(k),score(k,1),score(k,2));
end
fprintf('S1 = finishing at the end wall counts as complete; S2 = only exiting counts (else t = 120 s)\n');

%% 13. SIZE TRADE STUDY ------------------------------------------------
Ls  = 260:5:420;  Ws = Ls/1.5;
m_fixed = M - (m_deckFull - m_hole + 20)/1000;           % everything but foam + skirt
gapS = zeros(size(Ls));  diagS = gapS;  pS = gapS;
for k = 1:numel(Ls)
  mF = rho_foam*Ls(k)*Ws(k)*t_deck*1e-6 - m_hole;          % [g]
  mS = 20*(Ls(k)+Ws(k))/(L_deck+W_deck);                    % skirt scales with perimeter
  Mk = m_fixed + (mF+mS)/1000;
  Ak = Ls(k)*Ws(k)*1e-6;  pk = Mk*g/Ak;  pS(k) = pk/liftOp.p;
  Pk = 2*(Ls(k)+Ws(k)+4*skirtOut)*1e-3;
  gapS(k)  = 1e3*Qop(liftOp,pk)/(Cd*Pk*sqrt(2*pk/rho));
  diagS(k) = hypot(Ls(k)+2*skirtOut,Ws(k)+2*skirtOut);
end

%% 14. FIGURES ---------------------------------------------------------
% --- Fig 1 : top-view layout ---
figure('Name','Fig 1 - Top view layout','Color','w'); hold on; axis equal;
rectangle('Position',[-L_all/2 -W_all/2 L_all W_all],'Curvature',[0.08 0.12], ...
          'LineStyle','--','EdgeColor',[0.5 0.5 0.5]);
rectangle('Position',[-L_deck/2 -W_deck/2 L_deck W_deck],'EdgeColor','k','LineWidth',1.5);
th = linspace(0,2*pi,100);
plot(x_pivot + r_sweep*cos(th), y_pivot + r_sweep*sin(th),':','Color',[0.8 0.3 0.1]);
cols = lines(nC);
for k = 1:nC
  switch shp{k}
    case 'rect'
      rectangle('Position',[x_mm(k)-sx(k)/2, y_mm(k)-sy(k)/2, sx(k), sy(k)], ...
                'FaceColor',cols(k,:)*0.35+0.65,'EdgeColor',cols(k,:));
      text(x_mm(k),y_mm(k),sprintf('%d',k),'HorizontalAlignment','center','FontSize',8);
    case 'circ'
      rectangle('Position',[x_mm(k)-sx(k)/2, y_mm(k)-sy(k)/2, sx(k), sy(k)], ...
                'Curvature',[1 1],'FaceColor',[0.85 0.9 1],'EdgeColor','b');
      text(x_mm(k),y_mm(k),sprintf('%d',k),'HorizontalAlignment','center','FontSize',8);
  end
end
plot(xC*1e3,yC*1e3,'r+','MarkerSize',14,'LineWidth',2);
plot(0,0,'ko','MarkerSize',8);
plot([x_pivot-70 x_pivot-130],[0 0],'-','Color',[0.8 0.3 0.1],'LineWidth',2);  % jet
text(x_pivot-135,12,'jet','Color',[0.8 0.3 0.1],'HorizontalAlignment','right');
plot([190 240],[0 0],'k-','LineWidth',1.5); plot(240,0,'k>','MarkerFaceColor','k');
text(245,0,'+x fwd');
lbl = {};
for k = 1:nC, if any(strcmp(shp{k},{'rect','circ'})), lbl{end+1} = sprintf('%d  %s',k,name{k}); end, end %#ok<AGROW>
h1 = ceil(numel(lbl)/2);
text(-220,-140,strjoin(lbl(1:h1),char(10)),'FontSize',7,'VerticalAlignment','top');
text(  40,-140,strjoin(lbl(h1+1:end),char(10)),'FontSize',7,'VerticalAlignment','top');
title(sprintf('Design R top view - overall %dx%d mm, diagonal %.0f mm  (+ = COM, o = centre of lift)', ...
      L_all,W_all,diagAll),'FontSize',9);
xlabel('x [mm]'); ylabel('y [mm]'); xlim([-230 300]); ylim([-230 160]); grid on;

% --- Fig 2 : lift fan operating point ---
figure('Name','Fig 2 - Lift','Color','w');
subplot(1,2,1); hold on; grid on;
Qv = linspace(0,liftOp.Q,100);
plot(Qv, max(lift12.p*(1-Qv/lift12.Q),0),'b--');
plot(Qv, liftOp.p*(1-Qv/liftOp.Q),'b-','LineWidth',1.5);
plot([0 liftOp.Q],[p_c p_c],'r-','LineWidth',1.2);
plot(Qop(liftOp,p_c)/CFM2SI,p_c,'ko','MarkerFaceColor','k');
legend('AFB1212SH @12 V',sprintf('AFB1212SH @%.1f V',V_lift),'Required cushion pressure','Operating point');
xlabel('Flow [CFM]'); ylabel('Static pressure [Pa]'); title('Lift fan vs cushion requirement');
subplot(1,2,2); hold on; grid on;
Mv = linspace(0.5,1.0,100);
gv12 = zeros(size(Mv)); gvO = gv12;
for k = 1:numel(Mv)
  pk = Mv(k)*g/A_c; ve = sqrt(2*pk/rho);
  gv12(k) = 1e3*Qop(lift12,pk)/(Cd*Per*ve);  gvO(k) = 1e3*Qop(liftOp,pk)/(Cd*Per*ve);
end
plot(Mv,gv12,'b--',Mv,gvO,'b-','LineWidth',1.2);
plot([M M],[0 5],'k:'); plot([0.5 1],[3 3],'r:');
legend('@12 V',sprintf('@%.1f V',V_lift),'this design','3 mm obstacle');
xlabel('Total mass [kg]'); ylabel('Hover gap [mm]'); title('Hover gap vs mass'); ylim([0 5]); xlim([0.5 1]);

% --- Fig 3 : thrust vectoring ---
figure('Name','Fig 3 - Thrust vectoring','Color','w');
subplot(1,2,1); plot(delta,alphaZ,'LineWidth',1.3); grid on;
xlabel('Servo deflection \delta [deg]'); ylabel('\alpha_z [rad/s^2]'); title('Yaw acceleration vs fan angle');
subplot(1,2,2); plot(delta,Fx/M,'LineWidth',1.3); grid on;
xlabel('Servo deflection \delta [deg]'); ylabel('a_x [m/s^2]'); title('Forward acceleration vs fan angle');

% --- Fig 4 : time responses ---
figure('Name','Fig 4 - Time responses','Color','w');
subplot(2,2,1); plot(T,S,'LineWidth',1.3); grid on; xlabel('t [s]'); ylabel('x [m]'); title('Straight 2.35 m (with drag)');
subplot(2,2,2); plot(T,V,'LineWidth',1.3); grid on; xlabel('t [s]'); ylabel('v [m/s]'); title('Forward speed');
subplot(2,2,3); plot(tt,thB*180/pi,'LineWidth',1.3); grid on; xlabel('t [s]'); ylabel('\theta [deg]'); title('180 deg bang-bang turn');
subplot(2,2,4); plot(tt,wB,'LineWidth',1.3); grid on; xlabel('t [s]'); ylabel('\omega [rad/s]'); title('Yaw rate');

% --- Fig 5 : size trade ---
figure('Name','Fig 5 - Size trade','Color','w');
subplot(1,2,1); plot(Ls,gapS,'LineWidth',1.3); hold on; grid on;
plot([L_deck L_deck],[0 max(gapS)],'k:'); xlabel('Deck length L [mm] (W = L/1.5)'); ylabel('Hover gap [mm]');
title(sprintf('Hover gap @%.1f V',V_lift));
subplot(1,2,2); plot(Ls,diagS,'LineWidth',1.3); hold on; grid on;
plot(Ls,480*ones(size(Ls)),'r--',Ls,500*ones(size(Ls)),'m--',[L_deck L_deck],[350 520],'k:');
legend('Overall diagonal','48 cm rule','50 cm lane','this design','Location','northwest');
xlabel('Deck length L [mm]'); ylabel('[mm]'); title('Diagonal vs limits');

% --- Fig 6 : score comparison ---
figure('Name','Fig 6 - Score','Color','w');
bar(score); grid on;
set(gca,'XTickLabel',{'A','B','C','D','E'});
legend('S1: end wall = finish','S2: exit = finish','Location','northwest');
ylabel('Score d/(N_c t) x bonus'); title('Sensor configuration vs competition score');
