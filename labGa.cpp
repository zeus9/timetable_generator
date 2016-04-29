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
int nPeriodsPerWeek, nLabSubjects, nRooms, labslots, nLabs;
int populationsize, generationlimit;
int tournamentsize, tempint;
double mutationrate;
int elitism, crossoversplit, labCrossverSplit;	//csefaculty;


vector <string> teachers;
map <string, int> teacherid;


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
bool conflicts[MAX_TEACHERS][MAX_TEACHERS];
individual elite;
vector <individual> population;

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
	int n=0;
	int kroom, lroom;
	
	for(int i = 0; i<population.size(); i++)
	{
		double tempfitness = 0, first2Hours = 0, confAvail = 0, oneLabperday = 0;
		
		//calculate conflicts
		for(int j = 0; j<labslots; j++)
		{
			if((j%labslots/5)==0)
				count+=1;

			for(int k = 0; k<nLabs; k++)
			{
				if(population[i].table[k][j] == EMPTY)
					continue;

				else
				{	
					kteacher = teachers[population[i].table[k][j]];
					for(int m = 0; m<kteacher.size(); m++)
					{

						if(kteacher[m]=='r')
						{
							kroom = stoi(kteacher.substr(m+1,kteacher.size()-1));
						}
					

						if(kteacher[m] == '/')
							ktid = kteacher.substr(0,m-1);
					}


					for(int l = k+1; l<nLabs; l++)
					{
						if(population[i].table[l][j] == EMPTY)
								continue;
						else
						{	

							if(k!=l)
							{							
								if(conflicts[population[i].table[k][j]][population[i].table[l][j]] != 0)	/* Conflict checking for teachers and corrresponding rooms called to the lab room */ 
									confAvail += 1;

								for(int n = j; n<count*labslots/5; n++)
								{
									lteacher = teachers[population[i].table[l][n]];
									for(int m = 0; m<lteacher.size(); m++)
									{
							
										if(lteacher[m]=='r')
										{
											lroom = stoi(lteacher.substr(m+1,lteacher.size()-1));
										}	
	
										if(lteacher[m] == '/')
										{
											ltid = lteacher.substr(0,m-1);
										}
									}	
									
									if(kroom == lroom)	//checking for one lab/day for a teacher as well as a classroom
									{
										oneLabperday+=1;
										if(ktid == ltid)
											oneLabperday+=1;
									}	
								}						
							}



/*							if(j == count*nPeriodsPerWeek/5-1 )
							{	
								++count;
							}
							else
							{
								if(population[i].table[k][j] == population[i].table[l][j+1] && population[i].table[k][j]<csefaculty)
									consecutiveHours++;
							}
*/
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


		int firstPeriod, secondPeriod;
		for(int m = 0; m < nLabs; m++)
			for(int n = 0; n < 5; n++)
			{
				firstPeriod = n*labslots/5;
				secondPeriod = n*labslots/5+1;
				if(population[i].table[m][firstPeriod] == EMPTY)
					first2Hours += 1;
				if(population[i].table[m][secondPeriod] == EMPTY)	
					first2Hours += 1;
			}	


		tempfitness = (17/30)*confAvail + (4/30)*first2Hours +  (9/30)*oneLabperday;
		//cout<<"confAvail : "<<confAvail<<endl;
		//cout<<"first2Hours : "<<first2Hours<<endl;
		//cout<<"consecutiveHours : "<<consecutiveHours<<endl;


		population[i].fitness = tempfitness;
		if(tempfitness<minvalue)
		{
			minvalue = tempfitness;
			minid = i;
		}
		
	}

	return minid;

	}
}


int tournamentselection()
{
	double tournamentminfitness = POSITIVE_INFINITY;
	int tournamentwinnerid = 0;
	int tempint;
	for(int i = 0; i<tournamentsize; i++)
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
		for(int j = 0; j<labslots; j++)
		{
			if(initial[i][j] == EMPTY)
			{
				weekperiod.push_back(population[b].table[i][j]);
			}
		}
		
		for(int j = 0; j<labslots; j++)
		{
			if(initial[i][j] != EMPTY)
			{
				offspring.table[i][j] = initial[i][j];
			}
			
			else 
			{
				if(j<labCrossverSplit)
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


void get_variables(string filename = "csv/labsCsv/labvariables.csv")
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
		nLabSubjects = val;
		
		getline(in,var1,',');
		val = stoi(var1);
		nRooms = val;

		getline(in,var1,',');
		val = stoi(var1);
		nLabs = val;
		
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


void get_periodcount(string filename = "csv/labsCsv/labPeriodcount.csv")
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
			
			teachers.push_back(tempstring);
			teacherid[tempstring] = i;
			
			for(int j = 0; j < nLabs && linestream.good(); j++)
			{
				getline(linestream,var1,',');
				
				int val = stoi(var1);
				periodcount[j][i] = val/2;

			}
		}
	}
	else cout<<"\n error: file for periodcount not found\n";
}



void get_initial(string filename = "csv/initial.csv")
{
	string var1, line1, tempstring;
	ifstream infile(filename);

	if(getline(infile,line1,'\n'))
	{
		for(int i = 0; i < labslots && infile.good(); i++)
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


int main()
{
	randomoffset = 0;
	get_variables();
	labslots = nPeriodsPerWeek/2;
	labCrossverSplit = crossoversplit/2;

	string tempstring;
	
	get_periodcount();
	get_conflicts();
	get_initial();

	for(int i = 0; i < nLabs; i++)
	{
		for(int j = 0; j < labslots; j++)
		{
			labinitial[i][j] = EMPTY;	//for now assuming all labs slots are empty
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


	for(int i = 0; i < nRooms; i++)
	{
		cout<<endl<<i<<"\t";	
		for(int j = 0; j< nPeriodsPerWeek; j++)
		{
			cout<<initial[i][j]<<" ";
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
				weekperiod.insert(weekperiod.end(), periodcount[j][k],k);
			}

			for(int k = 0; k<labslots/5; k++)
			{
				for(int l = k; l<labslots ; l+=labslots/5)
				{
					if(weekperiod.size()>0)
					{
						tempint = randomint(0,weekperiod.size()-1);
						newindividual.table[j][l] = weekperiod[tempint];			//Spaced entering of teacher ids, which pushes free
						weekperiod.erase(weekperiod.begin()+tempint);				//hrs for end of day
					}

					else
						newindividual.table[j][l] = EMPTY;

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
				cout << teachers[population[i].table[j][k]] << "\t";
		}
		cout << endl;
	}

	
	}
*/
	


	//algorithm
	cout << "Starting genetic algorithm..." << endl;
		
	elapsedgenerations = 0;
	int elitismoffset = 0;
	
	if(elitism) 
		elitismoffset = 1;
	
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
			for(int j = 0; j<nRooms; j++)
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
	
	int minid = getminfitnessid();
	
	cout << endl << "RESULT (fitness: " << population[minid].fitness << ")" << endl;

	for(int i = 0; i<labslots; i++)
	{
		for(int j = 0; j<nRooms; j++)
		{
			if(population[minid].table[j][i] == EMPTY)
				cout<<"_\t";
			else
				cout << teachers[population[minid].table[j][i]] << "\t";
		}
		cout << endl;
	}

	
return 0;

}
