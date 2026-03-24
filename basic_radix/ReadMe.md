ok so basically this is a fun toy project i wanted to share
watched interesting vid on radix sort and wanted to impl it, so I did.
Started with C then thought to make it in convoluted cpp for fun
basic_radix.cpp (main) is entirely AI generated but the other files are completely off the dome

only did 4 bit lsd radix sort for simplicty, my revisit later with variable bit and MSD sort, or with optimisation to my algs.
# Debug mode score on my machine (100 mil elems):

it  1 | c  516.898 | cpp 2929.856 | f1  308.329 | f2  868.756 | f3  273.340 | std 8152.978 | ok true
it  2 | c  472.476 | cpp 3055.605 | f1  327.908 | f2  909.107 | f3  278.681 | std 7943.075 | ok true
it  3 | c  457.515 | cpp 2949.246 | f1  312.180 | f2  898.872 | f3  289.261 | std 7911.611 | ok true
it  4 | c  468.775 | cpp 2983.319 | f1  306.734 | f2  870.710 | f3  269.608 | std 7981.170 | ok true
it  5 | c  452.106 | cpp 2932.328 | f1  310.702 | f2  875.245 | f3  272.716 | std 7844.887 | ok true
it  6 | c  452.323 | cpp 3030.086 | f1  311.366 | f2  925.602 | f3  284.964 | std 7970.578 | ok true
it  7 | c  459.774 | cpp 3011.287 | f1  318.972 | f2  928.052 | f3  271.524 | std 7855.998 | ok true
it  8 | c  492.272 | cpp 2949.444 | f1  305.729 | f2  905.729 | f3  272.604 | std 7858.284 | ok true
it  9 | c  450.703 | cpp 2955.854 | f1  311.823 | f2  901.053 | f3  289.924 | std 7930.361 | ok true
it 10 | c  459.661 | cpp 3039.703 | f1  305.924 | f2  886.224 | f3  290.730 | std 7933.118 | ok true

tot
c   4682.503
cpp 29836.727
f1  3119.667
f2  8969.350
f3  2793.354
std 79382.061

avg
c    468.250
cpp 2983.673
f1   311.967
f2   896.935
f3   279.335
std 7938.206


# Release mode scores on my machine (100 mil elems):
it  1 | c  191.076 | cpp  187.130 | f1  141.448 | f2  100.814 | f3  110.136 | std  782.260 | ok true
it  2 | c  187.108 | cpp  171.294 | f1  134.684 | f2   94.880 | f3   92.802 | std  753.637 | ok true
it  3 | c  178.843 | cpp  173.105 | f1  132.405 | f2   94.466 | f3   93.263 | std  753.247 | ok true
it  4 | c  179.414 | cpp  170.339 | f1  131.048 | f2   93.751 | f3   92.892 | std  756.360 | ok true
it  5 | c  199.674 | cpp  192.812 | f1  142.761 | f2  101.539 | f3   93.212 | std  755.141 | ok true
it  6 | c  180.990 | cpp  173.686 | f1  131.964 | f2   93.586 | f3   92.220 | std  763.277 | ok true
it  7 | c  177.384 | cpp  171.248 | f1  131.071 | f2   95.744 | f3   92.338 | std  764.801 | ok true
it  8 | c  179.310 | cpp  171.080 | f1  130.624 | f2   93.835 | f3   91.648 | std  756.046 | ok true
it  9 | c  178.054 | cpp  168.626 | f1  132.098 | f2   93.963 | f3   92.891 | std  749.397 | ok true
it 10 | c  177.615 | cpp  174.907 | f1  130.388 | f2   93.677 | f3   92.498 | std  759.374 | ok true

tot
c   1829.468
cpp 1754.229
f1  1338.490
f2   956.254
f3   943.898
std 7593.540

avg
c    182.947
cpp  175.423
f1   133.849
f2    95.625
f3    94.390
std  759.354