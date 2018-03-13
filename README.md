# genetic-programming
C++ implementation of genetic programming

The problem is : Given N points, find an equation (it could be nonlinear equation) that is minimizing the MSE.

We solve it using genetic programming. Here's what we do:
1. Number of chromosomes for each population is 10 chromosomes.
2. Selection : Find two random chromosomes to be parents. Just random, without any probability (like roulette wheel or any other method).
3. Crossover : Find a node from each parent, and then interchange them (the nodes and their subtree). There will be two offsprings.
4. Mutation : Randomly pick a leaf from a random offspring. The mutation process is one of the following: change into a variable, a random constant, or an operator and its two operands.
5. Natural Selection : Take the best 10 from current population.
