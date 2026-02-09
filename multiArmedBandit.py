import numpy as np

class Bras:
    def __init__(self, possibilites, probas):
        self.possibilites = possibilites
        self.probas = probas


    def pull(self):
        return np.random.choice(self.possibilites, p=self.probas)






class MultiArmedBandit:
    def __init__(self, bras):
        self.bras = bras

    def pull_bras(self, bras_index):
        if 0 <= bras_index < len(self.bras):
            return self.bras[bras_index].pull()
        print("Bras invalide")


bras_0 = Bras([0.3, 0.2, 0.0], [0.9, 0.02, 0.08])
bras_1 = Bras([0.1, 0.5, 0.9], [0.2, 0.3, 0.5])
bras_2 = Bras([1.0, 0.0], [0.1, 0.9])

bandit = MultiArmedBandit([bras_0, bras_1, bras_2])

for i in range(10):
    print(bandit.pull_bras(2))