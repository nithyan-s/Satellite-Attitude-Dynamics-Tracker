# Satellite-Attitude-Dynamics-Tracker

#Author - Nithyan

Description: 
This project is a real-time application designed for filtering and analyzing satellite attitude data, specifically focusing on pitch, yaw, and roll telemetry. It reads raw telemetry data, extracts specific command events (enable-disable) and associated time intervals, and cross-references these with an external time log to calculate dump counts and other relevant statistics. The system is optimized for high-performance real-time operations and is built to provide precise monitoring and analysis of satellite orientation, ensuring the effective control and adjustment of satellite positioning.

Key Features:
- Real-time telemetry processing of pitch, yaw, and roll data.
- Efficient command filtering for enable-disable events.
- Cross-referencing with external log files for time-based data analysis.
- Calculation of satellite attitude dump counts for telemetry verification.
- Structured for high-performance, mission-critical environments.

Sample TCH[Topographic Correction High] file data :-
		GSAT-AB  TELECOMMAND HISTORY    PAGE-001  	
<br>SL.NO	CODE	    COMMAND MNEMONIC	    DATE	    TIME	    STATUS <br>
1	    C24000C7	DECODER TEST COMMAND-2	01-Aug-24	00:10:00	CONFIRMD <br>

Sample TAB file data :- <br>
Date	    Time	    Timer	Driver_status	Pitch_dump_status	yaw_dump_status	Pitch_comp	Yaw_comp <br>
01-Aug-24	06:00:00	299.99	    OFF	            DISABLE	            DISABLE	        300	        60 <br>
