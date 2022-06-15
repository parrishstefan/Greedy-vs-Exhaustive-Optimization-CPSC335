///////////////////////////////////////////////////////////////////////////////
// maxcalorie_test.cc
//
// Unit tests for maxcalorie.hh
//
///////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <sstream>


#include "maxcalorie.hh"
#include "rubrictest.hh"


int main()
{
	Rubric rubric;
	
	FoodVector trivial_foods;
	trivial_foods.push_back(std::shared_ptr<FoodItem>(new FoodItem("test whole corn", 100.0, 20.0)));
	trivial_foods.push_back(std::shared_ptr<FoodItem>(new FoodItem("test pasta", 40.0, 5.0)));
	
	auto all_foods = load_food_database("food.csv");
	assert( all_foods );
	
	auto filtered_foods = filter_food_vector(*all_foods, 1, 2500, all_foods->size());
	
	//
	rubric.criterion(
		"load_food_database still works", 2,
		[&]()
		{
			TEST_TRUE("non-null", all_foods);
			TEST_EQUAL("size", 8064, all_foods->size());
		}
	);
	
	//
	rubric.criterion(
		"filter_food_vector", 2,
		[&]()
		{
			auto
				three = filter_food_vector(*all_foods, 100, 500, 3),
				ten = filter_food_vector(*all_foods, 100, 500, 10);
			TEST_TRUE("non-null", three);
			TEST_TRUE("non-null", ten);
			TEST_EQUAL("total_size", 3, three->size());
			TEST_EQUAL("total_size", 10, ten->size());
			TEST_EQUAL("contents", "refried spicy delicious beans", (*ten)[0]->description());
			TEST_EQUAL("contents", "baked MSG-free chocolate", (*ten)[9]->description());
			for (int i = 0; i < 3; i++) {
				TEST_EQUAL("contents", (*three)[i]->description(), (*ten)[i]->description());
			}
		}
	);
	
	//
	rubric.criterion(
		"greedy_max_calories trivial cases", 2,
		[&]()
		{
			std::unique_ptr<FoodVector> soln;
			
			soln = greedy_max_calories(trivial_foods, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());
			
			soln = greedy_max_calories(trivial_foods, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("whole corn only", 1, soln->size());
			TEST_EQUAL("whole corn only", "test whole corn", (*soln)[0]->description());
			
			soln = greedy_max_calories(trivial_foods, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("pasta only", 1, soln->size());
			TEST_EQUAL("pasta only", "test pasta", (*soln)[0]->description());
			
			soln = greedy_max_calories(trivial_foods, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("whole corn and pasta", 2, soln->size());
			TEST_EQUAL("whole corn and pasta", "test whole corn", (*soln)[0]->description());
			TEST_EQUAL("whole corn and pasta", "test pasta", (*soln)[1]->description());
		}
	);
	
	//
	rubric.criterion(
		"greedy_max_calories correctness", 4,
		[&]()
		{
			std::unique_ptr<FoodVector> soln_small, soln_large;
			
			soln_small = greedy_max_calories(*filtered_foods, 500),
			soln_large = greedy_max_calories(*filtered_foods, 5000);
			
			//print_food_vector(*soln_small);
			//print_food_vector(*soln_large);
			
			TEST_TRUE("non-null", soln_small);
			TEST_TRUE("non-null", soln_large);
			
			TEST_FALSE("non-empty", soln_small->empty());
			TEST_FALSE("non-empty", soln_large->empty());
			
			double
				weight_small, calories_small,
				weight_large, calories_large
				;
			sum_food_vector(*soln_small, weight_small, calories_small);
			sum_food_vector(*soln_large, weight_large, calories_large);
			
			//	Precision
			weight_small	= std::round( weight_small	* 100 ) / 100;
			calories_small	= std::round( calories_small	* 100 ) / 100;
			weight_large	= std::round( weight_large	* 100 ) / 100;
			calories_large	= std::round( calories_large	* 100 ) / 100;
			
			TEST_EQUAL("Small solution weight",	481.48,	weight_small);
			TEST_EQUAL("Small solution calories",	950.19,	calories_small);
			TEST_EQUAL("Large solution weight",	4990.35,	weight_large);
			TEST_EQUAL("Large solution calories",	9209.82,	calories_large);
		}
	);
	
	//
	rubric.criterion(
		"exhaustive_max_calories trivial cases", 2,
		[&]()
		{
			std::unique_ptr<FoodVector> soln;
			
			soln = exhaustive_max_calories(trivial_foods, 10);
			TEST_TRUE("non-null", soln);
			TEST_TRUE("empty solution", soln->empty());
			
			soln = exhaustive_max_calories(trivial_foods, 100);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("wholecorn only", 1, soln->size());
			TEST_EQUAL("whole corn only", "test whole corn", (*soln)[0]->description());
			
			soln = exhaustive_max_calories(trivial_foods, 99);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("pasta only", 1, soln->size());
			TEST_EQUAL("pasta only", "test pasta", (*soln)[0]->description());
			
			soln = exhaustive_max_calories(trivial_foods, 150);
			TEST_TRUE("non-null", soln);
			TEST_EQUAL("whole corn and pasta", 2, soln->size());
			TEST_EQUAL("whole corn and pasta", "test whole corn", (*soln)[0]->description());
			TEST_EQUAL("whole corn and pasta", "test pasta", (*soln)[1]->description());
		}
	);
	
	//
	rubric.criterion(
		"exhaustive_max_calories correctness", 4,
		[&]()
		{
			std::vector<double> optimal_calories_totals =
			{
				500,		1033.05,	1100,	1600,	1600,
				1600,		1900,		2100,	2300,	2300,
				2300,		2300,		2400,	2400,	2400,
				2400,		2400,		2400,	2400,	2400
			};
			
			for ( int optimal_index = 0; optimal_index < optimal_calories_totals.size(); optimal_index++ )
			{
				int n = optimal_index + 1;
				double expected_calories = optimal_calories_totals[optimal_index];
				
				auto small_foods = filter_food_vector(*filtered_foods, 1, 2000, n);
				TEST_TRUE("non-null", small_foods);
				
				auto solution = exhaustive_max_calories(*small_foods, 2000);
				TEST_TRUE("non-null", solution);
				
				double actual_weight, actual_calories;
				sum_food_vector(*solution, actual_weight, actual_calories);
				
				// Round
				expected_calories	= std::round( expected_calories	/ 100.0) * 100;
				actual_calories		= std::round( actual_calories	/ 100.0) * 100;
				
				std::stringstream ss;
				ss
					<< "exhaustive search n = " << n << " (optimal index = " << optimal_index << ")"
					<< ", expected calories = " << expected_calories
					<< " but algorithm found = " << actual_calories
					;
				TEST_EQUAL(ss.str(), expected_calories, actual_calories);
				
				auto greedy_solution = greedy_max_calories(*small_foods, 2000);
				double greedy_actual_weight, greedy_actual_calories;
				sum_food_vector(*solution, greedy_actual_weight, greedy_actual_calories);
				greedy_actual_calories	= std::round( greedy_actual_calories	/ 100.0) * 100;
				TEST_EQUAL("Exhaustive and greedy get the same answer", actual_calories, greedy_actual_calories);
			}
		}
	);

	return rubric.run();
}




