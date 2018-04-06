# TidyMallet
A tidy-native LDA implementation in Rcpp

The fundamental data structure for Gibbs sampling in LDA is a sequence of tokens, each with a word ID and a topic variable, grouped into documents. It's essentially a tidy text format. So why not run the algorithm directly on the tidy data structure?

I use Rcpp to create a function that implements one sweep of a Gibbs sampler, and that can be used by dplyr as long as tokens are grouped into documents:

    tokens_df %>% mutate(word_id = word_id, topic = sampleTopics(word_id, topic, topic_model))

The key to efficient sampling is being able to get the current topic counts for the word type at the current token. The best way I could figure out to do this is to have a data frame with one column per word type and one row per topic. It's cumbersome and weird, but I'm not good enough at Rcpp to find a better way. Suggestions are welcome!

Right now this is a very straightforward implementation, with none of the tricks that I used in Mallet. For small collections it's suitably fast, but there are some simple improvements that might make a difference.
