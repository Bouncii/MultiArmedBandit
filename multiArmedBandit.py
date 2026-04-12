import numpy as np
import matplotlib.pyplot as plt

class Bras:
    def __init__(self, possibilites, probas):
        self.possibilites = np.array(possibilites)
        self.probas = np.array(probas)
        self.moyenne = np.sum(self.possibilites * self.probas)

    def pull(self):
        return np.random.choice(self.possibilites, p=self.probas)
    
class MovingBras(Bras):
    def __init__(self, possibilites, fonction_probas):
        self.possibilites = np.array(possibilites)
        self.fonction_probas = fonction_probas
        self.t = 0 
        
        self.probas = np.array(self.fonction_probas(self.t))
        self.moyenne = np.sum(self.possibilites * self.probas)

    def actualiser_etat(self):
        self.t += 1
        self.probas = np.array(self.fonction_probas(self.t))
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


class UCB:
    def __init__(self, n_arms):
        self.n_arms = n_arms
        self.counts = np.zeros(n_arms)
        self.values = np.zeros(n_arms)
        self.t = 0

    def select_arm(self):
        self.t += 1

        if self.t <= self.n_arms:
            return np.argmin(self.counts) #on tire une fois chaque bras pour eviter la division par 0
        
        bonus = np.sqrt(2*np.log(self.t) / self.counts)
        ucb_values = self.values + bonus
        return np.argmax(ucb_values)

    def update(self, arm_index, reward):
        self.counts[arm_index] += 1
        n = self.counts[arm_index]

        moy = self.values[arm_index]
        new_moy = moy + (1 / n) * (reward - moy)
        self.values[arm_index] = new_moy


class EpsilonDecreasing:
    def __init__(self, n_arms, epsilon=1.0):
        self.n_arms = n_arms
        self.epsilon_start = epsilon
        self.t = 0
        
        self.counts = np.zeros(n_arms)
        self.values = np.zeros(n_arms)

    def select_arm(self):
        self.t += 1
        epsilon_t = self.epsilon_start * np.exp(-0.001 * self.t)
        
        if np.random.random() < epsilon_t:
            return np.random.randint(self.n_arms)
        else:
            return np.argmax(self.values)

    def update(self, arm_index, reward):
        self.counts[arm_index] += 1
        n = self.counts[arm_index]
        
        moy = self.values[arm_index]
        new_moy = moy + (1 / n) * (reward - moy)
        self.values[arm_index] = new_moy

class EXP3:
    def __init__(self, n_arms, n_iters):
        self.n_arms = n_arms
        self.n_iters = n_iters
        self.gamma = min(1, np.sqrt(abs(n_arms * np.log(n_arms) / ((np.e - 1) * n_iters))))
        self.weights = np.ones(n_arms)
        self.probs = np.zeros(n_arms)

    def select_arm(self):
        sum_weights = np.sum(self.weights)
        self.probs = (1 - self.gamma) * (self.weights / sum_weights) + (self.gamma / self.n_arms)  
        self.probs /= self.probs.sum() 
        return np.random.choice(self.n_arms, p=self.probs)

    def update(self, arm_index, reward):
        estimated_reward = reward / self.probs[arm_index]
        factor = np.exp(self.gamma * estimated_reward / self.n_arms)
        self.weights[arm_index] *= factor
        if self.weights[arm_index] > 1e100:
            self.weights /= 1e100

def randomNumberArray(n:int):
    rng = np.random.default_rng()
    return rng.random(n)


def randomProbabilityArray(n: int):
    rng = np.random.default_rng()
    q = rng.random(n)
    Q = np.sum(q)
    p = q / Q
    return p


def generate_random_bandit(nb_machines):
    bras_list = []
    
    for _ in range(nb_machines):
        n_outcomes = np.random.randint(2, 6)
        possibilites = randomNumberArray(n_outcomes)
        probas = randomProbabilityArray(n_outcomes)
        bras_list.append(Bras(possibilites, probas))
        
    return MultiArmedBandit(bras_list)

def generate_near_bandit(nb_machines, gap=0.01):
    bras_list = []
    base_mean = 0.5
    for i in range(nb_machines):
        if i == 0 :
            mean = base_mean + gap  
        elif i ==2:
            mean = base_mean - gap  
        else :
            mean = base_mean
        bras_list.append(Bras([0, 1], [1 - mean, mean]))
        
    return MultiArmedBandit(bras_list)

def generate_switching_bandit(nb_machines): # 
    bras_list = []
    for _ in range(nb_machines):
        n_outcomes1 = np.random.randint(2, 6)
        n_outcomes2 = np.random.randint(2, 6)
        poss1 = randomNumberArray(n_outcomes1)
        prob1 = randomProbabilityArray(n_outcomes1)
        poss2 = randomNumberArray(n_outcomes2)
        prob2 = randomProbabilityArray(n_outcomes2)
        bras_list.append(MovingBras(poss1, prob1, poss2, prob2, 0.6)) 
    return MultiArmedBandit(bras_list)


