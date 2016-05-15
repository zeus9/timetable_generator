#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>

#define MAX_PERIODS_PER_WEEK 1100
#define MAX_ROOMS 50
#define MAX_TEACHERS 150
#define POSITIVE_INFINITY 9999
#define EMPTY -1


using namespace std;

long long randomoffset;
int elapsedgenerations;

//nPeriodsPerWeek always multiple of 5
int nPeriodsPerWeek, nteachers, nRooms;
int nLabSubjects, labslots, nLabs, firstLabRoom, lastLabRoom;
int populationsize, generationlimit;
int tournamentsize, tempint;
double mutationrate;
int elitism, crossoversplit, labCrossverSplit;	//csefaculty;


vector <string> teachers, labTeachers;
map <string, int> teacherid, labTeacherid;


class individual 
{
	public:
		int table[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
		double fitness;
	
		individual () 
		{ fitness = 0; }
};

int initial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
int labInitial[MAX_ROOMS][MAX_PERIODS_PER_WEEK];
int availability[MAX_TEACHERS][MAX_PERIODS_PER_WEEK];
int periodcount[MAX_ROOMS][MAX_TEACHERS];
int labPeriodcount[MAX_ROOMS][MAX_TEACHERS];
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];
individual elite;
vector <individual> population;

string return_tID(string labSub)
{
	string tid;
	for(int m = 0; m<labSub.size(); m++)
	{			
		if(labSub[m] == '/')
		{
			tid = labSub.substr(0,m);
		}
	}
	return tid;
}


int return_roomNo(string labSub)
{
	int room;
	for(int m = 0; m < labSub.size(); m++)
	{
		if(labSub[m] == 'r')
		{
			room = stoi(labSub.substr(m+1));
		}
	}
	return room;
}


int randomint(int lower, int upper)
{
	srand(time(0)+randomoffset);
	randomoffset = (randomoffset+1)%2823401239LL;
	if(upper<lower) return lower;
	return rand()%(upper-lower+1)+lower;
}

bool randombool(double chance)
{	
	if(randomint(0,1000000) < (long long) 1000000*chance) return true; else return false;
}


int getminfitnessid()
{
	double minvalue = POSITIVE_INFINITY;
	int minid = 0, count = 0;
	string kteacher, lteacher, ktid, ltid;
	int n = 0;
	int kroom, lroom;
	double tempfitness = 0, first2Hours = 0, confAvail = 0, oneLabperday = 0;
	
	for(int i = 0; i < population.size(); i++)
	{
		//cout<<"HI"<<i<<"\n";
		tempfitness = 0;
		first2Hours = 0;
		confAvail = 0;
		oneLabperday = 0;

		for(int j = 0; j < labslots; j++)	//calculate conflicts with fixed slots in initial
		{
			for(int k = 0; k < nLabs; k++)
			{
				if(population[i].table[k][j] != EMPTY)
				{
					int room = return_roomNo(labTeachers[population[i].table[k][j]]);

					if(initial[room-1][2*j] != EMPTY || initial[room-1][2*j+1] != EMPTY)	// subjective to this this slot system with 6 hours a day and 2 hour labs
						confAvail++; 				//calculate conflicts with fixed slots in initial
				}
			}	
		}

		count = 0;	//very fucking important, took 3 days to find the bug caused without this piece of shit
		//calculate conflicts within lab classes
		for(int j = 0; j < labslots; j++)
		{
			if( j%(labslots/5) == 0)
				count += 1;

			for(int k = 0; k < nLabs; k++)
			{
				if(population[i].table[k][j] == EMPTY)
					continue;
				else
				{	
					kteacher = labTeachers[population[i].table[k][j]];
					int kroom = return_roomNo(kteacher);
					string ktid = return_tID(kteacher);

					for(int n = j+1; n < count*(labslots/5); n++)
					{							
						for(int l = 0; l < nLabs; l++)
						{
							if(population[i].table[l][n] == EMPTY)
								continue;
							else
							{						
								lteacher = labTeachers[population[i].table[l][n]];
								int lroom = return_roomNo(lteacher);
								string lid = return_tID(lteacher);	
								
								if(kroom == lroom)	//checking for one lab/day for a teacher as well as a classroom
								{
									oneLabperday += 1;
								}
									//cout<<kroom<<" "<<lroom<<" ";
								if(ktid.compare(ltid) == 0)
								{
								//	cout<<ktid<<ltid<<endl;
									oneLabperday += 1;
								}
							}						
						}
					}
				}
			}
		}
/*			
			for(int l = 0; l<csefaculty; l++)
			{
				if(availability[l][j]==0) 
					confAvail += 1;
			}
		}
*/
		for(int j = 0; j < labslots; j++)
			for(int k = 0; k < nLabs; k++)
				for(int l = k+1; l < nLabs; l++)
					if(conflicts[population[i].table[k][j]][population[i].table[l][j]] != 0)
						confAvail += 1;	/* Conflict checking for teachers and corrresponding rooms called to the lab room */ 



		int firstPeriod, secondPeriod;
		for(int m = 0; m < nLabs; m++)
			for(int n = 0; n < 5; n++)	//5 referring to no of days in a week
			{
				firstPeriod = n*labslots/5;
				secondPeriod = n*labslots/5+1;
				if(population[i].table[m][firstPeriod] == EMPTY)
					first2Hours += 1;
				if(population[i].table[m][secondPeriod] == EMPTY)	
					first2Hours += 1;
			}	


		//tempfitness = (17.0d/30)*confAvail + (4.d/30)*first2Hours + (9.0d/30)*oneLabperday;
		tempfitness = 0.8*confAvail + 0.05*first2Hours + 0.15*oneLabperday;
		//cout<<i<<" tempfitness : "<<tempfitness<<endl;
		//cout<<"confAvail : "<<confAvail<<endl;
		//cout<<"first2Hours : "<<first2Hours<<endl;
		//cout<<"oneLabperday : "<<oneLabperday<<endl;

		population[i].fitness = tempfitness;
		if(tempfitness < minvalue)
		{
			minvalue = tempfitness;
			minid = i;
		}
	}

	return minid;
}

