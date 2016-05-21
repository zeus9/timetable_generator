# Timetable Generator
Generates a timetable for my university using a genetic algorithm.

Run<br> 
$ sudo ./ttgen<br>
in your terminal to execute the program. An initial gui would pop up where you can change the default values, if needed.
The rest of the input is given to the program as .csv files:<br>
- initial.csv<br>
- periodcount.csv<br>
- labPeriodcount.csv<br>
which is the university data. The default data (real data of [Amrita University](https://www.amrita.edu/)) is included with the program. 

Presently the program is a bunch of cpp and python files linked by a bash script(ttgen.sh).
Sophistication of the software using a particular framework, say qt to make it a proper desktop app is the plan forward once the genetic algorithm is completely optimized.
