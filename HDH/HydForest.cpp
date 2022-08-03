/* HydForest 04/03/2021

 $$$$$$$$$$$$$$$$$$$$$
 $   HydForest.cpp   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A wrapper for the random forest implementation 'ranger'
 
 https://github.com/imbs-hl/ranger
 
 https://cran.r-project.org/web/packages/ranger/ranger.pdf
 
*/


#ifndef __HYDFOREST_H__
#include "HydForest.h"
#endif

#ifndef __HYDDATA_H__
#include "HydData.h"
#endif



#include "globals.h"
#include "ArgumentHandler.h"
#include "ForestClassification.h"
#include "ForestRegression.h"
#include "ForestSurvival.h"
#include "ForestProbability.h"
#include "utility.h"



using namespace ranger;

HydForest::HydForest( void ) : ranger::ForestClassification(), 
                               m_seed(13), 
                               m_haveForest(0), 
                               m_config(), 
                               m_trace()
{
    
    m_logger.getLogLevel( "HDHeuristic" );
    verbose_out = &m_trace;  
}

HydForest::HydForest( const HDHConfig& config ) : ranger::ForestClassification(), 
                                                                    m_seed(13), 
                                                                    m_haveForest(0), 
                                                                    m_config(config), 
                                                                    m_trace()
{
    m_logger.getLogLevel( "HDHeuristic" );
    m_config = config;
    dependent_variable_names = std::vector<std::string>(1,m_config.outputName());
    verbose_out = &m_trace;  
}

void
HydForest::init( const std::string& forestName, 
      const std::vector<std::string>& in_names, 
      const std::vector<std::string>& out_names )
{
    m_config.name(forestName);
    m_config.inputNames(in_names);
    dependent_variable_names = out_names;
}

void
HydForest::init( const HDHConfig& config )
{
    m_config = config;
    dependent_variable_names = std::vector<std::string>(1,m_config.outputName());
}