int tournamentselection()
{
	double tournamentminfitness = POSITIVE_INFINITY;
	int tournamentwinnerid = 0;
	int tempint;
	for(int i = 0; i < tournamentsize; i++)
	{
		tempint = randomint(0,population.size()-1);
		if(population[tempint].fitness < tournamentminfitness)
		{
			tournamentminfitness = population[tempint].fitness;
			tournamentwinnerid = tempint;
		}
	}
	return tournamentwinnerid;
}

individual crossover(int a, int b)
{
	individual offspring;
	for(int i = 0; i<nLabs; i++)
	{
		vector <int> weekperiod;
		for(int j = 0; j < labslots; j++)
		{
			if(labInitial[i][j] == EMPTY)
			{
				weekperiod.push_back(population[b].table[i][j]);
			}
		}
		
		for(int j = 0; j < labslots; j++)
		{
			if(labInitial[i][j] != EMPTY)
			{
				offspring.table[i][j] = initial[i][j];
			}
			
			else 
			{
				if(j < labCrossverSplit)
				{
					offspring.table[i][j] = population[a].table[i][j];
					weekperiod.erase(find(weekperiod.begin(),weekperiod.end(),offspring.table[i][j]));
				}
				
				else 
				{
					offspring.table[i][j] = weekperiod[0];
					weekperiod.erase(weekperiod.begin());
				}
			}
		}
		
	}
	return offspring;
}


void get_variables(string filename = "csv/labCsv/labVariables.csv")
{
	ifstream in(filename);
	string line1,var1;
	int val,flag = 0;
	stringstream linestream;

	if(getline(in,line1,'\n'))
	{
		flag++;
		getline(in,var1,',');
		val = stoi(var1);
		nPeriodsPerWeek = val; 	
			
		getline(in,var1,',');
		val = stoi(var1);
		nteachers = val;

		getline(in,var1,',');
		val = stoi(var1);
		nLabSubjects = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		nRooms = val;

		getline(in,var1,',');
		val = stoi(var1);
		nLabs = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		firstLabRoom = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		tournamentsize = val;
		
		getline(in,var1,',');
		linestream<<var1;
		linestream>>mutationrate;
		
		getline(in,var1,',');
		val = stoi(var1);
		populationsize = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		generationlimit = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		elitism = val;

		getline(in,var1,',');
		val = stoi(var1);
		crossoversplit = val;
	}

	if(flag == 0)
		cout<<"\nerror: file for variables not found\n";

}

void get_periodcount(string filename = "csv/periodcount.csv")
{
	ifstream in;
	string tempstring;
	in.open(filename);
	string var1, var2, line1, line2;
	
	if(getline(in,line1,'\n'))
	{	//cout<<"Ji\n";
		for(int i = 0; i < nteachers && in.good(); i++)
		{
			getline(in,line1,'\n');

			stringstream linestream(line1);
			//cout<<line1<<endl;
			getline(linestream,var1,',');

			//cout<<var1<<endl;
			tempstring.assign(var1);
			//cout<<tempstring<<endl;
			teachers.push_back(tempstring);
			teacherid[tempstring] = i;
			
			for(int j = 0; j < nRooms && in.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);
				periodcount[j][i] = val;

			}
		}
	}
	else cout<<"\n error: file for periodcount not found\n";
}


