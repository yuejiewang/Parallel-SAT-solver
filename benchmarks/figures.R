library(tidyverse)
library(stringr)
library(dplyr)
library(glue)

# phole -> pigeonhole problem DIMACS (UNSAT)
# AIM -> random 3 sat instances with (solution)_vars_clauses.cnf

benchmarks <- readr::read_csv("benchmark_results.csv") %>%
  rename(thread_count = `Thread Count`,
                time = `Time (s)`) %>% 
  mutate(Model = as_factor(Model)) %>% 
  mutate(Benchmark = case_when(
    Benchmark == "phole/01.cnf" ~ "phole/unsat_42_133/01.cnf",
    Benchmark == "phole/02.cnf" ~ "phole/unsat_56_204/02.cnf",
    Benchmark == "phole/03.cnf" ~ "phole/unsat_72_297/03.cnf",
    TRUE ~ Benchmark
  )) %>% 
  separate(Benchmark, into = c("Type", "SAT", "Variables", "Clauses", "Instance"),
           remove = FALSE) %>% 
  mutate(time = if_else(time == -1, 200, time))

plot1_sat <- benchmarks %>% 
  filter(SAT == "sat", thread_count == 1) %>%
  ggplot(aes(x = Benchmark, y = time, group = Model)) +
  geom_line(aes(color = Model)) +
  ggtitle("Random SAT instances versus lingeling") +
  ylab("Time (seconds)") +
  geom_point() + 
  xlab(NULL) +
  theme(axis.text.x = element_text(angle = 90)) +
  
ggsave("plingeling_sat.png", plot1_sat)

plot2_unsat <- benchmarks %>% 
  filter(SAT == "unsat", thread_count == 1) %>%
  filter(str_starts(Model, "dpll")) %>%
  filter(time != 200) %>% 
  ggplot(aes(x = Benchmark, y = time, group = Model)) +
  geom_line(aes(color = Model)) +
  geom_point() + 
  xlab(NULL) +
  theme(axis.text.x = element_text(angle = 90))

get_speedup <- function(df = benchmarks, benchmark, time){
  sequential_time <- df %>%
    filter(Benchmark == benchmark) %>% 
    filter(Model == "dpll-parallel" & thread_count == 1) %>%
    pull(time)
  
  sequential_time/time
}

benchmarks %>% 
  filter(SAT == "sat") %>%
  filter(Model == "dpll-parallel" & thread_count != 64) %>% 
  mutate(time = ifelse(time == 200, NA, time)) %>% 
  filter(Benchmark != "aim/sat_100_340/03.cnf") %>%
  mutate(Speedup = get_speedup(df = ., Benchmark, time)) 


  mutate(Model = as.character(Model)) %>% 
  mutate(Benchmark = as_factor(Benchmark)) %>% 
  ggplot(aes(x = Benchmark, y = Speedup)) +
  geom_line() +
  facet_wrap(~thread_count) +
  geom_point() + 
  xlab(NULL) +
  theme(axis.text.x = element_text(angle = 90))


nqueens_res <- readr::read_csv("nqueens_benchmarks.csv")

nqueens_res <- nqueens_res %>% 
  rename(thread_count = `Thread Count`, time = `Time (s)`) %>% 
  filter(thread_count != 64) %>% 
  filter(str_starts(Model, "dpll"))