def generate_regime_switch_bandit(nb_machines, total_iters):
    bras_list = []
    mid = total_iters // 2
    
    for i in range(nb_machines):
        def create_p_func(arm_idx):
            def p_regime(t):
                if arm_idx == 0: 
                    if t < mid :
                        p_succes = 0.8
                    else:
                        p_succes =0.1
                elif arm_idx == 1: 
                    if t < mid :
                        p_succes = 0.1
                    else:
                        p_succes =0.8
                else: 
                    p_succes = 0.5
                
                return [1.0 - p_succes, p_succes]
            return p_regime
            
        fonction_probas = create_p_func(i)
        bras_list.append(MovingBras([0, 1], fonction_probas))
        
    return MultiArmedBandit(bras_list)



def test(nb_machines, nb_iter, algo_class, bandit_fixe=None):
    if bandit_fixe is None:
        bandit = generate_random_bandit(nb_machines)
    else:
        bandit = bandit_fixe

    if algo_class == EXP3:
        agent = algo_class(nb_machines, nb_iter)
    else:
        agent = algo_class(nb_machines)
    
    regret_cumule = 0
    historique_regret = []

    for t in range(nb_iter):
        for b in bandit.bras:
            if isinstance(b, MovingBras):
                b.actualiser_etat()
        
        moyennes_instantanées = [b.moyenne for b in bandit.bras]
        meilleure_moyenne_t = max(moyennes_instantanées)
        
        choix = agent.select_arm()
        reward = bandit.pull_bras(choix)
        agent.update(choix, reward)
        
        regret_cumule += meilleure_moyenne_t - bandit.bras[choix].moyenne
        historique_regret.append(regret_cumule)

    return historique_regret, agent, moyennes_instantanées

n_m = 10
n_i = 25000
bandit_commun = generate_regime_switch_bandit(n_m,n_i)
regret_eps, agent_eps, moyennes_eps = test(n_m, n_i, EpsilonGreedy,bandit_commun)
regret_ucb, agent_ucb, moyennes_ucb = test(n_m, n_i, UCB,bandit_commun)
regret_dec, agent_dec, moyennes_dec = test(n_m, n_i, EpsilonDecreasing, bandit_commun)
regret_exp3, agent_exp3, moyennes_exp3 = test(n_m, n_i, EXP3, bandit_commun)


plt.figure(figsize=(10, 6))
plt.plot(regret_eps, label="Epsilon-Greedy (ε=0.1)", color='red')
plt.plot(regret_dec, label="Epsilon-Decreasing", color='green', linewidth=2)
plt.plot(regret_ucb, label="UCB1", color='blue')
plt.plot(regret_exp3, label="EXP3 (Adversarial)", color='orange', linewidth=2)

plt.title(f"Regret Cumulé : Multi Armed Bandit ({n_m} machines)")
plt.xlabel("Itérations")
plt.ylabel("Regret Cumulé")
plt.legend()
plt.grid(True, linestyle='--', alpha=0.7)
plt.show()

print("\n" + "-"*45)
print("DÉTAILS DES MACHINES (BRAS)")
print("-"*45)

nb_bras = len(moyennes_eps)
index_meilleur = np.argmax(moyennes_eps)

for i in range(nb_bras):
    m_reelle = moyennes_eps[i]
    suffixe = " <--- MEILLEUR RÉEL" if i == index_meilleur else ""
    
    print(f"Bras n°{i} | Réelle: {m_reelle:.4f} {suffixe}")

print("\n" + "-"*45)
print("BILAN DES PERFORMANCES (REGRET FINAL)")
print("-"*45)
print(f"Epsilon-Greedy (0.1)  : {regret_eps[-1]:.2f}")
print(f"Epsilon-Decreasing : {regret_dec[-1]:.2f}")
print(f"UCB1 : {regret_ucb[-1]:.2f}")
print(f"EXP3 : {regret_exp3[-1]:.2f}")

choix_eps = np.argmax(agent_eps.values)
choix_dec = np.argmax(agent_dec.values)
choix_ucb = np.argmax(agent_ucb.values)
choix_exp3 = np.argmax(agent_exp3.weights)

print(f"Vrai meilleur bras (théorique) : n°{index_meilleur}")
print(f"Choix final Epsilon-Greedy : n°{choix_eps}")
print(f"Choix final Epsilon-Decreasing : n°{choix_dec}")
print(f"Choix final UCB1 : n°{choix_ucb}")
print(f"Choix final EXP3 : n°{choix_exp3}")