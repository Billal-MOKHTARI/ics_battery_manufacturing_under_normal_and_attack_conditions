DROP DATABASE IF EXISTS production;
CREATE DATABASE production;

USE production;

-- 1. FIXED PROCESS TABLE (no changes needed)
CREATE TABLE Process (
    name VARCHAR(255) PRIMARY KEY,
    description TEXT,
    created_at TIMESTAMP,
    updated_at TIMESTAMP
);

-- 2. FIXED MACHINE TABLE (no changes needed)
CREATE TABLE Machine (
    name VARCHAR(255) PRIMARY KEY,
    process VARCHAR(255),
    description TEXT,
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (process) REFERENCES Process(name) ON DELETE SET NULL
);

-- 3. FIXED INTERFACE TABLE (no changes needed)
CREATE TABLE Interface (
    machine VARCHAR(255),
    name VARCHAR(255),
    ipv4 VARCHAR(15),
    mask INT(2),
    addr_type VARCHAR(50) CHECK (addr_type IN ("Static", "DHCP")),
    MAC VARCHAR(17),
    PRIMARY KEY (machine, name),
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (machine) REFERENCES Machine(name) ON DELETE CASCADE
);

-- 4. FIXED PORT TABLE (MAJOR FIX HERE)
CREATE TABLE Port (
    machine VARCHAR(255),
    interface VARCHAR(255),  -- Changed from 'interface' to match Interface.ipv4
    port INT,
    service_name VARCHAR(255),
    PRIMARY KEY (machine, interface, port),
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (machine, interface) 
        REFERENCES Interface(machine, name)  -- Composite foreign key
        ON DELETE CASCADE
);

-- 5. FIXED SESSION TABLE (FIXED COLUMNS AND FOREIGN KEYS)
CREATE TABLE Session (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    machine VARCHAR(255),
    interface VARCHAR(255),
    port INT,

    src_ipv4 VARCHAR(15),
    src_mask INT(2),

    dest_ipv4 VARCHAR(15),
    dest_port INT,
    
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (machine, interface, port) 
        REFERENCES Port(machine, interface, port)
        ON DELETE CASCADE
);

-- 6. FIXED PLCVariable TABLE (no changes needed)
CREATE TABLE PLCVariable (
    id VARCHAR(255) PRIMARY KEY,
    var VARCHAR(255),
    documentation TEXT,
    var_type VARCHAR(50) CHECK (var_type IN ("Sensor", "Actuator", "State", "Transition")),
    physical_address VARCHAR(255),
    digital_address INT,
    pin_mode VARCHAR(50) CHECK (pin_mode IN ("Digital In", "Digital Out", "Analog In", "Analog Out")),
    pin_prog VARCHAR(50) CHECK (pin_prog IN ("Coil", "Holding Register")),
    machine_id VARCHAR(255),
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (machine_id) REFERENCES Machine(name) ON DELETE CASCADE
);

-- Indexes (corrected)
CREATE INDEX idx_plcvariable_id ON PLCVariable(id);
CREATE INDEX idx_session_id ON Session(id);

