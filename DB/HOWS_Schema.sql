#
#    HOWS Schema 17/02/2021
# 
#    $$$$$$$$$$$$$$$$$$$$$$$
#    $   HOWS_Schema.sql   $
#    $$$$$$$$$$$$$$$$$$$$$$$
#   
#    by W.B. Yates
#    Copyright (c) University of Exeter. All rights reserved.
#    History:
# 
#    A database schema for a HydNetwork - tested with MySQL 8.0.23
#    node and link IDs use  VARCHAR(64) because in practice these IDs can be very long
# 


CREATE TABLE IF NOT EXISTS network
(
   user VARCHAR(64) NOT NULL,
   name VARCHAR(64) NOT NULL,
   
   nodes        INT NOT NULL,
   links        INT NOT NULL,

   PRIMARY KEY (user, name)
)
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS network_value
(
   user           VARCHAR(64) NOT NULL,
   name VARCHAR(64) NOT NULL,
   
   time_index   INT NOT NULL,
   cost         FLOAT NOT NULL,
   head_deficit FLOAT NOT NULL,
   resilience   FLOAT NOT NULL,

   PRIMARY KEY (user, name, time_index)
)
ENGINE InnoDB;




CREATE TABLE IF NOT EXISTS pipe
(
   user         VARCHAR(64) NOT NULL,
   network      VARCHAR(64) NOT NULL,
   id           VARCHAR(64) NOT NULL,
   
   dv_group     VARCHAR(64) NOT NULL,
   from_node    VARCHAR(64) NOT NULL,
   to_node      VARCHAR(64) NOT NULL,
   diameter     FLOAT NOT NULL,
   length       FLOAT NOT NULL,
   roughness    FLOAT NOT NULL,
   initial_status VARCHAR(8) NOT NULL,

   PRIMARY KEY (user, network, id)
)
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS pipe_value
(
   user         VARCHAR(64) NOT NULL,
   network      VARCHAR(64) NOT NULL,
   id           VARCHAR(64) NOT NULL,
   time_index   INT NOT NULL,
   
   flow            FLOAT NOT NULL,
   velocity        FLOAT NOT NULL,
   friction_factor FLOAT NOT NULL,
   reaction_rate   FLOAT NOT NULL,
   quality         FLOAT NOT NULL,
   status          VARCHAR(8) NOT NULL,

   PRIMARY KEY (user, network, id, time_index)
)
ENGINE InnoDB;




CREATE TABLE IF NOT EXISTS valve
(
   user           VARCHAR(64) NOT NULL,
   network        VARCHAR(64) NOT NULL,
   id             VARCHAR(64) NOT NULL,
   
   dv_group       VARCHAR(64) NOT NULL,
   from_node      VARCHAR(64) NOT NULL,
   to_node        VARCHAR(64) NOT NULL,

   diameter       FLOAT NOT NULL,
   valve_type     VARCHAR(3) NOT NULL,
   head_loss      FLOAT NOT NULL,
   setting        FLOAT NOT NULL,
   fixed_status   VARCHAR(8) NOT NULL,
   
   PRIMARY KEY (user, network, id)
)
ENGINE InnoDB;



CREATE TABLE IF NOT EXISTS valve_value
(
   user         VARCHAR(64) NOT NULL,
   network      VARCHAR(64) NOT NULL,
   id           VARCHAR(64) NOT NULL,
   time_index   INT NOT NULL,
   
   flow         FLOAT NOT NULL,
   velocity     FLOAT NOT NULL,
   quality      FLOAT NOT NULL,
   status       VARCHAR(8) NOT NULL,
   
   PRIMARY KEY (user, network, id, time_index)
)
ENGINE InnoDB;

 

CREATE TABLE IF NOT EXISTS pump
(
   user           VARCHAR(64) NOT NULL,
   network        VARCHAR(64) NOT NULL,
   id             VARCHAR(64) NOT NULL,
   
   dv_group       VARCHAR(64) NOT NULL,
   from_node      VARCHAR(64) NOT NULL,
   to_node        VARCHAR(64) NOT NULL,
   
   power          FLOAT NOT NULL,
   speed          FLOAT NOT NULL,
   energy_price    FLOAT NOT NULL,
   headloss       FLOAT NOT NULL,
   curve_id VARCHAR(64) NOT NULL,
   effic_id VARCHAR(64) NOT NULL,
   pattern_id VARCHAR(64) NOT NULL,
   price_pattern_id VARCHAR(64) NOT NULL,
   initial_status VARCHAR(8) NOT NULL,
    
   PRIMARY KEY (user, network, id)
)
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS pump_value
(
   user         VARCHAR(64) NOT NULL,
   network      VARCHAR(64) NOT NULL,
   id           VARCHAR(64) NOT NULL,
   time_index   INT NOT NULL,
   
   flow         FLOAT NOT NULL,
   quality      FLOAT NOT NULL,
   status       VARCHAR(8) NOT NULL,
   
   PRIMARY KEY (user, network, id, time_index)
)
ENGINE InnoDB;



CREATE TABLE IF NOT EXISTS junction
(
   user           VARCHAR(64) NOT NULL,
   network        VARCHAR(64) NOT NULL,
   id             VARCHAR(64) NOT NULL,
   
   dv_group       VARCHAR(64) NOT NULL,
   base_elevation FLOAT NOT NULL,
   base_demand    FLOAT NOT NULL,
   
   PRIMARY KEY (user, network, id)
)
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS junction_value
(
   user           VARCHAR(64) NOT NULL,
   network       VARCHAR(64) NOT NULL,
   id            VARCHAR(64) NOT NULL,
   time_index    INT NOT NULL,

   total_head    FLOAT NOT NULL,
   head_deficit  FLOAT NOT NULL,
   head_excess   FLOAT NOT NULL,
   min_head      FLOAT NOT NULL,
   pressure         FLOAT NOT NULL,
   actual_demand FLOAT NOT NULL,
   quality       FLOAT NOT NULL,

   PRIMARY KEY (user, network, id, time_index)
)
ENGINE InnoDB;





CREATE TABLE IF NOT EXISTS reservoir
(
   user         VARCHAR(64) NOT NULL,
   network      VARCHAR(64) NOT NULL,
   id           VARCHAR(64) NOT NULL,

   dv_group     VARCHAR(64) NOT NULL,
   total_head   FLOAT NOT NULL,
   elevation    FLOAT NOT NULL,

   PRIMARY KEY (user, network, id)
)
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS reservoir_value
(
   user          VARCHAR(64) NOT NULL,
   network       VARCHAR(64) NOT NULL,
   id            VARCHAR(64) NOT NULL,
   time_index    INT NOT NULL,

   net_inflow   FLOAT NOT NULL,
   pressure     FLOAT NOT NULL,
   quality      FLOAT NOT NULL,
   
   PRIMARY KEY (user, network, id, time_index)
)
ENGINE InnoDB;