std::vector<double>
HydForest::predict( std::vector<double>& inputs )
{
    if (!m_haveForest)
    {
        if (m_logger.level() >= 1)
        {
            writeOutput();
            
            Message txt(1);
            txt << "Predict Error: no forest exists for prediction";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return std::vector<double>();
    }
    
    int Nouts = dependent_variable_names.size();
    this->data = std::make_unique<HydData>(m_config.inputNames(), inputs, std::vector<double>(Nouts, 0.0));
    num_samples = 1;
    
    bool calc_oob = true;
    bool verbose = true;
    prediction_mode = true; // learn = false, predict = true
    run(verbose, calc_oob);
    
    if (m_logger.level() >= 3)
    {
        writeOutput();
        
        Message txt(3);
        txt << "Predict output....\n" << m_trace.str();
        m_logger.logMsg(txt);
        m_logger.flush();
        
        // clear the trace stream
        m_trace.str("");
    }
    
    if (!predictions.empty() && !predictions[0].empty())
        return predictions[0][0];
    
    return std::vector<double>();
}

bool 
HydForest::learn( const Matrix<double>& inputs, 
                  const Matrix<double>& outputs ) 
{
    assert(inputs.rows() == outputs.rows());
    
    if (m_haveForest)
    {
        if (m_logger.level() >= 1)
        {
            writeOutput();
            
            Message txt(1);
            txt << "Learn Error: a forest already exists";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false; 
    }
    
    MemoryMode memory_mode         = MemoryMode::MEM_DOUBLE;
    uint mtry                      = 0;
    std::string output_prefix      = m_config.name();
    uint num_trees                 = m_config.numTrees(); // DEFAULT_NUM_TREE;
    uint seed                      = m_seed;
    uint num_threads               = m_config.numThreads(); // DEFAULT_NUM_THREADS;
    ImportanceMode importance_mode = DEFAULT_IMPORTANCE_MODE; // IMP_GINI
    uint min_node_size             = DEFAULT_MIN_NODE_SIZE_CLASSIFICATION;
    
    bool prediction_mode           = false; // learn = false, predict = true
    
    bool sample_with_replacement   = true;
    std::vector<std::string> unordered_variable_names;
    bool memory_saving_splitting   = false;
    SplitRule splitrule            = DEFAULT_SPLITRULE;
    bool predict_all               = false;
    std::vector<double> sample_fraction = { DEFAULT_SAMPLE_FRACTION_REPLACE };
    double alpha                   = DEFAULT_ALPHA;
    double minprop                 = DEFAULT_MINPROP;
    bool holdout                   = false; 
    PredictionType prediction_type = DEFAULT_PREDICTIONTYPE;
    uint num_random_splits         = DEFAULT_NUM_RANDOM_SPLITS;
    bool order_snps                = false;
    uint max_depth                 = m_config.maxDepth() ; //DEFAULT_MAXDEPTH;
    const std::vector<double> regularization_factor;
    bool regularization_usedepth   = false;
    
    std::unique_ptr<Data> input_data = std::make_unique<HydData>(m_config.inputNames(), inputs, outputs);
    

    try
    {
        init(memory_mode, std::move(input_data), mtry, output_prefix,
                            num_trees, seed, num_threads, importance_mode, min_node_size,
                            prediction_mode, sample_with_replacement, unordered_variable_names,
                            memory_saving_splitting, splitrule, predict_all, sample_fraction,
                            alpha, minprop, holdout, prediction_type, num_random_splits,
                            order_snps, max_depth, regularization_factor, regularization_usedepth);
        
        bool calc_oob = true;
        bool verbose = true;
        
        run(verbose, calc_oob);
    }
    catch ( const std::runtime_error e )
    {
        if (m_logger.level() >= 1)
        {
            writeOutput();
            
            Message txt(1);
            txt << "Error: " << e.what();
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
    
    if (m_logger.level() >= 2)
    {
        writeOutput();
        
        Message txt(2);
        txt << "Learning output....\n" << m_trace.str();
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    // clear the trace stream
    m_trace.str("");
    m_haveForest = 1;
    return true;
}


bool 
HydForest::save( const std::string& name )
{
    if (!name.empty())
        output_prefix = name;
    
    try
    {
        saveToFile();
    }
    catch ( const std::runtime_error e )
    {
        if (m_logger.level() >= 1)
        {
            writeOutput();
            
            Message txt(1);
            txt << "Error: " << e.what();
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
        
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << m_trace.str();
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    // clear the trace stream
    m_trace.str("");
    
    output_prefix = m_config.name();
    return true;
}

bool 
HydForest::load( const std::string& name )
// Ive modified the ranger::Forest::initCpp(...) method
{
    if (!name.empty())
        m_config.name(name);
    
    // set arguments for initialisation
    MemoryMode memory_mode         = MemoryMode::MEM_DOUBLE;
    uint mtry                      = 0;
    std::string output_prefix      = m_config.name();
    uint num_trees                 = m_config.numTrees(); //DEFAULT_NUM_TREE;
    uint seed                      = m_seed;
    uint num_threads               = m_config.numThreads(); //DEFAULT_NUM_THREADS;
    ImportanceMode importance_mode = DEFAULT_IMPORTANCE_MODE;
    uint min_node_size             = DEFAULT_MIN_NODE_SIZE_CLASSIFICATION;
    bool sample_with_replacement   = true;
    bool memory_saving_splitting   = false;
    bool predict_all               = false;
    bool holdout                   = false; 
    PredictionType prediction_type = DEFAULT_PREDICTIONTYPE;
    SplitRule splitrule            = DEFAULT_SPLITRULE;
    uint num_random_splits         = DEFAULT_NUM_RANDOM_SPLITS;
    double alpha                   = DEFAULT_ALPHA;
    double minprop                 = DEFAULT_MINPROP;
    uint max_depth                 = m_config.maxDepth(); //DEFAULT_MAXDEPTH;
    std::vector<std::string> unordered_variable_names;
    const std::vector<double> regularization_factor = {1.0};
    bool regularization_usedepth   = false;
    

    bool prediction_mode = false;
    
    this->verbose_out = verbose_out;
    
    // Sample fraction default and convert to vector
    std::vector<double> sample_fraction_vector = { DEFAULT_SAMPLE_FRACTION_REPLACE };
    if (sample_fraction_vector[0] == 0) 
    {
        if (sample_with_replacement) 
            sample_fraction_vector[0] = DEFAULT_SAMPLE_FRACTION_REPLACE;
        else sample_fraction_vector[0] = DEFAULT_SAMPLE_FRACTION_NOREPLACE;
    }
    
    try
    {
        // load the forest variable names
        std::string load_forest_filename = name;
        loadDependentVariableNamesFromFile(load_forest_filename);

        // call init function - must specify number of inputs and outputs before init
        std::vector<double> inputs(m_config.inputNames().size(), 0.0); 
        std::vector<double> outputs(dependent_variable_names.size(), 0.0);
        std::unique_ptr<Data> input_data = std::make_unique<HydData>(m_config.inputNames(), inputs, outputs);
        
        init(memory_mode, std::move(input_data), mtry, output_prefix, num_trees, seed, num_threads, importance_mode,
             min_node_size, prediction_mode, sample_with_replacement, unordered_variable_names, memory_saving_splitting,
             splitrule, predict_all, sample_fraction_vector, alpha, minprop, holdout, prediction_type, num_random_splits,
             false, max_depth, regularization_factor, regularization_usedepth);
        
        // Load the forest
        loadFromFile(load_forest_filename);
        
        std::vector<std::string> always_split_variable_names;
        // Set variables to be always considered for splitting
        if (!always_split_variable_names.empty()) 
            setAlwaysSplitVariables(always_split_variable_names);
        
        std::string split_select_weights_file;
        // TODO: Read 2d weights for tree-wise split select weights
        // Load split select weights from file
        if (!split_select_weights_file.empty()) 
        {
            std::vector<std::vector<double>> split_select_weights;
            split_select_weights.resize(1);
            loadDoubleVectorFromFile(split_select_weights[0], split_select_weights_file);
            if (split_select_weights[0].size() != num_independent_variables) 
                throw std::runtime_error("Number of split select weights is not equal to number of independent variables.");
            
            setSplitWeightVector(split_select_weights);
        }
        
        std::string case_weights_file;
        // Load case weights from file
        if (!case_weights_file.empty()) 
        {
            loadDoubleVectorFromFile(case_weights, case_weights_file);
            if (case_weights.size() != num_samples) 
                throw std::runtime_error("Number of case weights is not equal to number of samples.");
        }
        
        // Sample from non-zero weights in holdout mode
        if (holdout && !case_weights.empty()) 
        {
            size_t nonzero_weights = 0;
            for (auto& weight : case_weights) 
            {
                if (weight > 0) 
                    ++nonzero_weights;
            }
            this->sample_fraction[0] = this->sample_fraction[0] * ((double) nonzero_weights / (double) num_samples);
        }
        
        // Check if all catvars are coded in integers starting at 1
        if (!unordered_variable_names.empty()) 
        {
            std::string error_message = checkUnorderedVariables(*data, unordered_variable_names);
            if (!error_message.empty()) 
                throw std::runtime_error(error_message);
        }
    }
    catch ( const std::runtime_error e )
    {
        if (m_logger.level() >= 1)
        {
            // writeOutput();
            // clear the trace stream
            m_trace.str("");
            Message txt(1);
            txt << "Error: " << e.what();
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
    
    if (m_logger.level() >= 3)
    {
        writeOutput();
        Message txt(3);
        txt << m_trace.str();
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    // clear the trace stream
    m_trace.str("");
    m_haveForest = 1;
    return true;
}