CREATE TABLE MixingPLCHistory (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    session BIGINT,
    m_id VARCHAR(255),
    a_id VARCHAR(255),
    b_id VARCHAR(255),
    d1_id VARCHAR(255),
    d2_id VARCHAR(255),
    e1_id VARCHAR(255),
    e2_id VARCHAR(255),
    f_id VARCHAR(255),
    g_id VARCHAR(255),
    l_id VARCHAR(255),
    EA_id VARCHAR(255),
    EB_id VARCHAR(255),
    ED_id VARCHAR(255),
    EE_id VARCHAR(255),
    EF_id VARCHAR(255),
    EG_id VARCHAR(255),
    M1_id VARCHAR(255),
    M2_id VARCHAR(255),
    X0_id VARCHAR(255),
    X1_id VARCHAR(255),
    X2_id VARCHAR(255),
    X3_id VARCHAR(255),
    X4_id VARCHAR(255),
    X5_id VARCHAR(255),
    X6_id VARCHAR(255),
    X7_id VARCHAR(255),
    X8_id VARCHAR(255),
    T0_id VARCHAR(255),
    T1_id VARCHAR(255),
    T2_id VARCHAR(255),
    T3_id VARCHAR(255),
    T4_id VARCHAR(255),
    T5_id VARCHAR(255),
    T6_id VARCHAR(255),
    T7_id VARCHAR(255),
    T8_id VARCHAR(255),
    PV_id VARCHAR(255),
    CV_id VARCHAR(255),
    CU_id VARCHAR(255),
    time_1_id VARCHAR(255),
    time_2_id VARCHAR(255),
    timer_gauge_1_id VARCHAR(255),
    timer_gauge_2_id VARCHAR(255),
    m BOOLEAN,
    a BOOLEAN,
    b BOOLEAN,
    d1 BOOLEAN,
    d2 BOOLEAN,
    e1 BOOLEAN,
    e2 BOOLEAN,
    f BOOLEAN,
    g BOOLEAN,
    l BOOLEAN,
    EA BOOLEAN,
    EB BOOLEAN,
    ED BOOLEAN,
    EE BOOLEAN,
    EF BOOLEAN,
    EG BOOLEAN,
    M1 BOOLEAN,
    M2 BOOLEAN,
    X0 BOOLEAN DEFAULT TRUE,
    X1 BOOLEAN,
    X2 BOOLEAN,
    X3 BOOLEAN,
    X4 BOOLEAN,
    X5 BOOLEAN,
    X6 BOOLEAN,
    X7 BOOLEAN,
    X8 BOOLEAN,
    T0 BOOLEAN,
    T1 BOOLEAN,
    T2 BOOLEAN,
    T3 BOOLEAN,
    T4 BOOLEAN,
    T5 BOOLEAN,
    T6 BOOLEAN,
    T7 BOOLEAN,
    T8 BOOLEAN,
    CU BOOLEAN DEFAULT TRUE,
    PV INT DEFAULT 3,
    CV INT,
    time_1 INT DEFAULT 0,
    time_2 INT DEFAULT 0,
    timer_gauge_1 INT DEFAULT 5,
    timer_gauge_2 INT DEFAULT 5,
    created_at TIMESTAMP,
    updated_at TIMESTAMP,
    FOREIGN KEY (session) REFERENCES Session(id) ON DELETE CASCADE,
    FOREIGN KEY (m_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (a_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (b_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (d1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (d2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (e1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (e2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (f_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (g_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (l_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (EA_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (EB_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (ED_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (EE_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (EF_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (EG_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (M1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (M2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X0_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X3_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X4_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X5_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X6_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X7_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (X8_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T0_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T3_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T4_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T5_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T6_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T7_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (T8_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (PV_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (CV_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (CU_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (time_1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (time_2_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (timer_gauge_1_id) REFERENCES PLCVariable(id),
    FOREIGN KEY (timer_gauge_2_id) REFERENCES PLCVariable(id)
);




-- Add tuples
INSERT INTO Process (name, description, created_at, updated_at) 
VALUES ("Mixing", "The anode and the cathode are mixed together using different materials.", NOW(), NOW());

INSERT INTO Machine (name, process, description, created_at, updated_at) 
VALUES (
    "Batt-TDS Mixing Machine", 
    "Mixing", 
    "Mixing machines for battery production, specifically for electrode slurries, are often high-shear, planetary, or vacuum mixers. These machines ensure the uniform dispersion of active materials and binders, while vacuum systems minimize air bubbles. Temperature control is also crucial to maintain the slurry\"s viscosity. These processes are essential for achieving the right consistency and performance of the slurry used in battery manufacturing.",
    NOW(), 
    NOW()
);

INSERT INTO Interface (machine, addr_type, ipv4, mask, name, created_at, updated_at, MAC) 
VALUES
    ("Batt-TDS Mixing Machine", "Static", "192.168.60.2", 24, "eth1", NOW(), NOW(), "02:42:2C:77:6A:01"),
    ("Batt-TDS Mixing Machine", "DHCP", "192.168.122.27", 24, "eth0", NOW(), NOW(), "02:42:2C:77:6A:00");

INSERT INTO Port (machine, interface, port, service_name, created_at, updated_at) 
VALUES
    ("Batt-TDS Mixing Machine", "eth0", 502, "Modbus TCP", NOW(), NOW()),
    ("Batt-TDS Mixing Machine", "eth1", 502, "Modbus TCP", NOW(), NOW());

INSERT INTO PLCVariable (id, var, documentation, var_type, physical_address, digital_address, pin_mode, pin_prog, machine_id, created_at, updated_at)
VALUES
    ("batttdsmixm", "m", "Starting button", "Sensor", "%QX0.0", 0, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixa", "a", "Position sensor placed on the valve A", "Sensor", "%QX0.1", 1, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixb", "b", "Position sensor placed on the valve B", "Sensor", "%QX0.2", 2, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixd1", "d1", "Weight sensor that controls the weight of the materials inside the valve D according to the fixed quantity", "Sensor", "%QX0.3", 3, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixd2", "d2", "Position sensor placed on the valve D", "Sensor", "%QX0.4", 4, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixe1", "e1", "Weight sensor that controls the weight of the materials inside the valve E according to the fixed quantity", "Sensor", "%QX0.5", 5, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixe2", "e2", "Position sensor placed on the valve E", "Sensor", "%QX0.6", 6, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixf", "f", "Position sensor placed in front of the motor M1", "Sensor", "%QX0.7", 7, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixg", "g", "Position sensor placed in front of the motor M2", "Sensor", "%QX1.0", 8, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixl", "l", "Sensor that measures the level of the liquid inside the pipe", "Sensor", "%QX1.1", 9, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixEA", "EA", "Cylinder controlling the opening and the closing of the valve A", "Actuator", "%QX2.0", 16, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixEB", "EB", "Cylinder controlling the opening and the closing of the valve B", "Actuator", "%QX2.1", 17, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixED", "ED", "Cylinder controlling the opening and the closing of the valve C", "Actuator", "%QX2.2", 18, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixEE", "EE", "Cylinder controlling the opening and the closing of the valve D", "Actuator", "%QX2.3", 19, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixEF", "EF", "Cylinder controlling the opening and the closing of the valve attached to the engine M1", "Actuator", "%QX2.4", 20, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixEG", "EG", "Cylinder controlling the opening and the closing of the vessel", "Actuator", "%QX2.5", 21, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixM1", "M1", "3D mixer engine", "Actuator", "%QX2.6", 22, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixM2", "M2", "Inline-mixer (high shear) engine", "Actuator", "%QX2.7", 23, "Digital Out", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),

    ("batttdsmixX0", "X0", "Step 0", "State", "%QX3.0", 24, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX1", "X1", "Step 1", "State", "%QX3.1", 25, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX2", "X2", "Step 2", "State", "%QX3.2", 26, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX3", "X3", "Step 3", "State", "%QX3.3", 27, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX4", "X4", "Step 4", "State", "%QX3.4", 28, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX5", "X5", "Step 5", "State", "%QX3.5", 29, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX6", "X6", "Step 6", "State", "%QX3.6", 30, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX7", "X7", "Step 7", "State", "%QX3.7", 31, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixX8", "X8", "Step 8", "State", "%QX4.0", 32, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),


    ("batttdsmixT0", "T0", "Transition 0", "Transition", "%QW5.0", 40, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT1", "T1", "Transition 1", "Transition", "%QW5.1", 41, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT2", "T2", "Transition 2", "Transition", "%QW5.2", 42, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT3", "T3", "Transition 3", "Transition", "%QW5.3", 43, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT4", "T4", "Transition 4", "Transition", "%QW5.4", 44, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT5", "T5", "Transition 5", "Transition", "%QW5.5", 45, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT6", "T6", "Transition 6", "Transition", "%QW5.6", 46, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT7", "T7", "Transition 7", "Transition", "%QW5.7", 47, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixT8", "T8", "Transition 8", "Transition", "%QW6.0", 48, NULL, "Coil", "Batt-TDS Mixing Machine", NOW(), NOW()),


    ("batttdsmixPV", "PV", "Preset Value", "Sensor", "%QW0", 0, "Analog In", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixCV", "CV", "Control Value", "Actuator", "%QW1", 1, "Analog Out", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixtime_1", "time_1", "Time counter 1", "Actuator", "%QW2", 2, "Analog Out", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixtime_2", "time_2", "Time counter 2", "Actuator", "%QW3", 3, "Analog Out", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixtimer_gauge_1", "timer_gauge_1", "Timer gauge 1", "Sensor", "%QW4", 4, "Analog In", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),
    ("batttdsmixtimer_gauge_2", "timer_gauge_2", "Timer gauge 2", "Sensor", "%QW5", 5, "Analog In", "Holding Register", "Batt-TDS Mixing Machine", NOW(), NOW()),

    ("batttdsmixCU", "CU", "Counting Unit", "Sensor", "%QX7.0", 56, "Digital In", "Coil", "Batt-TDS Mixing Machine", NOW(), NOW());