void get_labPeriodcount(string filename = "csv/labCsv/labPeriodcount.csv")
{
	ifstream in;
	string tempstring;
	in.open(filename);
	string var1, var2, line1, line2;
	
	if(getline(in,line1,'\n'))
	{
		for(int i = 0; i < nLabSubjects && in.good(); i++)
		{
			getline(in,line1,'\n');

			stringstream linestream(line1);

			getline(linestream,var1,',');


			tempstring.assign(var1);
			
			labTeachers.push_back(tempstring);
			labTeacherid[tempstring] = i;
			
			for(int j = 0; j < nLabs && linestream.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);

				labPeriodcount[j][i] = val/2;	//for implementation labslots are considered half

			}
		}
	}
	else cout<<"\n error: file for labPeriodcount not found\n";
}



void get_initial(string filename = "csv/initial.csv")
{
	string var1, line1, tempstring;
	ifstream infile(filename);

	if(getline(infile,line1,'\n'))
	{
		for(int i = 0; i < nPeriodsPerWeek && infile.good(); i++)
		{

			getline(infile,line1,'\n');	// to ignore the initial token of hour no. in the week
   			stringstream linestream(line1);			
			getline(linestream,var1,',');	

			for(int j = 0; j < nRooms; j++)
			{

     			getline(linestream,var1,',');
				
				// to get rid of error caused by extra '\n' in final token from a line in the file
				if(j == nRooms-1)
					tempstring.assign(var1.begin(),var1.end());
				else
					tempstring.assign(var1);
				

				if(tempstring == "_")
					initial[j][i] = EMPTY;
				else
					initial[j][i] = teacherid[tempstring];
			}
		}
	}
	else cout<<"\n error: file for initial matrix not found";
}



void get_conflicts(string filename = "csv/labCsv/labConflicts.csv")
{
	string line,element;
	bool val;
	ifstream infile(filename);

	for(int i = 0; i < nLabSubjects && infile.good(); i++)
	{
		getline(infile,line,'\n');
		stringstream linestream(line);
		for(int j = 0; j < nLabSubjects && linestream.good(); j++)
		{	
			getline(linestream,element,',');
			stringstream valstream(element) ;
			valstream >> val;
			conflicts[i][j] = val;
		}
	}
}

void write_output(string filename = "csv/initialLabs.csv")
{
	
	ofstream out;
	out.open(filename);

	for(int i = 0; i < nRooms; i++)
	{
		out << "r" << i;
		if( i != nRooms-1)
			out << ",";
		else
			out << "\n";
	}

	for(int i = 0; i < nPeriodsPerWeek; i++)
	{
		for(int j = 0; j < nRooms; j++)
		{
			if(j >= firstLabRoom-1 && j <= lastLabRoom-1)
			{
				if(initial[j][i] == EMPTY)
					out << "_";
				else
					out << labTeachers[initial[j][i]];
				
				
				if( j != nRooms-1)
					out << ",";
				else
					out << "\n";
			}
			else
			{	
				if(initial[j][i] == EMPTY)
					out << "_";
				else
					out << teachers[initial[j][i]];

				if( j != nRooms-1)
					out << ",";
				else
					out << "\n";
			}
		}
	}
	out.close();
	
}


