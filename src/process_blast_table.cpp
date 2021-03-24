#include <Rcpp.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

using namespace Rcpp;

//' @export
// [[Rcpp::export]]
List process_blast_table(std::string filename, int hit_len) 
{
  std::fstream csv_file(filename);    
  std::string line;

  std::unordered_set< std::string > primer_set;
  std::unordered_set< std::string > seq_set;

  std::getline(csv_file, line);

  while(std::getline(csv_file, line)){

    std::istringstream s(line);
    std::string field;

    std::string QueryId;
    std::string TargetId;
    int TargetMatchStart;
    int TargetMatchEnd;
    std::string QueryMatchSeq;
    std::string TargetMatchSeq;

    int ix { 0 };
    while (std::getline(s, field, ',')) {
      switch (ix)
        {
        case 0:
          QueryId = field;
          break;
        case 1:
          TargetId = field;
          break;
        case 4:
          TargetMatchStart = std::stoi(field);
          break;
        case 5:
          TargetMatchEnd = std::stoi(field);
          break;
        case 6:
          QueryMatchSeq = field;
          break;
        case 7:
          TargetMatchSeq = field;
          break;
        }
      ++ix;
    }

    std::string IndId;
    std::istringstream QId(QueryId);

    while (std::getline(QId, IndId, ';')) {

      int obs_hit_len = TargetMatchEnd - TargetMatchStart;

      std::string TruncQueryId { IndId.substr(0, IndId.find("_")) };
      std::string seq_pair { TruncQueryId + "," + TargetId };
      if (TruncQueryId != TargetId && QueryMatchSeq == TargetMatchSeq)
        seq_set.insert(seq_pair);

      std::string LongTargetId { TargetId + "_" + std::to_string(TargetMatchStart) };
      std::string primer_pair { IndId + "," + LongTargetId };
      if (IndId != LongTargetId && QueryMatchSeq == TargetMatchSeq)
        primer_set.insert(primer_pair);
    }
  }
  
  std::vector < std::string > left_primers;
  std::vector < std::string > right_primers;

  for (std::string pair : primer_set)
    {
      std::string left_primer { pair.substr(0, pair.find(",")) };
      std::string right_primer { pair.substr(pair.find(",") + 1, pair.size()) };
      left_primers.push_back(left_primer);
      right_primers.push_back(right_primer);
    }

  std::vector < std::string > left_seqs;
  std::vector < std::string > right_seqs;

  for (std::string pair : seq_set)
    {
      std::string left_seq { pair.substr(0, pair.find(",")) };
      std::string right_seq { pair.substr(pair.find(",") + 1, pair.size()) };
      left_seqs.push_back(left_seq);
      right_seqs.push_back(right_seq);
    }
  
  DataFrame primer_table = DataFrame::create(Named("Left") = left_primers,
                                             Named("Right") = right_primers);

  DataFrame seq_table = DataFrame::create(Named("Left") = left_seqs,
                                          Named("Right") = right_seqs);

  return List::create(primer_table, seq_table);

}
