#ifndef GASPI_GPI_PROFILER_H
#define GASPI_GPI_PROFILER_H

#include <GASPI.h>
#include "gaspi_gpi.h"

#include <limits>
#include <map>
#include <string>
#include <iostream>

using namespace std;

enum {
	COUNTER_TOTAL_WRITE_VALUE,
	COUNTER_TOTAL_WRITE_INVOC,
	COUNTER_TOTAL_WRITE_MIN,
	COUNTER_TOTAL_WRITE_MAX,
	COUNTER_TOTAL_READ_VALUE,
	COUNTER_TOTAL_READ_INVOC,
	COUNTER_TOTAL_READ_MIN,
	COUNTER_TOTAL_READ_MAX,
	COUNTER_RANK_WRITE_VALUE,
	COUNTER_RANK_WRITE_INVOC,
	COUNTER_RANK_WRITE_MIN,
	COUNTER_RANK_WRITE_MAX,
	COUNTER_RANK_READ_VALUE,
	COUNTER_RANK_READ_INVOC,
	COUNTER_RANK_READ_MIN,
	COUNTER_RANK_READ_MAX
};

enum counter_op{
	OPERATION_ADD,
	OPERATION_MIN,
	OPERATION_MAX
};

class Counter
{
	public:
		Counter(gaspi_statistic_argument_t argument, string name, string description, gaspi_number_t verbosity_level) :
			argument_(argument), name_(name), description_(description), verbosity_level_(verbosity_level) {};

		gaspi_statistic_argument_t getStatisticArgument(void)const { return argument_; };
		string getName(void)const { return name_; };
		string getDescription(void)const { return description_; };
		gaspi_number_t getVerbosityLevel(void)const { return verbosity_level_; };

		virtual void add(const gaspi_number_t& argument, const gaspi_number_t& value) = 0;
		virtual void reset(void) = 0;
		//virtual void set(const gaspi_number_t& argument, const T& value) = 0;
		virtual gaspi_number_t getValue(const gaspi_number_t& argument)const = 0;
		
	private:
		gaspi_statistic_argument_t argument_;
		string name_;
		string description_;
		gaspi_number_t verbosity_level_;
};

class CounterBasic : public Counter
{
	public:
		CounterBasic(gaspi_statistic_argument_t argument, string name, string description, gaspi_number_t verbosity_level, counter_op operation) :
			Counter::Counter(argument, name, description, verbosity_level), operation_(operation) { reset(); };
		
		virtual void add(const gaspi_number_t& argument, const gaspi_number_t& value) { 
			switch(operation_){
				case OPERATION_ADD: value_ += value; break;
				case OPERATION_MIN: if(value < value_) value_ = value; break;
				case OPERATION_MAX: if(value > value_) value_ = value; break; 
				default: value_ += value;
			};
		};
		void reset(void) { 
			if(operation_ == OPERATION_MIN) 
				value_ = numeric_limits<gaspi_number_t>::max(); 
			else 
				value_ = 0; 
		};
		gaspi_number_t getValue(const gaspi_number_t& argument)const { return value_; };
		
	private:
		gaspi_number_t value_;
		const counter_op operation_;
};

class CounterBasicRank : public Counter
{
	public:
		CounterBasicRank(gaspi_statistic_argument_t argument, string name, string description, gaspi_number_t verbosity_level) :
			Counter::Counter(argument, name, description, verbosity_level) {};
		
		void add(const gaspi_number_t& argument, const gaspi_number_t& value) { values_[argument] += value; };
		void reset(void) { values_.clear(); };
		gaspi_number_t getValue(const gaspi_number_t& argument)const { 
			map<gaspi_number_t,gaspi_number_t>::const_iterator it = values_.find(argument);
			if(it == values_.end())
			{
				cout << "iwashere\n";
				return 0;
			}
			else
			{
				cout << "Value:" << it->second << "\n";
				return it->second;
			}
		}
	private:
		map<gaspi_number_t,gaspi_number_t> values_;
};

#endif