int main()
{
	randomoffset = 0;
	get_variables();
	labslots = nPeriodsPerWeek/2;
	labCrossverSplit = crossoversplit/2;
	lastLabRoom = firstLabRoom+nLabs-1;

	string tempstring;
	
	get_periodcount();
	get_labPeriodcount();

	//for(int i = 0; i < 55; i++)
	//	cout<<teachers[i]<<endl;
	//cout<<teachers[0]<<endl;

	

	get_conflicts();
	get_initial();	


	for(int i = 0, p = firstLabRoom; i < nLabs && p <= lastLabRoom; i++, p++)	//need to improve labInitial matrix initialization
	{
		for(int j = 0; j < labslots; j++)
		{
			labInitial[i][j] = initial[p][2*j];	//in labs, all periods are 2 slots long
		}
	}

/*
	output initial and periodcount matrices

	for(int i = 0; i < nLabSubjects; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nLabs; j++)
		{
			cout<<periodcount[j][i]<<" ";
		}
	}
	cout<<endl<<endl<<endl;

	for(int i = 0; i < nLabs; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j < labslots; j++)
		{
			cout<<labInitial[i][j]<<" ";
		}
	}

	//print initial
	for(int i = 0; i < nPeriodsPerWeek; i++)
	{
		cout<<endl<<'p'<<i<<"\t";	
		for(int j = 0; j < nRooms; j++)
		{
			cout<<initial[j][i]<<" ";
		}
	}
*/



	for(int i = 0; i<nLabSubjects; i++)
	{
		for(int j = 0; j<nPeriodsPerWeek; j++)
		{
				availability[i][j] = 1;
		}
	}
	

	/*for(int i = 0; i<nLabSubjects; i++)
	{
		for(int j = 0; j<nLabSubjects; j++)
		{
			if(i!=j)
				conflicts[i][j] = false;
			else 
				conflicts[i][j] = true;
		}
	}
	*/
	
	cout << "Finished input" << endl;
	
	//insert to population
	for(int i = 0; i<populationsize; i++)
	{
		individual newindividual;
		vector <int> weekperiod;
		for(int j = 0; j<nLabs; j++)
		{
			for(int k = 0; k<nLabSubjects; k++)
			{
				weekperiod.insert(weekperiod.end(), labPeriodcount[j][k],k);
			}

			for(int k = 0; k<labslots/5; k++)
			{
				for(int l = k; l<labslots; l+=labslots/5)
				{
					if(weekperiod.size()>0 && labInitial[j][l] == EMPTY)
					{
						tempint = randomint(0,weekperiod.size()-1);
						newindividual.table[j][l] = weekperiod[tempint];			//Spaced entering of teacher ids, which pushes free
						weekperiod.erase(weekperiod.begin()+tempint);				//hrs for end of day
					}
					else
						newindividual.table[j][l] = labInitial[j][l];
				}

/*				if(initial[j][k] == EMPTY && weekperiod.size()>0)
				{
					tempint = randomint(0,weekperiod.size()-1);
					newindividual.table[j][k] = weekperiod[tempint];
					weekperiod.erase(weekperiod.begin()+tempint);
				}
				else 
					newindividual.table[j][k] = initial[j][k];
*/
			}
		}
		population.push_back(newindividual);	

//display individual for checking
/*
		for(int k = 0; k<labslots; k++)
		{
			for(int j = 0; j<nLabs; j++)
			{
				if(population[i].table[j][k] == EMPTY)
					cout<<"_\t";
				else
					cout << labTeachers[population[i].table[j][k]] << "\t";
			}
			cout << endl;
		}
*/
	}
	


	//algorithm
	cout << "Starting genetic algorithm..." << endl;
		
	elapsedgenerations = 0;
	int elitismoffset = 0;
	
	if(elitism) 
		elitismoffset = 1;	//can make elitism offset a variable from the python gui to monitor algorithm benchmarks 
	
	while(elapsedgenerations < generationlimit)
	{
		vector <individual> newpopulation;
		
		//compute fitness, find minimum
		int minid = getminfitnessid();

		double minvalue = population[minid].fitness;
		if(elitism)
		{
			newpopulation.push_back(population[minid]);
		}
			
		//crossover;
		for(int i = elitismoffset; i<population.size(); i++)
		{
			int a = tournamentselection();
			int b = tournamentselection();
			individual offspring = crossover(a,b);
			newpopulation.push_back(offspring);		
		}
		
		
		//mutate;
		for(int i = elitismoffset; i<population.size(); i++)
		{
			for(int j = 0; j<nLabs; j++)
			{
				if(randombool(mutationrate))
				{
					int a, b;
					do 
					{
						a = randomint(0,labslots-1);
						b = randomint(0,labslots-1);
					} while((initial[j][a]!=EMPTY) || (initial[j][b]!=EMPTY));
					swap(newpopulation[i].table[j][a],newpopulation[i].table[j][b]);
				}
			}
		}
		

		population = newpopulation;

		elapsedgenerations++;
		
		cout << "Computed generation " << elapsedgenerations << ", minimum fitness at start: " << minvalue << endl;
	}
	
/*
	//print initial
	for(int i = 0; i < nPeriodsPerWeek; i++)
	{
		cout<<endl<<'p'<<i<<"\t";	
		for(int j = 0; j < nRooms; j++)
		{
			cout<<initial[j][i]<<" ";
		}
	}

*/
	int minid = getminfitnessid();
	
	cout << endl << "RESULT (fitness: " << population[minid].fitness << ")" << endl;
	//cout<<labslots<<endl;
/*
	for(int i = 0; i<labslots; i++)
	{
		for(int j = 0; j<nLabs; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else
				cout << teachers[population[minid].table[j][i]] << "\t";
		}
		cout << endl;
	}
*/

	for(int i = 0; i < labslots; i++)
	{
		if(i%(labslots/5) == 0)
			cout<<endl<<endl;
		for(int j = 0 ,p = firstLabRoom-1; j < nLabs && p < lastLabRoom; j++, p++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else
				cout << labTeachers[population[minid].table[j][i]] << "\t";
				
			initial[p][2*i] = population[minid].table[j][i];	//works for system with 2 slots for lab only
			initial[p][2*i+1] = population[minid].table[j][i];
		}
		cout << endl;
	}
/*	
//print initial
	for(int i = 0; i < nPeriodsPerWeek; i++)
	{
		cout<<endl<<'p'<<i<<"\t";	
		for(int j = 0; j < nRooms; j++)
		{
			cout << initial[j][i]<<" ";
		}
	}
*/
	write_output();

	return 0;

}