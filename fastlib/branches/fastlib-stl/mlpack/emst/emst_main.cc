/**
* @file emst.cc
 *
 * Calls the DualTreeBoruvka algorithm from dtb.h
 * Can optionally call Naive Boruvka's method
 * See README for command line options.  
 *
 * @author Bill March (march@gatech.edu)
*/

#include "dtb.h"

#include <armadillo>
#include <fastlib/base/arma_compat.h>
#include <fastlib/fx/io.h>

PARAM_FLAG("using_thor", "For when an implementation of thor is around", 
    "emst");
PARAM_STRING_REQ("input_file", "Data input file.", "emst");
PARAM_STRING("output_file", "Data output file.", "emst", "emst_output.csv");

PARAM_FLAG("do_naive", "Check against naive.", "naive");
PARAM_STRING("output_file", "Naive data output file.", "naive",
    "naive_output.csv");

PARAM(double, "total_squared_length", "Calculation result.", "dtb", 0.0, false);

using namespace mlpack;

int main(int argc, char* argv[]) {
  IO::ParseCommandLine(argc, argv);
  
  // For when I implement a thor version
  bool using_thor = IO::GetParam<bool>("emst/using_thor");
  
  
  if (using_thor) {
    IO::Warn << "thor is not yet supported" << std::endl;
  }
  else {
      
    ///////////////// READ IN DATA ////////////////////////////////// 
    
    std::string data_file_name = IO::GetParam<std::string>("emst/input_file");
    
    arma::mat data_points;
    data::Load(data_file_name.c_str(), data_points);
    
    /////////////// Initialize DTB //////////////////////
    DualTreeBoruvka dtb;
    
    ////////////// Run DTB /////////////////////
    arma::mat results;
    
    dtb.ComputeMST(results);
    
    //////////////// Check against naive //////////////////////////
    if (IO::GetParam<bool>("naive/do_naive")) {
     
      DualTreeBoruvka naive;
      IO::GetParam<bool>("naive/do_naive") = true;
      
      naive.Init(data_points);
      
      arma::mat naive_results;
      naive.ComputeMST(naive_results);
      
      /* Compare the naive output to the DTB output */
      
      IO::StartTimer("naive/comparison");
      
           
      // Check if the edge lists are the same
      // Loop over the naive edge list
      int is_correct = 1;
      /*
      for (index_t naive_index = 0; naive_index < results.size(); 
           naive_index++) {
       
        int this_loop_correct = 0;
        index_t naive_lesser_index = results[naive_index].lesser_index();
        index_t naive_greater_index = results[naive_index].greater_index();
        double naive_distance = results[naive_index].distance();
        
        // Loop over the DTB edge list and compare against naive
        // Break when an edge is found that matches the current naive edge
        for (index_t dual_index = 0; dual_index < naive_results.size();
             dual_index++) {
          
          index_t dual_lesser_index = results[dual_index].lesser_index();
          index_t dual_greater_index = results[dual_index].greater_index();
          double dual_distance = results[dual_index].distance();
          
          if (naive_lesser_index == dual_lesser_index) {
            if (naive_greater_index == dual_greater_index) {
              DEBUG_ASSERT(naive_distance == dual_distance);
              this_loop_correct = 1;
              break;
            }
          }
          
        }
       
        if (this_loop_correct == 0) {
          is_correct = 0;
          break;
        }
        
      }
     */ 
      if (is_correct == 0) {
       
        IO::Warn << "Naive check failed!" << std::endl <<  
        "Edge lists are different." << std::endl << std::endl;
        
        // Check if the outputs have the same length
        if (IO::GetParam<double>("naive/total_squared_length") !=
           IO::GetParam<double>("naive/total_squared_length")) { 
          
          IO::Fatal << "Total lengths are different! " 
             << " One algorithm has failed." << std::endl;
             
          return 1;
        }
        else {
          // NOTE: if the edge lists are different, but the total lengths are
          // the same, the algorithm may still be correct.  The MST is not 
          // uniquely defined for some point sets.  For example, an equilateral
          // triangle has three minimum spanning trees.  It is possible for 
          // naive and DTB to find different spanning trees in this case.
          IO::Info << "Total lengths are the same."; 
          IO::Info << "It is possible the point set"; 
          IO::Info << "has more than one minimum spanning tree." << std::endl;
        }
      
      }
      else {
        IO::Info << "Naive and DualTreeBoruvka produced the same MST." <<
          std::endl << std::endl;
      }
      
      IO::StopTimer("naive/comparison");
      
      std::string naive_output_filename = 
          IO::GetParam<std::string>("naive/output_file");
      
      data::Save(naive_output_filename.c_str(), naive_results);
    }
    
    //////////////// Output the Results ////////////////
    
    std::string output_filename = 
        IO::GetParam<std::string>("emst/output_file");
   
    data::Save(output_filename.c_str(), results);
    
  }// end else (if using_thor)
  
  return 0;
  
}
