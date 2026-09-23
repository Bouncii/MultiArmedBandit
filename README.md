# MultiArmedBandit
## 📌 Overview

This repository contains a Python implementation of reinforcement learning algorithms focused on solving the Multi-Armed Bandit (MAB) problem. These algorithms are then applied to create an intelligent agent (AI) capable of playing and optimizing its choices in a Gomoku game environment.

## Features

The project explores the exploration-exploitation dilemma through several classic bandit algorithms:

* Epsilon-Greedy: Balances random exploration and choosing the best-known option.

* Upper Confidence Bound (UCB): Uses optimism in the face of uncertainty for smarter exploration.

* Exp3: An adversarial multi-armed bandit algorithm that chooses actions probabilistically based on weights updated exponentially according to their rewards.

### Game AI Application

The repository includes a practical application where the algorithms are used as the core decision-making brain with the Monte Carlo Tree Search (MCTS) for a Gomoku game AI, allowing the agent to learn the most rewarding strategies over time.

## Installation

Clone this repository:

`git clone https://github.com/Bouncii/MultiArmedBandit.git`

Install the required dependencies:

`pip install numpy matplotlib`

## Usage

Run the main simulation to see the agents in action and compare their performances:

`python multiArmedBandit.py`

## Visualizations

The code outputs performance graphs (such as cumulative reward and average regret over time) to easily compare how Epsilon-Greedy, UCB, and Thompson Sampling perform against each other in the game environment.

## 📜 License

Distributed under the MIT License. See LICENSE for more information.
