#' Constructor for a TidyMallet topic model
#' @param tokens_df A data frame with one row per token
#' @param num.topics The number of topics to fit
#' @param alphaSum How much weight do you want to place on the uniform prior distribution?
#' @param beta How open should topics be to new words?
TidyMallet <- function (tokens_df, num.topics=10, alphaSum=5.0, beta=0.01) {
  
  ## summary statistics
  tokens_per_topic <- tokens_df %>% group_by(topic) %>% summarise(count=n())
  
  word_topic_counts <- tokens_df %>% group_by(word, topic) %>% summarise(topic.count = n())
  
  ## I want to be able to address the topic counts for a given word as a string, and then have those counts contiguous in memory.
  ## This is going to be an extremely wide, shallow data frame, but I think it's what I need.
  type_topic_counts <- word_topic_counts %>% spread(word, topic.count, fill=0)
  ## The first column may now be a redundant "topic" field, but 
  ## if the word "topic" appears in the corpus it may be a meaningful
  ## column.
  
  ## Assign each word type a numeric ID, and then copy those IDs into the tokens.
  word_ids <- data_frame(word = names(type_topic_counts)) %>% mutate(word_id = row_number())
  vocab_size <- nrow(word_ids)
  
  return(list(num.topics = num.topics, alpha = rep(alphaSum / num.topics, num.topics), beta = beta, beta_sum = vocab_size * beta,
              tokens_per_topic = as.vector(tokens_per_topic$count), type_topic_counts = type_topic_counts, word_ids = word_ids))
}

random_topics <- function(tokens_df, num.topics) {
  topics.list <- 1:num.topics
  return(mutate(tokens_df, topic = sample(topics.list, nrow(tokens_df), replace=TRUE)))
}

add_word_ids <- function(tokens_df, topic_model) {
  return(tokens_df %>% left_join(topic_model[["word_ids"]], by = "word"))
}

sample_topics <- function(tokens_df, topic_model, num_iterations=200, burn_in=100, thinning=10) {
  for (iter in 1:num_iterations) {
    tokens_df <- tokens_df %>% mutate(word_id = word_id, topic = sampleTopics(word_id, topic, topic_model))
    print(iter)
  }
  
  return(tokens_df)
}

topic_words <- function(tokens_df) {
  return(tokens_df %>% group_by(topic, word) %>% summarise(count=n()) %>% arrange(topic, -count))
}

doc_topics <- function(tokens_df) {
  return(tokens_df %>% group_by(DocID, topic) %>% summarise(count=n()) %>% arrange(DocID, -count))
}
