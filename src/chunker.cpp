#include <Rcpp.h>
#include <string>
#include <vector>

using namespace Rcpp;

std::vector< std::string > sliding_window(std::string sequence, int window_size)
{
  int win_size{ window_size - 1 };
  std::vector<char> chars( sequence.begin(), sequence.end() );
  int num_chunks( chars.size() - win_size );
  std::vector< std::string > acc;
  for (int i=0; i < num_chunks; ++i)
    acc.push_back(sequence.substr(i, win_size));
  return acc;
}

// [[Rcpp::export]]
DataFrame chunker(DataFrame seq_table,
                  int window_size = 20) 
{

  std::vector< std::string > ids = seq_table["Id"];
  std::vector< std::string > seqs = seq_table["Seq"];
  
  std::vector< std::string > seq_chunk_acc;
  std::vector< std::string > seq_id_acc;

  for (int inc1 = 0; inc1 < seqs.size(); ++inc1) {
    std::vector< std::string > seq_acc(sliding_window(seqs[inc1], window_size));
    std::string seq_id = ids[inc1];
    for (int inc2 = 0 ; inc2 < seq_acc.size() ; ++inc2) {
      seq_chunk_acc.push_back(seq_acc[inc2]);
      seq_id_acc.push_back(seq_id + "_" + std::to_string(inc2 + 1));
    }
  }

  return DataFrame::create(Named("Id") = seq_id_acc,
                           Named("Seq") = seq_chunk_acc);
}


