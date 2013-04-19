#ifndef GASPI_GPI_PROFILER_H
#define GASPI_GPI_PROFILER_H

#include <GASPI.h>
#include "gaspi_gpi.h"

#include <limits>
#include <string>
#include <map>

// counter id
enum {
	COUNTER_FUNC_INVOC,
	COUNTER_FUNC_BYTES_TOTAL,
	COUNTER_FUNC_BYTES_MIN,
	COUNTER_FUNC_BYTES_MAX,
	COUNTER_FUNC_TIME_TOTAL,
	COUNTER_FUNC_TIME_MIN,
	COUNTER_FUNC_TIME_MAX,
	COUNTER_RANK_WRITE_VALUE,
	COUNTER_RANK_WRITE_INVOC,
	COUNTER_RANK_WRITE_MIN,
	COUNTER_RANK_WRITE_MAX,
	COUNTER_RANK_READ_VALUE,
	COUNTER_RANK_READ_INVOC,
	COUNTER_RANK_READ_MIN,
	COUNTER_RANK_READ_MAX,
	COUNTER_NUMBER_MAX // maximum counter id
};

// function id
enum {
	FUNC_GASPI_WRITE,
	FUNC_GASPI_READ,
	FUNC_NUMBER_MAX // maximum function id
};

//possible operations for Counter
class Operation
{
	public:
		virtual void doOperation(gaspi_number_t& value, const gaspi_number_t& newValue) = 0;
		virtual void reset(gaspi_number_t& value) = 0;
};

class OperationAdd : public Operation
{
	public:
		void doOperation(gaspi_number_t& value, const gaspi_number_t& newValue) { value += newValue; };
		void reset(gaspi_number_t& value) { value = 0; }
};

class OperationMin : public Operation
{
	public:
		void doOperation(gaspi_number_t& value, const gaspi_number_t& newValue) { if(newValue < value) value = newValue; };
		void reset(gaspi_number_t& value) { value = std::numeric_limits<gaspi_number_t>::max(); };
};

class OperationMax : public Operation
{
	public:
		void doOperation(gaspi_number_t& value, const gaspi_number_t& newValue) { if(newValue > value) value = newValue; };
		void reset(gaspi_number_t& value) { value = 0; };
};

//Counter
class Counter
{
	public:
		Counter(gaspi_statistic_argument_t argument, std::string name, std::string description, gaspi_number_t verbosity_level) :
			argument_(argument), name_(name), description_(description), verbosity_level_(verbosity_level) {};

		gaspi_statistic_argument_t getStatisticArgument(void)const { return argument_; };
		std::string getName(void)const { return name_; };
		std::string getDescription(void)const { return description_; };
		gaspi_number_t getVerbosityLevel(void)const { return verbosity_level_; };

		virtual void doOperation(const gaspi_number_t& argument, const gaspi_number_t& value) = 0;
		virtual void reset(void) = 0;
		//virtual void set(const gaspi_number_t& argument, const T& value) = 0;
		virtual gaspi_number_t getValue(const gaspi_number_t& argument)const = 0;
		
	private:
		gaspi_statistic_argument_t argument_;
		std::string name_;
		std::string description_;
		gaspi_number_t verbosity_level_;
};

// Basic Counter with no arguments
class CounterBasic : public Counter
{
	public:
		CounterBasic(gaspi_statistic_argument_t argument, std::string name, std::string description, gaspi_number_t verbosity_level, 
			Operation* operation) :
			Counter::Counter(argument, name, description, verbosity_level), operation_(operation) { reset(); };
		
		void doOperation(const gaspi_number_t& argument, const gaspi_number_t& value) { 
			(*operation_).doOperation(value_, value); };
		void reset(void) { (*operation_).reset(value_); };
		gaspi_number_t getValue(const gaspi_number_t& argument)const { return value_; };
		
	private:
		gaspi_number_t value_;
		Operation* operation_;
};

// counts values for different functions - argument is a function id
class CounterBasicFunc : public Counter
{
	public:
		CounterBasicFunc(gaspi_statistic_argument_t argument, std::string name, std::string description, gaspi_number_t verbosity_level, Operation* operation) :
			Counter::Counter(argument, name, description, verbosity_level), operation_(operation) { reset();};
		
		void doOperation(const gaspi_number_t& argument, const gaspi_number_t& value) {
			if(argument <= FUNC_NUMBER_MAX)
				(*operation_).doOperation(values_[argument], value); 
		}
		void reset(void) { gaspi_number_t i; for(i = 0; i < FUNC_NUMBER_MAX; i++) (*operation_).reset(values_[i]); };
		gaspi_number_t getValue(const gaspi_number_t& argument)const { 
			if(argument <= FUNC_NUMBER_MAX)
				return values_[argument];
			
			return 0;
		}
	private:
		gaspi_number_t values_[FUNC_NUMBER_MAX];
		Operation* operation_;
};

// counts values for different ranks - argument is a rank id
class CounterBasicRank : public Counter
{
	public:
		CounterBasicRank(gaspi_statistic_argument_t argument, std::string name, std::string description, gaspi_number_t verbosity_level, Operation* operation) : Counter::Counter(argument, name, description, verbosity_level), operation_(operation) {};
		
		void doOperation(const gaspi_number_t& argument, const gaspi_number_t& value) { 
			(*operation_).doOperation(values_[argument], value); 
		};
		void reset(void) { values_.clear(); };
		gaspi_number_t getValue(const gaspi_number_t& argument)const { 
			std::map<gaspi_number_t,gaspi_number_t>::const_iterator it = values_.find(argument);
			if(it == values_.end())
				return 0;
			else
				return it->second;
		}
	private:
		std::map<gaspi_number_t,gaspi_number_t> values_;
		Operation* operation_;
};

#endif
