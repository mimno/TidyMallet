// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
NumericVector sampleTopics(NumericVector word_ids, NumericVector topics, List model) {
  
  
  int numTopics = model["num.topics"];
  NumericVector topicCounts(numTopics);
  int numTokens = word_ids.size();
  int oldTopic, newTopic;
  double distribution [numTopics];
  NumericVector tokensPerTopic = model["tokens_per_topic"];
  NumericVector alpha = model["alpha"];
  //List topicSumList = model["tokens_per_topic"];
  //NumericVector tokensPerTopic = topicSumList[1];
  List typeTopicCounts = model["type_topic_counts"];
  double beta = model["beta"];
  double betaSum = model["beta_sum"];
  double sum, sample;
  NumericVector samples = Rcpp::runif(numTokens,0,1.0);  // Calvinist predestination
  
  //Rcout << "doc with " << numTokens << " tokens\n";
  for (int i = 0; i < numTokens; i++) {
    topicCounts[ topics[i]-1 ]++;
  }
  
  for (int i = 0; i < numTokens; i++) {
    int word_id = word_ids[i];
    
    //std::unordered_map<String,int>::iterator iterator = vocabMap.find(word);
    //int wordID = iterator->second;
    NumericVector currentTypeTopicCounts = typeTopicCounts[word_id-1]; // minus 1 for C++
    
    oldTopic = topics[i] - 1;
    topicCounts[oldTopic]--;
    if (topicCounts[oldTopic] < 0) {
      Rcout << "topic count < 0 for " << oldTopic << "\n";
    }
    currentTypeTopicCounts[oldTopic]--;
    if (currentTypeTopicCounts[oldTopic] < 0) {
      Rcout << "type topic count < 0 for " << word_id << " in " << oldTopic << "\n";
    }
    tokensPerTopic[oldTopic]--;
    
    sum = 0.0;
    for (int topic = 0; topic < numTopics; topic++) {
      distribution[topic] = 
        (alpha[topic] + topicCounts[topic]) * (beta + currentTypeTopicCounts[topic]) / (betaSum + tokensPerTopic[topic]);
      sum += distribution[topic];
      //Rcout << (alpha[topic] + topicCounts[topic]) << " * " << (beta + currentTypeTopicCounts[topic]) << " / " << (betaSum + tokensPerTopic[topic]) << "\n";
    }
    //Rcout << "\n";
    
    sample = samples[i] * sum;
    
    newTopic = 0;
    while (sample > distribution[newTopic]) {
      sample -= distribution[newTopic];
      newTopic++;
    }
    
    topicCounts[newTopic]++;
    currentTypeTopicCounts[newTopic]++;
    tokensPerTopic[newTopic]++;
    topics[i] = newTopic + 1;
    
  }
  
  // It looks like this array might be passing by value rather than by reference? But typeTopicCounts seems fine.
  model["tokens_per_topic"] = tokensPerTopic;
  
  return topics;
}
