#include <stdio.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "partalloc.h"
#include "student.h"
#include "schchprob.h"
#include "parser.h"

struct school
{
	int quota;
	int position;
	int reserve;
};

int RSumCalc(struct school *PSet, int PSetCount) {
	int RSum = 0;

	for(int i = 0; i < PSetCount; i++) {
		RSum = RSum + PSet[i].reserve;
	}

	return RSum;
}

int minValChange(int posMin, int pastMin) {
	if(posMin < pastMin) {
		return posMin;
	}else {
		return pastMin;
	}
}

int SumOfSumCalc(struct school *PcSet, struct student *ISet,int PcSetCount, int ILength) {

	//Declaration of subset of I (JSet)
	struct student JSet[ILength];
	memset(JSet, 0, ILength * sizeof(struct student));

	int minSumOfSum = INT_MAX;

	for(int i = 0; i < (int) floor(pow(2, ILength)); i++) { //For Each JSubset
		int SumOfSum = 0;
		int JSetCount = 0;

		for(int j = 0; j < ILength; j++) { //Creating the subset (D.N.E)
			if(i & (1 << j)) {
				JSet[JSetCount] = ISet[j];
				JSetCount++;
			}
		}

		//ERROR in the for loop
		
		for(int j = 0; j < JSetCount; j++) {
			for(int k = 0; k < PcSetCount; k++) {
				SumOfSum = SumOfSum + JSet[j].eligibility[PcSet[k].position];
			}

			SumOfSum = SumOfSum - JSet[j].requirement;
		}

		minSumOfSum = minValChange(SumOfSum, minSumOfSum);
	}

	return minSumOfSum;
}

int GMCinequality(struct school *OSet, struct student *ISet, int ILength, int OLength) {

	//Min Value
	int minValue = INT_MAX;

	//Defining subset of O (PSet)
	struct school PSet[OLength];
	memset(PSet, 0, OLength*sizeof(struct school));

	//Defining the complement subset of P (PcSet)
	struct school PcSet[OLength];
	memset(PcSet, 0, OLength*sizeof(struct school));

	//For Each subset of OSet
	for (int i = 0; i < (int) floor(pow(2, OLength)); ++i)
	{
		//Defining Counts for each subset
		int PSetCount = 0;
		int PcSetCount = 0;

		//Defining 
		for(int j = 0; j < OLength; j++) {
			if(i & (1 << j)) {
				PSet[PSetCount] = OSet[j];
				//printf("%i" ,OSet[PSetCount].position);
				PSetCount++;
			}
			else {
				PcSet[PcSetCount] = OSet[i];
				PcSetCount++;
			}
		}

		int RSum = RSumCalc(PSet, PSetCount);

		int SumOfSum = SumOfSumCalc(OSet, ISet, PcSetCount, ILength); //Error

		int totalSum = SumOfSum + RSum;

		minValue = minValChange(totalSum, minValue);
	}

	return minValue;
}

int main(int argc, char const *argv[])
{
  struct input_sch_ch_prob my_sch_ch_prob;
  struct sch_ch_prob new_sch_ch_prob;
  struct sch_ch_prob red_sch_ch_prob;
    
  /*  my_sch_ch_prob = make_toy_sch_ch_prob(); */
  my_sch_ch_prob = sch_ch_prob_from_file(); 
  print_input_sch_ch_prob(my_sch_ch_prob);
  new_sch_ch_prob = sch_ch_prob_from_input(my_sch_ch_prob);
  destroy_input_sch_ch_prob(my_sch_ch_prob);
  print_sch_ch_prob(new_sch_ch_prob);
  red_sch_ch_prob = reduced_sch_ch_prob(new_sch_ch_prob);
  destroy_sch_ch_prob(new_sch_ch_prob);
  print_sch_ch_prob(red_sch_ch_prob);
  destroy_sch_ch_prob(red_sch_ch_prob);

  exit(0);
  
  my_sch_ch_prob = make_toy_sch_ch_prob();
  print_input_sch_ch_prob(my_sch_ch_prob);
  destroy_input_sch_ch_prob(my_sch_ch_prob);
  
  exit(0);
  
	int OLength = 3;
	int ILength = 3;

	struct school OSet[OLength];
	struct student ISet[ILength];

	for(int i = 0; i < OLength; i++) {
		OSet[i].reserve = 1;
		OSet[i].position = i;
		OSet[i].quota = 1;
	}

	for(int i = 0; i < ILength; i++) {
		ISet[i].requirement = 1;
		ISet[i].eligibility = malloc(OLength * sizeof(int));

		for(int j = 0; j < OLength; j++) {
			ISet[i].eligibility[j] = 1;
		}
	}

	int minValue = GMCinequality(OSet, ISet, OLength, ILength);

	printf("%i", minValue);
	
	return 0;
}
