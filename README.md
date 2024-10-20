# Spacecraft-Momentum-Management-Monitoring-Tool

#Author - Nithyan

Description: 
This project is a periodic utility application designed for analyzing satellite momentum data, specifically focusing on pitch and yaw axes. It analyzes the history of commanding activity and extracts specific command events (enable-disable), associated time intervals, calculates dump counts, and other relevant statistics. The software is built to provide precise monitoring and analysis of satellite environment disturbances, ensuring the effective control and adjustment of satellite positioning.

Key Features:
- Offline Telemetry processing of pitch, yaw momentum data.
- Efficient command filtering for specific/required events.
- Calculation of satellite momentum dump counts for bookkeeping of propellant consumption.
- Structured for high-performance, mission-critical environments.

Sample TCH[Topographic Correction High] file data :-<br>
		SAT-AB  TELECOMMAND HISTORY    PAGE-001  	
<br>SL.NO	CODE	    COMMAND MNEMONIC	    DATE	    TIME	    STATUS <br>
1	    X24000Z7	 TEST COMMAND-2        	01-Aug-24	00:10:00	CONFIRMD <br>
2	    X24000Z8	 TEST COMMAND-1        	01-Aug-24	00:15:00	CONFIRMD <br>

Sample TAB file data:- <br>
Date	    Time	    Timer	Driver_status	Pitch_dump_status	yaw_dump_status	Pitch_comp	Yaw_comp <br>
01-Aug-24	06:00:00	299.99	    OFF	            DISABLE	            DISABLE	        300	        60 <br>
