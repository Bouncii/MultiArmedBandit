import numpy as np


class Bras:
    def __init__(self, possibilites, probas):
        self.possibilites = np.array(possibilites)
        self.probas = np.array(probas)
        self.moyenne = np.sum(self.possibilites * self.probas)

    def pull(self):
        return np.random.choice(self.possibilites, p=self.probas)

class MultiArmedBandit:
    def __init__(self, bras):
        self.bras = bras

    def pull_bras(self, bras_index):
        return self.bras[bras_index].pull()


class EpsilonGreedy:
    def __init__(self, n_arms, epsilon=0.1):
        self.n_arms = n_arms
        self.epsilon = epsilon
        
        self.counts = np.zeros(n_arms)
        self.values = np.zeros(n_arms)

    def select_arm(self):
        if np.random.random() < self.epsilon:
            return np.random.randint(self.n_arms)
        else:
            return np.argmax(self.values)

    def update(self, arm_index, reward):
        self.counts[arm_index] += 1
        n = self.counts[arm_index]
        
        moy = self.values[arm_index]
        new_moy = moy + (1 / n) * (reward - moy)
        self.values[arm_index] = new_moy





bras_0 = Bras([0.05, 0.1, 0.15, 0.2 ], [0.3, 0.4, 0.2, 0.1])
bras_1 = Bras([0.1, 0.12, 0.9], [0.45, 0.5, 0.05])
bras_2 = Bras([0.0, 0.3], [0.5, 0.5])

bras_list = [bras_0, bras_1, bras_2]
bandit = MultiArmedBandit(bras_list)
agent = EpsilonGreedy(3, 0.1)

moyennes = [b.moyenne for b in bras_list]
meilleure_moyenne = max(moyennes)
meilleur_bras_theorique = np.argmax(moyennes)

n_iterations = 1000
regret = 0
for i in range(n_iterations):
    bras = agent.select_arm()
    reward = bandit.pull_bras(bras)
    agent.update(bras, reward)

    regret += meilleure_moyenne - bras_list[bras].moyenne

print("Résultats après", n_iterations, "itérations :")
for i, val in enumerate(agent.values):
    print(f"Bras {i} : Estimé = {val} | Tiré {int(agent.counts[i])} fois")

best_arm = np.argmax(agent.values)
print(f"L'agent pense que le meilleur bras est le n°{best_arm}")
print(f"le regret est de : {regret}")