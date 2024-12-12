import matplotlib.pyplot as plt
import re

def parse_results(filename):
    results = {}
    with open(filename, 'r') as f:
        for line in f:
            match = re.match(r"(Sequential|Parallel) (sum|average|min/max|euclidean norm|manhattan norm|dot product): (\d+)", line)
            if match:
                test_type = match.group(1)
                operation = match.group(2)
                time = int(match.group(3))
                if operation not in results:
                  results[operation] = {'Sequential': [], 'Parallel': []}
                results[operation][test_type].append(time)
    return results

def plot_results(results):
    for operation, times in results.items():
        iterations = range(1, len(times['Sequential']) + 1)
        plt.plot(iterations, times['Sequential'], marker='o', label='Sequential')
        plt.plot(iterations, times['Parallel'], marker='x', label='Parallel')
        plt.xlabel('Iteration')
        plt.ylabel('Time (ms)')
        plt.title(f'{operation} Performance')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'{operation}_plot.png')  # Сохраняем график в файл
        plt.show()

if __name__ == "__main__":
    results = parse_results("results.txt")
    plot_results(results)