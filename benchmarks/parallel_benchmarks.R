library(readr)
library(tidyr)
library(dplyr)
library(stringr)
library(glue)
library(purrr)
library(ggplot2)
library(forcats)

generate_log_plot <- function(df, title){
  df %>%
    ggplot(aes(x = factor(threads), y = avg_time)) +
    geom_bar(stat="identity", position = position_dodge()) +
    scale_y_continuous(trans='log2') +
    facet_grid(~benchmark) +
    ggtitle(title) +
    ylab("Log of Average Time (seconds)") +
    xlab("Number of Threads") +
    theme_minimal()
}

generate_plot <- function(df, title){
  df %>%
    ggplot(aes(x = factor(threads), y = avg_time)) +
    geom_bar(stat="identity", position = position_dodge()) +
    facet_grid(~benchmark) +
    ggtitle(title) +
    ylab("Average Time (seconds)") +
    xlab("Number of Threads") +
    theme_minimal()
}

get_benchmark <- function(type, thread_exp){
  file_name <- glue("crunchy_{type}/benchmark_large_{type}_p{thread_exp}_b2.txt")
  
  read_csv(file_name, col_names = F) %>%
    separate(col = "X1", into = c("benchmark", "time1", "time1000"), sep = "\\s+") %>% 
    mutate(threads = 2^thread_exp) %>%
    rename(time = time1) %>%
    mutate(time = as.numeric(time)) %>% 
    select(benchmark, time, threads) %>%
    group_by(benchmark, threads) %>%
    summarize(avg_time = mean(time))
}

unsat_benchmarks <- map_dfr(0:6, ~get_benchmark("unsat",.x))  %>%
  ungroup() %>%
  mutate(benchmark = as_factor(benchmark))

unsat_plot <- generate_log_plot(unsat_benchmarks, "UNSAT Benchmarks")
ggsave("unsat_benchmarks_plot.png", plot = unsat_plot)

sat_benchmarks <- map_dfr(0:6, ~get_benchmark("sat",.x))  %>%
  ungroup() %>%
  mutate(benchmark = as_factor(benchmark))

sat_plot <- generate_plot(sat_benchmarks, "SAT Benchmarks")
ggsave("sat_benchmarks_plot.png", plot = sat_plot)

get_speedups <- function(df){
  pmap_dbl(df, function(benchmark, threads, avg_time){
    speedup <- df %>%
      filter(benchmark == !!benchmark & threads == 1) %>%
      pull(avg_time)
    
    speedup / avg_time
  })
}

unsat_speedups <- get_speedups(unsat_benchmarks)

unsat_benchmarks$speedup <- unsat_speedups

unsat_speedup_plot <- unsat_benchmarks %>%
  ggplot(aes(x = threads, y = speedup)) +
  geom_line() +
  geom_point() + 
  facet_grid(~benchmark) +
  ggtitle("UNSAT Speedup") +
  ylab("Speedup") +
  scale_x_continuous("Number of Threads", breaks = c(1,8,16,32,64))

ggsave("unsat_speedup_plot_benchmarks_plot.png", plot = unsat_speedup_plot)

sat_speedups <- get_speedups(sat_benchmarks)

sat_benchmarks$speedup <- sat_speedups

sat_speedup_plot <- sat_benchmarks %>%
  ggplot(aes(x = threads, y = speedup)) +
  geom_line() +
  geom_point() + 
  facet_grid(~benchmark) +
  ggtitle("SAT Speedup") +
  ylab("Speedup") +
  scale_x_continuous("Number of Threads", breaks = c(1,8,16,32,64))

ggsave("sat_speedup_plot_benchmarks_plot.png", plot = sat_speedup_plot)
